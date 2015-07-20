#ifndef __LESSION_H__
#define __LESSION_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "djpeg_if.h"
#include "chk_code.h"
#include "TcpLink.h"
#include "tcp_linux.h"
#include "http_util.h"

int lession_init_cookie(struct http_handle *hhttp, struct tcplink *tcplink);
int lession_get_check_code(struct http_handle *hhttp, struct tcplink *tcplink);

#endif /* __LESSION_H__ */
