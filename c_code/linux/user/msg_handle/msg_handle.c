#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include "msg_handle.h"
#include "mem_manage.h"

#define QUEUE_CMD_ERROR -255 //queue freed or queue empty
#define MSG_HANDLE_DESTORY -1 

struct msg_queue_entry {
	int cmd;
	void *data;
	struct msg_queue_entry *next;
};

struct msg_queue {
	struct msg_queue_entry *head;
	struct msg_queue_entry *tail;
	unsigned int entries;
};

struct msg_handle{
	sem_t sem;
	pthread_mutex_t mutex;
	struct msg_queue *queue;
	pthread_t thread;

	msg_handle_cb_t callback;
	void *user_data;
};

static struct msg_queue *msg_queue_new(void)
{
	struct msg_queue *queue;
	queue = mem_calloc(sizeof(struct msg_queue),1);
	if (!queue){
		return NULL;
	}
	queue->head = NULL;
	queue->tail = NULL;
	queue->entries = 0;
	return queue;
}

static struct msg_queue_entry *msg_queue_entry_new(int cmd, void *data)
{
	struct msg_queue_entry *entry;
	entry = mem_calloc(sizeof(struct msg_queue_entry), 1);
	if (!entry)
		return NULL;
	entry->cmd = cmd;
	entry->data = data;
	return entry;
}

static unsigned int msg_queue_remove_all(struct msg_queue *queue)
{
	struct msg_queue_entry *entry;
	unsigned int count = 0;
	if (!queue){
		return 0;
	}
	entry = queue->head;
	queue->head = NULL;
	queue->tail = NULL;
	queue->entries = 0;
	while (entry) {
		struct msg_queue_entry *tmp = entry;
		entry = entry->next;
		count++;
		mem_free(tmp);
	}
	return count;
}
static void queue_destroy(struct msg_queue *queue)
{
	if (!queue){
		return;
	}
	msg_queue_remove_all(queue);
	mem_free(queue);
}

static void *queue_pop_head(struct msg_queue *queue, int *cmd)
{
	struct msg_queue_entry *entry;
	void *data;
	if (!queue || !queue->head){
		*cmd = QUEUE_CMD_ERROR;
		return NULL;
	}
	entry = queue->head;
	if(!queue->head->next){
		queue->head = NULL;
		queue->tail = NULL;
	}else{
		queue->head = queue->head->next;
	}
	data = entry->data;
	*cmd = entry->cmd;
	mem_free(entry);
	queue->entries--;
	return data;
}
static int queue_push_tail(struct msg_queue *queue, int cmd, void *data)
{
	struct msg_queue_entry *entry;
	if (!queue){
		return 0;
	}
	entry = msg_queue_entry_new(cmd, data);
	if (!entry){
		return 0;
	}
	if (queue->tail){
		queue->tail->next = entry;
	}
	queue->tail = entry;
	if (!queue->head){
		queue->head = entry;
	}
	queue->entries++;
	return 1;
}


static void msg_handle_unlock_free(struct msg_handle *msg_handle)
{
	queue_destroy(msg_handle->queue);
	sem_destroy(&msg_handle->sem);
	pthread_mutex_unlock(&msg_handle->mutex);
	pthread_mutex_destroy(&msg_handle->mutex);  
	mem_free(msg_handle);
}

static void msg_pthread_func(void* data)  
{
	struct msg_handle *msg_handle = data;
	struct msg_queue_entry *queue_data;
	int cmd;
	int res;
	while(1){
		res = sem_wait(&msg_handle->sem);
		if(res != 0){
			//return error
			printf("sem error !!!!\n");
			while(1);
			break;
		}
		pthread_mutex_lock(&msg_handle->mutex);
		queue_data = queue_pop_head(msg_handle->queue, &cmd);
		if(cmd == MSG_HANDLE_DESTORY){
			msg_handle_unlock_free(msg_handle);
			return;
		}
		pthread_mutex_unlock(&msg_handle->mutex);
		if(cmd != QUEUE_CMD_ERROR){
			msg_handle->callback(cmd, queue_data, msg_handle->user_data);
		}
	}
}

struct msg_handle *msg_handle_new(msg_handle_cb_t msg_handle_cb, void *user_data)
{
	struct msg_handle *msg_handle;
	if(!msg_handle_cb){
		return NULL;
	}
	msg_handle = mem_malloc(sizeof(struct msg_handle));
	if (!msg_handle){
		return NULL;
	}
	if(!msg_handle_cb){
		return NULL;
	}
	msg_handle->callback = msg_handle_cb;
	msg_handle->user_data = user_data;

	msg_handle->queue = msg_queue_new();
	if(!msg_handle->queue){
		mem_free(msg_handle);
		return NULL;
	}
	sem_init(&msg_handle->sem, 0, 0);
	pthread_mutex_init(&msg_handle->mutex, NULL);

	pthread_create(&msg_handle->thread, NULL, (void*)msg_pthread_func, msg_handle);
	return msg_handle;
}

void msg_send(struct msg_handle *msg_handle, int cmd, void *msg_data)
{
	pthread_mutex_lock(&msg_handle->mutex);
	queue_push_tail(msg_handle->queue, cmd, msg_data);
	pthread_mutex_unlock(&msg_handle->mutex);
	sem_post(&msg_handle->sem);
}

void msg_handle_destory(struct msg_handle *msg_handle)
{
	pthread_t thread;
	thread = msg_handle->thread;
	msg_send(msg_handle, MSG_HANDLE_DESTORY, NULL);
	pthread_join(thread, NULL);
}
