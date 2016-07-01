#include "bt_semaphore.h"
#include <time.h>

int bt_sem_init(bt_sem_t *sem, unsigned int value)
{
	return sem_init(sem, 0, value);
}

int bt_sem_wait(bt_sem_t *sem)
{
	return sem_wait(sem);
}

int bt_sem_try(bt_sem_t *sem)
{
	return sem_trywait(sem);
}

int bt_sem_timedwait(bt_sem_t *sem,int time_s)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += time_s;
	return sem_timedwait(sem, &ts);
}

int bt_sem_post(bt_sem_t *sem)
{
	return sem_post(sem);
}

int bt_sem_destroy(bt_sem_t *sem)
{
	return sem_destroy(sem);
}
