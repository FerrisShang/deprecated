#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <asoundlib.h>
#include "queue.h"

struct record_frame{
	int size;
	char *buffer;
	void (*destroy_cb)(struct record_frame *frame);
};
struct config{
	struct queue *q_frame;
	sem_t sem_in;
	pthread_mutex_t mutex;
    unsigned int device;
    unsigned int card;
    unsigned int period_size;
    unsigned int period_count;
    unsigned int channels;
    unsigned int rate;
    unsigned int bits;
};
static struct config* create_config(void);
static struct record_frame* new_frame(int size,
		void (*destroy_cb)(struct record_frame *frame));
static void destroy_frame(struct record_frame *frame);
static void *alsa_play(void* p);
static void *alsa_record(void* p);
static int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count);
static int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit);

int main(int argc, char*argv[])
{
	struct config *config;

	config = create_config();
	if(config == NULL){
		printf("Init failed\n");
		return -1;
	}

	pthread_t th_record, th_play;
	pthread_create(&th_record,    NULL, alsa_record,    (void*)config);
	pthread_create(&th_play,      NULL, alsa_play,      (void*)config);
	pthread_join(th_record, NULL);
}

static struct config* create_config(void)
{
	struct config *config;
	config = calloc(1, sizeof(struct config));

	config->q_frame = queue_new();
	sem_init(&config->sem_in, 0, 0);
	pthread_mutex_init(&config->mutex, NULL);
	config->device = 0;
    config->card = 1;
    config->period_size = 1024;
    config->period_count = 4;
    config->channels = 2;
    config->rate = 48000;
    config->bits = 16;

	return config;
}

static struct record_frame* new_frame(int size,
		void (*destroy_cb)(struct record_frame *frame))
{
	struct record_frame *frame;
	frame = calloc(1, sizeof(struct record_frame));
	if(frame == NULL){
		return NULL;
	}
	frame->buffer = malloc(size);
	if(frame->buffer == NULL){
		free(frame);
		return NULL;
	}
	frame->size = size;
	frame->destroy_cb = destroy_cb;
	return frame;
}
static void destroy_frame(struct record_frame *frame)
{
	free(frame->buffer);
	free(frame);
}

static void *alsa_play(void* p)
{
	struct config *conf = (struct config*)p;
	struct pcm_config config;
    struct pcm *pcm;
    memset(&config, 0, sizeof(config));
    config.channels = conf->channels;
    config.rate = conf->rate;
    config.period_size = conf->period_size;
    config.period_count = conf->period_count;
    if (conf->bits == 32)
        config.format = PCM_FORMAT_S32_LE;
    else if (conf->bits == 16)
        config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;
    pcm = pcm_open(conf->card, conf->device, PCM_OUT, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device %u (%s)\n",
                conf->device, pcm_get_error(pcm));
        return NULL;
    }

	while(1){
		struct record_frame *frame = NULL;
		int res;
		sem_wait(&conf->sem_in);
		pthread_mutex_lock(&conf->mutex);
		frame = queue_pop_head(conf->q_frame);
		pthread_mutex_unlock(&conf->mutex);
		if(frame != NULL){
			if (pcm_write(pcm, frame->buffer, frame->size/2)) {
                fprintf(stderr, "Error playing sample\n");
                break;
            }
			frame->destroy_cb(frame);
		}
	}
	return NULL;
}
static void *alsa_record(void* p)
{
	struct config *conf = (struct config*)p;
	struct pcm_config config;
    struct pcm *pcm;

    memset(&config, 0, sizeof(config));
    config.channels = conf->channels;
    config.rate = conf->rate;
    config.period_size = conf->period_size;
    config.period_count = conf->period_count;
    if (conf->bits == 32)
        config.format = PCM_FORMAT_S32_LE;
    else if (conf->bits == 16)
        config.format = PCM_FORMAT_S16_LE;
    config.start_threshold = 0;
    config.stop_threshold = 0;
    config.silence_threshold = 0;

    pcm = pcm_open(conf->card, conf->device, PCM_IN, &config);
    if (!pcm || !pcm_is_ready(pcm)) {
        fprintf(stderr, "Unable to open PCM device (%s)\n",
                pcm_get_error(pcm));
        return 0;
    }

	while(1){
		int res;
		struct record_frame *frame;
		frame = new_frame(conf->period_size, destroy_frame);
		if(frame == NULL){
			printf("Get new frame error.\n");
			break;
		}
		pcm_read(pcm, frame->buffer, frame->size/2);
		pthread_mutex_lock(&conf->mutex);
		queue_push_tail(conf->q_frame, frame);
		pthread_mutex_unlock(&conf->mutex);
		sem_post(&conf->sem_in);
	}
	return NULL;
}

static int sample_is_playable(unsigned int card, unsigned int device, unsigned int channels,
                        unsigned int rate, unsigned int bits, unsigned int period_size,
                        unsigned int period_count)
{
    struct pcm_params *params;
    int can_play;

    params = pcm_params_get(card, device, PCM_OUT);
    if (params == NULL) {
        fprintf(stderr, "Unable to open PCM device %u.\n", device);
        return 0;
    }

    can_play = check_param(params, PCM_PARAM_RATE, rate, "Sample rate", "Hz");
    can_play &= check_param(params, PCM_PARAM_CHANNELS, channels, "Sample", " channels");
    can_play &= check_param(params, PCM_PARAM_SAMPLE_BITS, bits, "Bitrate", " bits");
    can_play &= check_param(params, PCM_PARAM_PERIOD_SIZE, period_size, "Period size", "Hz");
    can_play &= check_param(params, PCM_PARAM_PERIODS, period_count, "Period count", "Hz");

    pcm_params_free(params);

    return can_play;
}

static int check_param(struct pcm_params *params, unsigned int param, unsigned int value,
                 char *param_name, char *param_unit)
{
    unsigned int min;
    unsigned int max;
    int is_within_bounds = 1;

    min = pcm_params_get_min(params, param);
    if (value < min) {
        fprintf(stderr, "%s is %u%s, device only supports >= %u%s\n", param_name, value,
                param_unit, min, param_unit);
        is_within_bounds = 0;
    }

    max = pcm_params_get_max(params, param);
    if (value > max) {
        fprintf(stderr, "%s is %u%s, device only supports <= %u%s\n", param_name, value,
                param_unit, max, param_unit);
        is_within_bounds = 0;
    }

    return is_within_bounds;
}

