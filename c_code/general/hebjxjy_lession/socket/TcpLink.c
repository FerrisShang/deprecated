#ifndef __LINUX__
#include "TcpLink.h"

struct tcplink* InitLink(char *Ip, int port, int recvTimeout)
{
	struct tcplink* tcplink;
	tcplink = malloc(sizeof(struct tcplink));
	if(tcplink == NULL)return NULL;
    tcplink->sockVersion = MAKEWORD(2,2);  
    if(WSAStartup(tcplink->sockVersion, &tcplink->data) != 0) return NULL;
    tcplink->serAddr.sin_family = AF_INET;  
    tcplink->serAddr.sin_port = htons(port);  
    tcplink->serAddr.sin_addr.S_un.S_addr = inet_addr(Ip);   
    tcplink->sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if(tcplink->sclient == INVALID_SOCKET)return NULL;  
    tcplink->tv_out.tv_sec = recvTimeout/1000;
    tcplink->tv_out.tv_usec = recvTimeout%1000;
    setsockopt(tcplink->sclient, SOL_SOCKET, SO_RCVTIMEO,(const char*)&tcplink->tv_out, sizeof(struct timeval));
    if (connect(tcplink->sclient, (struct sockaddr *)&tcplink->serAddr, sizeof(struct sockaddr_in)) == SOCKET_ERROR){  
        closesocket(tcplink->sclient);  
        return NULL;
    }
    return tcplink;
}
void CloseLink(struct tcplink* tcplink)
{
   closesocket(tcplink->sclient); 
   free(tcplink);
}
int SendData(struct tcplink* tcplink, char *data, int len)
{
    return send(tcplink->sclient, data, len, 0);     
}
int recvData(struct tcplink* tcplink, char *data, int len)
{
    return recv(tcplink->sclient, data, len, 0);      
}
#endif /* __LINUX__ */
