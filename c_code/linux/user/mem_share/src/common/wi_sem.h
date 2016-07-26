#ifndef __WI_SEMAPHORE_H__
#define __WI_SEMAPHORE_H__

#include <semaphore.h>
typedef sem_t wi_sem_t;

int wi_sem_init(wi_sem_t *sem, unsigned int value);
int wi_sem_wait(wi_sem_t *sem);
int wi_sem_try(wi_sem_t *sem);
int wi_sem_timedwait(wi_sem_t *sem,int time_s);
int wi_sem_post(wi_sem_t *sem);
int wi_sem_destroy(wi_sem_t *sem);

#endif /* __WI_SEMAPHORE_H__ */
