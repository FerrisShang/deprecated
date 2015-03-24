#include <stdio.h>  
#include <stdlib.h>  
#include <errno.h>  
#include <string.h>  
#include <fcntl.h>
#include <netdb.h>  
#include <sys/types.h>  
#include <netinet/in.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  

//#define HOST_IP "119.75.218.70"
//#define HOST_PORT 80
#define HOST_IP "192.168.43.1"
#define HOST_PORT 5000
void light_ctrl(char light, char state)
{
//	return; 
#define LIGHT_POS 32
#define STATE_POS 5
	char command[] = "echo 1 > /sys/class/leds/wl_led_b/brightness";
	command[LIGHT_POS] = light;
	command[STATE_POS] = state;
	system(command);
}
int main(int argc,char *argv[]) {  
	int sockfd,numbytes;  
	char read_buf[4096];
	char write_buf[4096];
	char host_ip[16];
	int host_port;
	char *msg = "GET /index.html HTTP/1.1\nHost: www.baidu.com\n\n";  
	struct hostent *he;  
	struct sockaddr_in their_addr;  
	int i = 0;  
	if(argc > 1){
		strcpy(host_ip, argv[1]);
		if(argc >= 2)
			host_port = atoi(argv[2]);
		else
			host_port = HOST_PORT;
	}else{
		strcpy(host_ip, HOST_IP);
	}
	while(1){
		light_ctrl('r','0');
		light_ctrl('g','0');
		light_ctrl('b','0');
		he = gethostbyname(host_ip);
		if((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) {  
			perror("socket");  
			exit(1);  
		}  
		their_addr.sin_family = AF_INET;  
		their_addr.sin_port = htons(host_port);  
		their_addr.sin_addr = *((struct in_addr *)he->h_addr);  
		bzero(&(their_addr.sin_zero),8);  
		while(1){
			if(connect(sockfd,(struct sockaddr *)&their_addr,  sizeof(struct sockaddr))==-1){  
				light_ctrl('r','1');
				perror("connect");  
				sleep(2);  
			}else{
				printf("connect ok!\n");  
				break;
			}
		}
		int sock_state = fcntl(sockfd, F_GETFL, 0);
		fcntl(sockfd, F_SETFL, sock_state | O_NONBLOCK);
		struct timeval waitd;            
		int err;
		fd_set read_flags,write_flags; // you know what these are  
		write(sockfd,"hello world\n",strlen("hello world\n")-1);  
		while(1) {  
			waitd.tv_sec = 1;     // Make select wait up to 1 second for data  
			waitd.tv_usec = 0;    // and 0 milliseconds.  
			FD_ZERO(&read_flags); // Zero the flags ready for using  
			FD_ZERO(&write_flags);  
			FD_SET(sockfd, &read_flags);  
			if(strlen(write_buf)!=0) FD_SET(sockfd, &write_flags);  
			err=select(sockfd+1, &read_flags,&write_flags,  
					(fd_set*)0,&waitd);  
			if(err < 0) continue;  
			if(FD_ISSET(sockfd, &read_flags)) { //Socket ready for reading  
				FD_CLR(sockfd, &read_flags);  
				memset(&read_buf,0,sizeof(read_buf));  
				if (read(sockfd, read_buf, sizeof(read_buf)-1) <= 0) {  
					close(sockfd);  
					break;  
				}else{
					printf("%s\n",read_buf);  
					light_ctrl(read_buf[0], read_buf[1]);
				}
			}  
			if(FD_ISSET(sockfd, &write_flags)) { //Socket ready for writing  
				FD_CLR(sockfd, &write_flags);  
				write(sockfd,read_buf,strlen(read_buf));  
				memset(&read_buf,0,sizeof(read_buf));  
			}  
		} 
		printf("socket closed..\n");
	}
}  
