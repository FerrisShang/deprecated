#include "bt_mutex.h"
int bt_mutex_init(bt_mutex_t *mutex)
{
	return pthread_mutex_init(mutex, NULL);
}
int bt_mutex_destroy(bt_mutex_t *mutex)
{
	return pthread_mutex_destroy(mutex);
}
int bt_mutex_trylock(bt_mutex_t *mutex)
{
	return pthread_mutex_trylock(mutex);
}
int bt_mutex_lock(bt_mutex_t *mutex)
{
	return pthread_mutex_lock(mutex);
}
int bt_mutex_unlock(bt_mutex_t *mutex)
{
	return pthread_mutex_unlock(mutex);
}
