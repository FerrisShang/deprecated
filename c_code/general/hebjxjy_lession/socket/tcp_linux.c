#ifdef __LINUX__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "tcp_linux.h"

struct tcplink* InitLink(char *Ip, int port)
{
	struct tcplink* tcplink;
	tcplink = malloc(sizeof(struct tcplink));
	if(tcplink == NULL)return NULL;
	tcplink->r_addr.sin_family = AF_INET;  
	tcplink->r_addr.sin_port = htons(port);  
	tcplink->r_addr.sin_addr.s_addr = inet_addr(Ip);
	//bzero(&(tcplink->r_addr.sin_zero),8);  
	tcplink->recv_max = TCP_RECV_LEN;
	tcplink->recv_buf = malloc(tcplink->recv_max);
	if(tcplink->recv_buf == NULL){
		close(tcplink->sockfd);
		free(tcplink);
		return NULL;
	}
	return tcplink;
}

int tcp_comm(struct tcplink* tcplink, char *send_buf, int send_len, 
		int(*func)(char *buf, int len))
{
	char *p;
	int ret,time_out = 0;
	tcplink->recv_len = 0;
	tcplink->recv_buf[0] = 0;
	p = tcplink->recv_buf;

	if((tcplink->sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {  
		return -1;
	}
	if(connect(tcplink->sockfd,(struct sockaddr *)&tcplink->r_addr,  sizeof(struct sockaddr))==-1){
		close(tcplink->sockfd);
		return -1;
	}
	int sock_state;
	sock_state = fcntl(tcplink->sockfd, F_GETFL, 0);
	fcntl(tcplink->sockfd, F_SETFL, sock_state | O_NONBLOCK);
	write(tcplink->sockfd, send_buf, send_len); 
	while(1){
		ret = read(tcplink->sockfd, p, tcplink->recv_max);
		if(ret == 0){
			close(tcplink->sockfd);
			tcplink->recv_len = p - tcplink->recv_buf;
			*p = 0;
			return tcplink->recv_len;
		}else if(ret > 0){
			p += ret;
			tcplink->recv_len = p - tcplink->recv_buf;
			time_out = 0;
		}else{
			usleep(TCP_RECV_TIME_INTV_MS*1000);
			time_out++;
			if(time_out > TCP_RECV_TIME_OUT_S*500/TCP_RECV_TIME_INTV_MS ||
				(func != NULL && func(tcplink->recv_buf, tcplink->recv_len)==1)){
				close(tcplink->sockfd);
				tcplink->recv_len = p - tcplink->recv_buf;
				*p = 0;
				return tcplink->recv_len;
			}
			puts("waiting for timeout");///////////////////////////////////////
		}
		usleep(TCP_RECV_TIME_INTV_MS*1000);
	}
}
void CloseLink(struct tcplink* tcplink)
{
	if(tcplink){
		close(tcplink->sockfd);
		free(tcplink->recv_buf);
		free(tcplink);
	}
	return;
}
#endif /* __LINUX__ */
