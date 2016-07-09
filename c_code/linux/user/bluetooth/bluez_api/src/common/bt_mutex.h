#ifndef __BT_NUTEX_H__
#define __BT_NUTEX_H__

#include <pthread.h>
typedef pthread_mutex_t bt_mutex_t;

int bt_mutex_init(bt_mutex_t *mutex);
int bt_mutex_destroy(bt_mutex_t *mutex);
int bt_mutex_trylock(bt_mutex_t *mutex);
int bt_mutex_lock(bt_mutex_t *mutex);
int bt_mutex_unlock(bt_mutex_t *mutex);

#endif /* __BT_NUTEX_H__ */
