#include "wi_mutex.h"
int wi_mutex_init(wi_mutex_t *mutex)
{
	return pthread_mutex_init(mutex, NULL);
}
int wi_mutex_destroy(wi_mutex_t *mutex)
{
	return pthread_mutex_destroy(mutex);
}
int wi_mutex_trylock(wi_mutex_t *mutex)
{
	return pthread_mutex_trylock(mutex);
}
int wi_mutex_lock(wi_mutex_t *mutex)
{
	return pthread_mutex_lock(mutex);
}
int wi_mutex_unlock(wi_mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}
