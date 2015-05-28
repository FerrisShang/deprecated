#include<stdio.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>

void main(int argc,const char **argv)
{
	long addr;
	struct hostent *hp;
	char **p;
	if(argc != 2)
		argv[1]="www.ingenic.com";	
	hp=gethostbyname(argv[1]);
	if(hp==NULL)
	{
		printf("host information for %s not found\n",argv[1]);
		return;
	}
	for(p=hp->h_addr_list;*p!=0;p++)
	{
		struct in_addr in;
		char **q;
		memcpy(&in.s_addr, *p, sizeof(in.s_addr));
		printf("%s\t%s", inet_ntoa(in), hp->h_name);
		for(q=hp->h_aliases;*q!=0;q++)
			printf("%s", *q);
		putchar('\n');
	}
	return;
}
