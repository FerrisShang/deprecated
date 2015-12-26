#include <stdio.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>


#include "notif_value.h"
#include "attribute_data.h"


struct assembler_data{
	Orig_data orig_data[ORI_BUF_SIZE]; 
	FUNC_CALLBK mcallback;
	int cur_memb;
	pthread_mutex_t data_lock; 
};

static struct assembler_data *ass_data = NULL;
static pthread_t ass_tid;
static pthread_cond_t ass_ready = PTHREAD_COND_INITIALIZER;

static void sig_process(int);
static void* assembling_pthread(void* arg);
static void assemble_alarm(long sec, long usec);


int start_resp_assembler_session(FUNC_CALLBK callback)
{
	int ret = 0;
	ass_data = malloc(sizeof(struct assembler_data));
	if(ass_data == NULL){
		DEBUG("alloc memory failed\n");
		return -1;
	}
	memset(ass_data, 0, sizeof(struct assembler_data));
	pthread_mutex_init(&ass_data->data_lock, NULL);
	ass_data->mcallback = callback;



	if(signal(SIGALRM,sig_process) == SIG_ERR){
		DEBUG("can't catch this signal");
		pthread_mutex_destroy(&ass_data->data_lock);
		free(ass_data);
		return -1;
	}


	ret = pthread_create(&ass_tid,NULL,assembling_pthread,NULL);
	if(ret != 0){
		DEBUG("pthread_create error\n");
		pthread_mutex_destroy(&ass_data->data_lock);
		free(ass_data);
		return -1;
	}

	return 0;	
}


int resp_data_assembling(void* src_data, int len)
{
	int cur_len = 0;
	unsigned char* pbuf = NULL;
	if(src_data == NULL){
		DEBUG("%s :src_data is NULL\n",__func__);
		return -1;
	}

	pthread_mutex_lock(&ass_data->data_lock);


	cur_len = ass_data->orig_data[ass_data->cur_memb].data_len;
	pbuf = ass_data->orig_data[ass_data->cur_memb].src_data;


	if(ass_data->orig_data[ass_data->cur_memb].in_use || cur_len >= DATA_LENGTH - 1){
		DEBUG("%s :orig_data is overrun in_use = %d, cur_len = %d\n",
				__func__, ass_data->orig_data[ass_data->cur_memb].in_use, cur_len);


		pthread_mutex_unlock(&ass_data->data_lock);
		return -1;
	}
	
	DEBUG("%s: cur_memb = %d\n",__func__,
		ass_data->cur_memb);

	if(len < DATA_LENGTH - cur_len){
		memcpy(pbuf + cur_len, src_data, len);	
		cur_len += len;
	}else{
		memcpy(pbuf + cur_len, src_data, DATA_LENGTH - cur_len);
		cur_len = DATA_LENGTH;
	}
	

	ass_data->orig_data[ass_data->cur_memb].data_len = cur_len;

	pthread_mutex_unlock(&ass_data->data_lock);
	assemble_alarm(0,MAX_PARTS_TIMEOUT);
}


static void* assembling_pthread(void* arg)
{
	int i = 0;
	void* dest_buf = NULL;
	int ret = 0;

	for(;;){
		pthread_cond_wait(&ass_ready,&ass_data->data_lock);		
		DEBUG("assembling_pthread tid = %lu\n",(unsigned long) pthread_self());

		for(i = 0;i < ORI_BUF_SIZE;i++){
			if(ass_data->orig_data[i].in_use){

				dest_buf = calloc(1,sizeof(Resp_data));						
				if(dest_buf == NULL){
					DEBUG("dest_buf alloc error\n");
					pthread_mutex_unlock(&ass_data->data_lock);
					pthread_exit(NULL);
				}

				ret = parse_data(dest_buf,ass_data->orig_data[i].src_data,
							ass_data->orig_data[i].data_len);
				if(ret < 0 ){
					DEBUG("parse_notif_data error\n");
					free(dest_buf);
					pthread_mutex_unlock(&ass_data->data_lock);
					pthread_exit(NULL);
				}
				pthread_mutex_unlock(&ass_data->data_lock);
				ass_data->mcallback(dest_buf);	
				pthread_mutex_lock(&ass_data->data_lock);
				ass_data->orig_data[i].in_use = 0;
				ass_data->orig_data[i].data_len = 0;
				resp_assembler_data_free(dest_buf);
			}
		}
	}
}
static void sig_process(int signo)
{
	if(signo == SIGALRM){
		pthread_mutex_lock(&ass_data->data_lock);
		
		ass_data->orig_data[ass_data->cur_memb].in_use = 1; 
		ass_data->cur_memb++;
		if(ass_data->cur_memb >= ORI_BUF_SIZE)
			ass_data->cur_memb = 0;

		pthread_mutex_unlock(&ass_data->data_lock);
		pthread_cond_signal(&ass_ready);
		DEBUG("%s\n",__func__);
	}
}

static void assemble_alarm(long sec, long usec)
{
	struct itimerval time_val;

	memset(&time_val,0,sizeof(time_val));
	setitimer(ITIMER_REAL,&time_val,NULL);

	time_val.it_value.tv_sec = sec;
	time_val.it_value.tv_usec = usec;

	setitimer(ITIMER_REAL, &time_val, NULL);
}

void resp_assembler_data_free(void* p)
{
	Resp_data* data = p;
	free_attribute_data(data);
	free(data);
}

void end_resp_assembler_session()
{
	int ret = pthread_mutex_destroy(&ass_data->data_lock);
	DEBUG("%s ret = %d\n",__func__,ret);
	free(ass_data);
	pthread_cancel(ass_tid);
	return ;
}
