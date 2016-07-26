#ifndef __WI_NUTEX_H__
#define __WI_NUTEX_H__

#include <pthread.h>
typedef pthread_mutex_t wi_mutex_t;

int wi_mutex_init(wi_mutex_t *mutex);
int wi_mutex_destroy(wi_mutex_t *mutex);
int wi_mutex_trylock(wi_mutex_t *mutex);
int wi_mutex_lock(wi_mutex_t *mutex);
int wi_mutex_unlock(wi_mutex_t *mutex);

#endif /* __WI_NUTEX_H__ */
