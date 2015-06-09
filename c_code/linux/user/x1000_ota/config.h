#ifndef __CONFIG_H__
#define __CONFIG_H__


/* specify debug level
 * 0 : DBG_CLOSE
 * 1 : DBG_NULL
 * 2 : DBG_ERROR
 * 3 : DBG_WARNING
 * 4 : DBG_INFO
 */
#define DEBUG_LEVEL 2

/* specify socket type
 * 0 : UDP
 * 1 : TCP
 */
#define SOCKET_TYPE 0


/* protocol module debug force on */
#define PRO_DBG           0
#define SOCK_IF_DBG       0
#define SOCK_UDP_DBG      0


#endif /* __CONFIG_H__ */
