/* audio_hw.h */
#include "app_utils.h"
#include "llist.h"
#include <pthread.h>

#define HS_SAMPLE_RATE		8000
#define HS_SAMPLE_BITS		AFMT_S16_LE
#define HS_SAMPLE_TIME		10        /*ms*/
#define HS_SAMPLE_CHANNEL	1

#define FALSE			0
#define AD_NODE_SUM		8
#define RECORD_BUFSZ		(1024)
#define AD_NODE_BUFSIZ		(1024)
#define AD_STREAM_MEMSIZ	(AD_NODE_SUM * AD_NODE_BUFSIZ)
#define SNDCTL_DSP_AEC_START    _SIOW ('J', 16, int)
#define SNDCTL_DSP_AEC_ENABLE   _SIOW ('J', 17, int)
#define SNDCTL_EXT_STOP_DMA	_SIOW ('P', 91, int)
#define SNDCTL_DSP_SYNC		_SIO  ('P', 1)

typedef unsigned int (*aec_func_t)(void);
typedef void *(*aec_calculate_t)(void *buf_mic, void *buf_ref, void *buf_result, unsigned int size);

typedef struct {
	char *in_buffer;
	char *out_buffer;
	unsigned int in_len;
	unsigned int out_len;
} bt_aec_resample_msg;

typedef void *(*bt_aec_resample_t)(bt_aec_resample_msg *bt_aec_rmsg);

typedef struct {
	char *in_buffer;
	char *out_buffer;
	unsigned int in_len;
	unsigned int out_len;
} hs_resample_msg;

typedef void *(*hs_resample_t)(hs_resample_msg *hs_msg);

typedef struct bt_aec_callback_interface {
	aec_func_t aec_enable;
	aec_func_t aec_get_buffer_length;
	aec_calculate_t aec_calculate;
	aec_func_t aec_init;
	aec_func_t aec_destroy;
} bt_aec_callback;

bt_aec_callback bt_ac;

typedef struct {
	struct list_head list;
	unsigned int size;
	char *addr;
} ADnode;

typedef struct {
	struct list_head    free_node_list;
	struct list_head    use_node_list;
	ADnode node[AD_NODE_SUM];
	int free_cnt;
	int use_cnt;
	char *mem;
	pthread_mutex_t slock;
} ADstream;
ADstream bt_dstream, bt_ustream;

typedef struct {
	unsigned int frame_len;
	unsigned int aec_enable;
	int time;
	int has_init;
	char *buf_play, *buf_record;
	aec_func_t aec_init;
	aec_func_t aec_get_enable;
	aec_func_t aec_destroy;
	aec_func_t aec_get_buffer_length;
	aec_calculate_t aec_calculate;
} AEC_DEV;

typedef struct {
	int resample_rate;
	int resample_channel;
	int resample_bits;
	int resample_enable;
	bt_aec_resample_t aec_resample_data;
} bt_aec_resample_init_data;

typedef struct {
	int sample_rate;
	int sample_channel;
	int sample_bits;
} hs_sample_init_data;

typedef struct {
	int sample_rate;
	int sample_channel;
	int sample_bits;
} bt_aec_sample_init_data;

typedef struct {
	int resample_rate;
	int resample_channel;
	int resample_bits;
	int resample_enable;
	hs_resample_t mozart_hs_resample_data;
} hs_resample_init_data;

struct bt_context {
	pthread_t dr_thread;
	pthread_t dw_thread;
	pthread_t ur_thread;
	pthread_t uw_thread;
} bt_dev;

struct device_prope {
	int dsp_play_r_fd;
	int dsp_play_w_fd;
	int dsp1_r_fd;
	int dsp1_w_fd;
	int dsp_record_r_fd;

	int write_bt_call;
	int read_bt_call;
	int write_codec_call;
	int read_dmic_call;
	char device_node_dsp[15];
	char device_node_mixer[15];
};

void mozart_aec_callback(bt_aec_callback *bt_ac);
int bt_start_call(struct device_prope *DevPrope);
int bt_end_call(struct device_prope *DevPrope);

int mozart_bt_set_aec_frame(int frame_len);
void mozart_get_hs_default_sampledata(hs_sample_init_data *sample_data);
void mozart_set_hs_resampledata_callback(hs_resample_init_data *resample_data);
