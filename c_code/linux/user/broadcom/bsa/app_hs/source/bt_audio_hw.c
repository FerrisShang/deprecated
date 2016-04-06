/* for X1000, DMIC is dsp3, BSA is dsp1, CODEC is dsp
 * read BSA dsp1  (read_bt_thread_func) ----> write CODEC dsp (write_codec_thread_func)
 * write BSA dsp1 (write_bt_thread_func) <----- read DMIC dsp3 (read_dmic_thread_func)
 */
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <sys/time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include "llist.h"
#include "bt_audio_hw.h"
#include "sharememory_interface.h"
#include "ini_interface.h"

static int read_bt_first_run = 0;
static int read_dmic_first_run = 0;
hs_resample_msg hs_rmsg;
bt_aec_resample_msg bt_aec_rmsg;

bt_aec_resample_init_data bt_aec_rdata = {
	.resample_rate = HS_SAMPLE_RATE,
	.resample_channel = HS_SAMPLE_CHANNEL,
	.resample_bits = HS_SAMPLE_BITS,
	.resample_enable = 0,
	.aec_resample_data = NULL,
};

hs_resample_init_data hs_resample_data = {
	.resample_rate = HS_SAMPLE_RATE,
	.resample_channel = HS_SAMPLE_CHANNEL,
	.resample_bits = HS_SAMPLE_BITS,
	.resample_enable = 0,
	.mozart_hs_resample_data = NULL,
};

AEC_DEV aec_dev = {
	.frame_len = 0,
	.time = 0,
	.has_init = 0,
	.aec_enable = 0,
	.buf_play = NULL,
	.buf_record = NULL,
	.aec_init = NULL,
	.aec_destroy = NULL,
	.aec_get_enable = NULL,
	.aec_get_buffer_length = NULL,
	.aec_calculate = NULL,
};

pthread_cond_t dstream_rdy_sig = PTHREAD_COND_INITIALIZER;
pthread_cond_t ustream_rdy_sig = PTHREAD_COND_INITIALIZER;
pthread_mutex_t dstream_rdy_lock;
pthread_mutex_t ustream_rdy_lock;

static ADnode *get_free_node(ADstream *stream)
{
	struct list_head *phead = &stream->free_node_list;
	ADnode *node = NULL;

	pthread_mutex_lock(&stream->slock);
	if (list_empty(phead)) {
		pthread_mutex_unlock(&stream->slock);
		return NULL;
	}

	node = list_entry(phead->next, ADnode, list);
	list_del(phead->next);
	stream->free_cnt--;
	pthread_mutex_unlock(&stream->slock);

	return node;
}

static void put_free_node(ADstream *stream, ADnode *node)
{
	struct list_head *phead = &stream->free_node_list;

	pthread_mutex_lock(&stream->slock);
	node->size = 0;
	list_add_tail(&node->list, phead);
	stream->free_cnt++;
	pthread_mutex_unlock(&stream->slock);
}

static void put_use_node(ADstream *stream, ADnode *node, int size)
{
	struct list_head *phead = &stream->use_node_list;

	pthread_mutex_lock(&stream->slock);
	node->size = size;
	list_add_tail(&node->list, phead);
	stream->use_cnt++;
	pthread_mutex_unlock(&stream->slock);
}

static ADnode *get_use_node(ADstream *stream)
{
	struct list_head *phead = &stream->use_node_list;
	ADnode *node = NULL;

	pthread_mutex_lock(&stream->slock);
	if (list_empty(phead)) {
		pthread_mutex_unlock(&stream->slock);
		return NULL;
	}

	node = list_entry(phead->next, ADnode, list);
	list_del(phead->next);
	stream->use_cnt--;
	pthread_mutex_unlock(&stream->slock);

	return node;
}

static inline int get_node_cnt_bylist(ADstream *stream, struct list_head *phead)
{
	ADnode *tmp = NULL;
	int cnt = 0;

	pthread_mutex_lock(&stream->slock);
	list_for_each_entry(tmp, phead, list)
	cnt++;
	pthread_mutex_unlock(&stream->slock);

	return cnt;
}

static int enable_aec_and_start_record(int dsp_play_r_fd, int dsp_record_r_fd)
{
	int ret = 0;
	void *buf;
	unsigned int mode = 0;

	printf("DEBUG: enable_aec_and_start_record !\n");
	buf = malloc(128);
	/* Enable AEC function */
	mode = 1;
	ret = ioctl(dsp_play_r_fd, SNDCTL_DSP_AEC_ENABLE, &mode);
	if (ret != 0) {
		printf("DSP_AEC_ENABLE is not ready\n");
		return -1;
	}
	/* First read dsp */
	ret = read(dsp_play_r_fd, buf, 128);
	if (ret != 0) {
		printf("First read dsp_play_r_fd not ready.\n");
		ret = -1;
		return -1;
	}

	ret = read(dsp_record_r_fd, buf, 128);
	if (ret != 0) {
		printf("First read dsp_record_r_fd not ready.\n");
		ret = -1;
		return -1;
	}

	/* Start record */
	ret = ioctl(dsp_play_r_fd, SNDCTL_DSP_AEC_START, &mode);
	if (ret != 0) {
		printf("DSP_AEC_START is not ready\n");
		return -1;
	}

	if(buf != NULL) {
		free(buf);
		buf = NULL;
	}

	return 1;
}

static void *read_dmic_thread_func(void *_dev)
{
	struct device_prope *DevPrope = _dev;
	ADnode *node = NULL;
	int size = 0;
	char bufloop[RECORD_BUFSZ] = {0};
	int aec_resample_length = 0;

	if (DevPrope->dsp_record_r_fd <= 0 || DevPrope->dsp_play_r_fd <= 0) {
		printf("DevPrope->dsp_record_r_fd or DevPrope->dsp_play_r_fd <= 0!\n");
		return NULL;
	}

	if(aec_dev.aec_enable) {
		aec_dev.buf_record = calloc(1, aec_dev.frame_len);
		if(aec_dev.buf_record == NULL) {
			printf("buf_record calloc error !\n");
			return NULL;
		}
		if (bt_aec_rdata.resample_enable) {
			aec_resample_length = bt_aec_rdata.resample_rate * bt_aec_rdata.resample_bits / 8 * bt_aec_rdata.resample_channel * HS_SAMPLE_TIME / 1000;
			aec_dev.buf_play  = calloc(1, aec_resample_length);
		} else {
			aec_dev.buf_play  = calloc(1, aec_dev.frame_len);
		}
		if(aec_dev.buf_play == NULL) {
			printf("buf_play calloc error !\n");
			return NULL;
		}

		aec_dev.has_init = 1;
		if(!enable_aec_and_start_record(DevPrope->dsp_play_r_fd, DevPrope->dsp_record_r_fd)) {
			printf("enable_aec_and_start_record failed!\n");
		}
		// read 49ms data  for  align
		read(DevPrope->dsp_play_r_fd, bufloop, 30*32);
		read(DevPrope->dsp_play_r_fd, bufloop, 19*32);
	}

	while (DevPrope->read_dmic_call) {
		node = get_free_node(&bt_dstream);
		if (node != NULL) {
			if (aec_dev.aec_enable) {
				if (bt_aec_rdata.resample_enable) {
					size = read(DevPrope->dsp_play_r_fd, aec_dev.buf_play, aec_resample_length);
					size = read(DevPrope->dsp_record_r_fd, aec_dev.buf_record, aec_dev.frame_len);
					bt_aec_rmsg.in_buffer = aec_dev.buf_play;
					bt_aec_rmsg.in_len = aec_resample_length;
					bt_aec_rdata.aec_resample_data(&bt_aec_rmsg);
					if (!bt_aec_rmsg.out_buffer) {
						printf("ERROR: bt_aec_rmsg.out_buffer == NULL, please checked!\n");
						return NULL;
					}
					if (!bt_aec_rmsg.out_len) {
						printf("ERROR: bt_aec_rmsg.out_len == 0, please checked!\n");
						return NULL;
					}
					aec_dev.aec_calculate(aec_dev.buf_record, bt_aec_rmsg.out_buffer, node->addr, bt_aec_rmsg.out_len);
				}else {
					size = read(DevPrope->dsp_play_r_fd, aec_dev.buf_play, aec_dev.frame_len);
					size = read(DevPrope->dsp_record_r_fd, aec_dev.buf_record, aec_dev.frame_len);
					aec_dev.aec_calculate(aec_dev.buf_record, aec_dev.buf_play, node->addr, aec_dev.frame_len);
				}
			} else {
				size = read(DevPrope->dsp_record_r_fd, node->addr, AD_NODE_BUFSIZ);
			}
			if (size > 0) {
				put_use_node(&bt_dstream, node, size);
				size = 0;
				if (read_dmic_first_run) {
					if (bt_dstream.use_cnt >= 3)
						pthread_cond_signal(&dstream_rdy_sig);
				} else {
					if (bt_dstream.use_cnt >= 1)
						pthread_cond_signal(&dstream_rdy_sig);
				}
			}
		} else {
			pthread_mutex_lock(&dstream_rdy_lock);
			if (bt_dstream.free_cnt <= 0) {
				pthread_cond_wait(&dstream_rdy_sig, &dstream_rdy_lock);
			}
			pthread_mutex_unlock(&dstream_rdy_lock);
		}
	}
	return NULL;
}

static void *write_bt_thread_func(void *_dev)
{
	struct device_prope *DevPrope = _dev;
	ADnode *node = NULL;

	if (DevPrope->dsp1_w_fd <= 0)
		return NULL;

	while (read_dmic_first_run) {
		if (bt_dstream.use_cnt < 3) {
			usleep(200);
		} else {
			read_dmic_first_run = 0;
		}
	}

	while (DevPrope->write_bt_call) {
		node = get_use_node(&bt_dstream);
		if (node != NULL) {
			write(DevPrope->dsp1_w_fd, node->addr, node->size);
			put_free_node(&bt_dstream, node);
			if (bt_dstream.free_cnt == 1)
				pthread_cond_signal(&dstream_rdy_sig);
		} else {
			pthread_mutex_lock(&dstream_rdy_lock);
			if (bt_dstream.use_cnt <= 0) {
				pthread_cond_wait(&dstream_rdy_sig, &dstream_rdy_lock);
			}
			pthread_mutex_unlock(&dstream_rdy_lock);
		}
	}

	return NULL;
}

static void *read_bt_thread_func(void *_dev)
{
	struct device_prope *DevPrope = _dev;
	ADnode *node = NULL;
	int size = 0;

	if (DevPrope->dsp1_r_fd <= 0)
		return NULL;

	while (DevPrope->read_bt_call) {
		node = get_free_node(&bt_ustream);
		if (node != NULL) {
			if(aec_dev.aec_enable)
				size = read(DevPrope->dsp1_r_fd, node->addr, aec_dev.frame_len);
			else {
				size = read(DevPrope->dsp1_r_fd, node->addr, AD_NODE_BUFSIZ);
			}
			if (size > 0) {
				put_use_node(&bt_ustream, node, size);
				size = 0;
				if (read_bt_first_run) {
					if (bt_ustream.use_cnt >= 3)
						pthread_cond_signal(&ustream_rdy_sig);
				} else {
					if (bt_ustream.use_cnt >= 1)
						pthread_cond_signal(&ustream_rdy_sig);
				}
			}
		} else {
			pthread_mutex_lock(&ustream_rdy_lock);
			if (bt_ustream.free_cnt <= 0) {
				pthread_cond_wait(&ustream_rdy_sig, &ustream_rdy_lock);
			}
			pthread_mutex_unlock(&ustream_rdy_lock);
		}
	}

	return NULL;
}

static void *write_codec_thread_func(void *_dev)
{
	struct device_prope *DevPrope = _dev;
	ADnode *node = NULL;

	if (DevPrope->dsp_play_w_fd <= 0)
		return NULL;
	while (read_bt_first_run) {
		if (bt_ustream.use_cnt < 3) {
			usleep(200);
		} else {
			read_bt_first_run = 0;
		}
	}
	while (DevPrope->write_codec_call) {
		node = get_use_node(&bt_ustream);
		if (node != NULL) {
			if (hs_resample_data.resample_enable) {
				if (hs_resample_data.mozart_hs_resample_data) {
					hs_rmsg.in_buffer = node->addr;
					hs_rmsg.in_len = node->size;
					hs_resample_data.mozart_hs_resample_data(&hs_rmsg);
				}
			} else {
				hs_rmsg.out_buffer = node->addr;
				hs_rmsg.out_len = node->size;
			}
			if (!hs_rmsg.out_buffer) {
				printf("ERROR: hs_rmsg.out_buffer == NULL, please set hs_rmsg.out_buffer !!!\n");
				return NULL;
			}

			write(DevPrope->dsp_play_w_fd, hs_rmsg.out_buffer, hs_rmsg.out_len);
			put_free_node(&bt_ustream, node);
			if (bt_ustream.free_cnt == 1)
				pthread_cond_signal(&ustream_rdy_sig);
		} else {
			pthread_mutex_lock(&ustream_rdy_lock);
			if (bt_ustream.use_cnt <= 0) {
				pthread_cond_wait(&ustream_rdy_sig, &ustream_rdy_lock);
			}
			pthread_mutex_unlock(&ustream_rdy_lock);
		}
	}

	return NULL;
}

static void bt_stream_init(void)
{
	int i = 0;
	ADnode *node = NULL;
	int buffer_length = 0;
	if(aec_dev.aec_enable) {
		buffer_length = aec_dev.frame_len * AD_NODE_SUM;
		if(!buffer_length) {
			printf("bt_dstream buffer_length is 0!\n");
			return;
		}
	}

	if (bt_dstream.mem == NULL) {
		if(aec_dev.aec_enable)
			bt_dstream.mem = (char *)calloc(1, 2 * buffer_length);
		else
			bt_dstream.mem = (char *)calloc(1, 2 * AD_STREAM_MEMSIZ);
		if (!bt_dstream.mem) {
			printf(" calloc bt mem error! ");
			return;
		} else {
			if(aec_dev.aec_enable)
				bt_ustream.mem = bt_dstream.mem + buffer_length;
			else
				bt_ustream.mem = bt_dstream.mem + AD_STREAM_MEMSIZ;
		}
	}

	list_init(&bt_dstream.free_node_list);
	list_init(&bt_ustream.free_node_list);
	list_init(&bt_dstream.use_node_list);
	list_init(&bt_ustream.use_node_list);

	for (i = 0; i < AD_NODE_SUM; i++) {
		node = &bt_dstream.node[i];
		if(aec_dev.aec_enable)
			node->addr = bt_dstream.mem + i * aec_dev.frame_len;
		else
			node->addr = bt_dstream.mem + i * AD_NODE_BUFSIZ;
		node->size = 0;
		list_add(&node->list, &bt_dstream.free_node_list);
	}

	for (i = 0; i < AD_NODE_SUM; i++) {
		node = &bt_ustream.node[i];
		if(aec_dev.aec_enable)
			node->addr = bt_ustream.mem + i * aec_dev.frame_len;
		else
			node->addr = bt_ustream.mem + i * AD_NODE_BUFSIZ;
		node->size = 0;
		list_add(&node->list, &bt_ustream.free_node_list);
	}

	bt_dstream.free_cnt = AD_NODE_SUM;
	bt_ustream.free_cnt = AD_NODE_SUM;
	bt_dstream.use_cnt = 0;
	bt_ustream.use_cnt = 0;
}

/*set record route*/
static void set_record_route(struct device_prope *DevPrope)
{
	int mixer_fd;
	int vol = 80;
	char buf[8] = {};
	int mode = SOUND_MIXER_DIGITAL1;

	strcpy(DevPrope->device_node_mixer, "/dev/mixer");
	if(DevPrope->device_node_dsp[8] != '\0') {
		sprintf(DevPrope->device_node_mixer,
				"%s%c",
				DevPrope->device_node_mixer,
				DevPrope->device_node_dsp[8]);
	}
	if ((mixer_fd = open(DevPrope->device_node_mixer, O_RDWR | O_NONBLOCK)) < 0) {
		perror("open /dev/mixer failed !!");
		return;
	} else {
		if (mozart_ini_getkey("/usr/data/system.ini", "product", "cpu", buf)) {
			perror("mozart_ini_getkey failed!\n");
			close(mixer_fd);
			return;
		}
		if (!strcasecmp(buf, "m150")) {
			/* select dmic or smic */
			if (ioctl(mixer_fd, SOUND_MIXER_WRITE_RECSRC, &mode) == -1) {
				perror("set record route failed!");
			}
		}
		/* adjust record volume */
		if (ioctl(mixer_fd, SOUND_MIXER_WRITE_MIC, &vol) == -1) {
			perror("set record volume failed!\n");
		}
		close(mixer_fd);
	}
}

static int config_device(int fd, int format, int config_channel, int speed)
{
	int ioctl_val;

	/* NOTE: config order is important: format, CHANNEL, speed */
	/* set bit format */
	ioctl_val = format;
	if (ioctl(fd, SNDCTL_DSP_SETFMT, &ioctl_val) == -1) {
		fprintf(stderr, "set format failed: %s\n", strerror(errno));
		return -1;
	}
	if (ioctl_val != format) {
		fprintf(stderr, "format not supported, changed by driver to 0x%08x\n",
				ioctl_val);
		return -1;
	}

	/*set CHANNEL */
	ioctl_val = config_channel;
	if ((ioctl(fd, SNDCTL_DSP_CHANNELS, &ioctl_val)) == -1) {
		fprintf(stderr, "set config_channel failed: %s\n", strerror(errno));
		return -1;
	}
	if (ioctl_val != config_channel) {
		fprintf(stderr, "%d CHANNEL is not supported, changed to %d by driver\n",
				config_channel, ioctl_val);
		return -1;
	}

	/*set speed */
	ioctl_val = speed;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &ioctl_val) == -1) {
		fprintf(stderr, "set speed failed: %s\n", strerror(errno));
		return -1;
	}
	if (ioctl_val != speed) {
		fprintf(stderr, "speed %d is not supported, changed to %d by driver\n",
				speed, ioctl_val);
		return -1;
	}

	return 0;
}

int bt_end_call(struct device_prope *DevPrope)
{
	int ret = 0;

	read_bt_first_run = 0;
	read_dmic_first_run = 0;

	DevPrope->read_bt_call = FALSE;
	DevPrope->read_dmic_call = FALSE;
	DevPrope->write_bt_call = FALSE;
	pthread_cond_signal(&dstream_rdy_sig);
	pthread_cond_signal(&ustream_rdy_sig);

	while(bt_ustream.use_cnt > 0) {
		usleep(1000);
	}

	DevPrope->write_codec_call = FALSE;
	pthread_cond_signal(&ustream_rdy_sig);

	if (DevPrope->dsp_record_r_fd > 0) {
		pthread_join(bt_dev.dr_thread, NULL);
		if (close(DevPrope->dsp_record_r_fd) < 0) {
			perror("close read dsp record:");
			return -1;
		}
		printf("close read dmic record\n");
		DevPrope->dsp_record_r_fd = -1;
	}

	if (DevPrope->dsp1_w_fd > 0) {
		if (ioctl(DevPrope->dsp1_w_fd, SNDCTL_EXT_STOP_DMA, &ret) < 0)
			printf("%s ioctl set SNDCTL_EXT_STOP_DMA failed !\n", __func__);
		pthread_join(bt_dev.uw_thread, NULL);
		if (close(DevPrope->dsp1_w_fd) < 0) {
			perror("close write dsp1:");
			return -1;
		}
		printf("close write dsp1\n");
		DevPrope->dsp1_w_fd = -1;
	}
	if (DevPrope->dsp1_r_fd > 0) {
		if (ioctl(DevPrope->dsp1_r_fd, SNDCTL_EXT_STOP_DMA, &ret) < 0)
			printf("%s ioctl set SNDCTL_EXT_STOP_DMA failed !\n", __func__);
		pthread_join(bt_dev.ur_thread, NULL);
		if (close(DevPrope->dsp1_r_fd) < 0) {
			perror("close read dsp1:");
			return -1;
		}
		printf("close read dsp1\n");
		DevPrope->dsp1_r_fd = -1;
	}

	if (DevPrope->dsp_play_w_fd > 0) {
		pthread_join(bt_dev.dw_thread, NULL);
		if (ioctl(DevPrope->dsp_play_w_fd, SNDCTL_DSP_SYNC, &ret) < 0)
			printf("%s ioctl set SNDCTL_DSP_SYNC failed !\n", __func__);
		if (close(DevPrope->dsp_play_w_fd) < 0) {
			perror("close write dsp:");
			return -1;
		}
		printf("close write dsp\n");
		DevPrope->dsp_play_w_fd = -1;
	}

	if (DevPrope->dsp_play_r_fd > 0) {
		if (close(DevPrope->dsp_play_r_fd) < 0) {
			perror("close read dsp:");
			return -1;
		}
		printf("close read dsp\n");
		DevPrope->dsp_play_r_fd = -1;
	}

	if (bt_dstream.mem != NULL) {
		free(bt_dstream.mem);
		bt_dstream.mem = NULL;
	}

	if(aec_dev.has_init) {
		aec_dev.has_init = 0;
		ret = 0;
		ioctl(DevPrope->dsp_play_r_fd, SNDCTL_DSP_AEC_ENABLE, &ret);
		if(aec_dev.aec_destroy)
			aec_dev.aec_destroy();
		free(aec_dev.buf_record);
		free(aec_dev.buf_play);
		aec_dev.buf_record = NULL;
		aec_dev.buf_play = NULL;
	}

	if (hs_resample_data.resample_enable) {
		if (hs_resample_data.mozart_hs_resample_data) {
			if ((hs_rmsg.out_buffer) && (hs_rmsg.out_buffer != hs_rmsg.in_buffer)) {
				printf("free hs_rmsg.out_buffer!\n");
				free(hs_rmsg.out_buffer);
				hs_rmsg.out_buffer = NULL;
			}
		}
	}
	return 0;
}

void mozart_aec_callback(bt_aec_callback *bt_ac)
{
	if (bt_ac) {
		aec_dev.aec_get_enable = bt_ac->aec_enable;
		aec_dev.aec_get_buffer_length = bt_ac->aec_get_buffer_length;
		aec_dev.aec_calculate = bt_ac->aec_calculate;
		aec_dev.aec_init = bt_ac->aec_init;
		aec_dev.aec_destroy = bt_ac->aec_destroy;
	}
}

void mozart_get_hs_default_sampledata(hs_sample_init_data *sample_data)
{
	sample_data->sample_rate = HS_SAMPLE_RATE;
	sample_data->sample_channel = HS_SAMPLE_CHANNEL;
	sample_data->sample_bits = HS_SAMPLE_BITS;
}

void mozart_set_hs_sampledata_callback(hs_resample_init_data *resample_data)
{
	if (resample_data) {
		hs_resample_data.resample_rate = resample_data->resample_rate;
		hs_resample_data.resample_bits = resample_data->resample_bits;
		hs_resample_data.resample_channel = resample_data->resample_channel;
		hs_resample_data.resample_enable = resample_data->resample_enable;
		hs_resample_data.mozart_hs_resample_data = resample_data->mozart_hs_resample_data;
		printf("mozart_set_hs_sampledata: resample_rate = %d, resample_bits = %d, resample_channel = %d, resample_enable = %d.\n",
			hs_resample_data.resample_rate, hs_resample_data.resample_bits, hs_resample_data.resample_channel, hs_resample_data.resample_enable);
	}
}

void mozart_get_bt_aec_default_sampledata(bt_aec_sample_init_data *sample_data)
{
	sample_data->sample_rate = HS_SAMPLE_RATE;
	sample_data->sample_channel = HS_SAMPLE_CHANNEL;
	sample_data->sample_bits = HS_SAMPLE_BITS;
}
void mozart_set_bt_aec_sampledata_callback(bt_aec_resample_init_data *resample_data)
{
	if (resample_data) {
		bt_aec_rdata.resample_rate = resample_data->resample_rate;
		bt_aec_rdata.resample_bits = resample_data->resample_bits;
		bt_aec_rdata.resample_channel = resample_data->resample_channel;
		bt_aec_rdata.resample_enable = resample_data->resample_enable;
		bt_aec_rdata.aec_resample_data = resample_data->aec_resample_data;
		printf("mozart_set_bt_aec_sampledata: resample_rate = %d, resample_bits = %d, resample_channel = %d, resample_enable = %d.\n",
			bt_aec_rdata.resample_rate, bt_aec_rdata.resample_bits, bt_aec_rdata.resample_channel, bt_aec_rdata.resample_enable);
	}
}
int bt_start_call(struct device_prope *DevPrope)
{
	char buf[32] = {};

	read_bt_first_run = 1;
	read_dmic_first_run = 1;
	DevPrope->write_bt_call = TRUE;
	DevPrope->write_codec_call = TRUE;
	DevPrope->read_dmic_call = TRUE;
	DevPrope->read_bt_call = TRUE;
	if (aec_dev.aec_get_enable) {
		aec_dev.aec_enable = aec_dev.aec_get_enable();
	}

	if(aec_dev.aec_enable) {
		if(aec_dev.aec_init)
			aec_dev.aec_init();
		if (bt_aec_rdata.resample_enable) {
			memset(&bt_aec_rmsg, 0, sizeof(bt_aec_resample_msg));
		}
	}

	if (aec_dev.aec_get_buffer_length) {
		/* get frame_len */
		aec_dev.frame_len = aec_dev.aec_get_buffer_length();
		if(aec_dev.frame_len <= 0) {
			printf("aec_dev.frame_len <= 0, please checked!\n");
			return -1;
		}
	}
	if (hs_resample_data.resample_enable) {
		memset(&hs_rmsg, 0, sizeof(hs_resample_msg));
	}
	bt_stream_init();

        // Parse pcm dsp device.
	if (mozart_ini_getkey("/usr/data/system.ini", "audio", "dev_pcm", buf)) {
		printf("[app_hs - OSS] Can't get pcm dsp device, force to /dev/dsp1.\n");
		strcpy(buf, "/dev/dsp1");
	}

	DevPrope->dsp1_w_fd = open(buf, O_WRONLY);
	if (DevPrope->dsp1_w_fd < 0) {
		printf("wopen pcm dsp(%s) error: %s.\n", buf, strerror(errno));
		DevPrope->dsp1_w_fd = -1;
		return -1;
	}

	DevPrope->dsp1_r_fd = open(buf, O_RDONLY);
	if (DevPrope->dsp1_r_fd < 0) {
		printf("ropen pcm dsp(%s) error: %s.\n", buf, strerror(errno));
		DevPrope->dsp1_r_fd = -1;
		return -1;
	}

	if (mozart_ini_getkey("/usr/data/system.ini", "audio", "dev_record", DevPrope->device_node_dsp)) {
		printf("failed to parse /usr/data/system.ini, force to /dev/dsp2\n");
		strcpy(DevPrope->device_node_dsp, "/dev/dsp2");
	}

	DevPrope->dsp_record_r_fd = open(DevPrope->device_node_dsp, O_RDONLY);
	if (DevPrope->dsp_record_r_fd < 0) {
		printf("ropen record dsp(%s) error: %s.\n", buf, strerror(errno));
		DevPrope->dsp_record_r_fd = -1;
		return -1;
	}

        // Parse playback dsp device.
	if (mozart_ini_getkey("/usr/data/system.ini", "audio", "dev_playback", buf)) {
		printf("[app_hs - OSS] Can't get playback dsp device, force to /dev/dsp.\n");
		strcpy(buf, "/dev/dsp");
	}

	DevPrope->dsp_play_w_fd = open(buf, O_WRONLY);
	if (DevPrope->dsp_play_w_fd < 0) {
		printf("wopen play dsp(%s) error: %s.\n", buf, strerror(errno));
		DevPrope->dsp_play_w_fd = -1;
		return -1;
	}

	DevPrope->dsp_play_r_fd = open(buf, O_RDONLY);
	if (DevPrope->dsp_play_r_fd < 0) {
		printf("ropen play dsp(%s) error: %s.\n", buf, strerror(errno));
		DevPrope->dsp_play_r_fd = -1;
		return -1;
	}

	set_record_route(DevPrope);

	printf("hs set codec: sample_rate = %d, sample_bits = %d, sample_channel = %d\n",
			hs_resample_data.resample_rate, hs_resample_data.resample_bits, hs_resample_data.resample_channel);

	if (config_device(DevPrope->dsp1_r_fd, HS_SAMPLE_BITS, HS_SAMPLE_CHANNEL, HS_SAMPLE_RATE) < 0 )
		return -1;
	if (config_device(DevPrope->dsp1_w_fd, HS_SAMPLE_BITS, HS_SAMPLE_CHANNEL, HS_SAMPLE_RATE) < 0 )
		return -1;
	if (config_device(DevPrope->dsp_play_r_fd, hs_resample_data.resample_bits, hs_resample_data.resample_channel, hs_resample_data.resample_rate) < 0 )
		return -1;
	if (config_device(DevPrope->dsp_play_w_fd, hs_resample_data.resample_bits, hs_resample_data.resample_channel, hs_resample_data.resample_rate) < 0 )
		return -1;
	if (config_device(DevPrope->dsp_record_r_fd, HS_SAMPLE_BITS, HS_SAMPLE_CHANNEL, HS_SAMPLE_RATE) < 0 )
		return -1;

	pthread_create(&bt_dev.uw_thread, NULL, write_codec_thread_func, DevPrope);
	pthread_create(&bt_dev.dw_thread, NULL, write_bt_thread_func, DevPrope);
	pthread_create(&bt_dev.dr_thread, NULL, read_dmic_thread_func, DevPrope);
	pthread_create(&bt_dev.ur_thread, NULL, read_bt_thread_func, DevPrope);

	return 0;
}
