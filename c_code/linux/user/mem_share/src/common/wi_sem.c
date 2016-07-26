#include "wi_sem.h"
#include <time.h>

int wi_sem_init(wi_sem_t *sem, unsigned int value)
{
	return sem_init(sem, 0, value);
}

int wi_sem_wait(wi_sem_t *sem)
{
	return sem_wait(sem);
}

int wi_sem_try(wi_sem_t *sem)
{
	return sem_trywait(sem);
}

int wi_sem_timedwait(wi_sem_t *sem,int time_s)
{
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += time_s;
	return sem_timedwait(sem, &ts);
}

int wi_sem_post(wi_sem_t *sem)
{
	return sem_post(sem);
}

int wi_sem_destroy(wi_sem_t *sem)
{
	return sem_destroy(sem);
}
