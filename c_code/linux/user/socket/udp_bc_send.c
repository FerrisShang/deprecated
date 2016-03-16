#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>

int main(int argc, char *argv[])
{
	int sock = -1;
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		return 0;
	}

	const int opt = 1;
	int nb = 0;
	nb = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt));
	if(nb == -1)
	{
		return 0;
	}

	struct sockaddr_in addrto;
	bzero(&addrto, sizeof(struct sockaddr_in));
	addrto.sin_family=AF_INET;
	addrto.sin_addr.s_addr=htonl(INADDR_BROADCAST);
	addrto.sin_port=htons(6000);
	int nlen=sizeof(addrto);

	while(1){
		static struct timeval tv;
		usleep(50000);
		gettimeofday(&tv, NULL);
		int ret=sendto(sock, &tv, sizeof(tv), 0, (struct sockaddr*)&addrto, nlen);
	}
	return 0;
}
