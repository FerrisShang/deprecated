#ifndef __BT_SEMAPHORE_H__
#define __BT_SEMAPHORE_H__

#include <semaphore.h>
typedef sem_t bt_sem_t;

int bt_sem_init(bt_sem_t *sem, unsigned int value);
int bt_sem_wait(bt_sem_t *sem);
int bt_sem_try(bt_sem_t *sem);
int bt_sem_timedwait(bt_sem_t *sem,int time_s);
int bt_sem_post(bt_sem_t *sem);
int bt_sem_distory(bt_sem_t *sem);

#endif /* __BT_SEMAPHORE_H__ */
