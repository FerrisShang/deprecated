#include "stdio.h"
#define HEADER_LEN (8+4+4)
int main(int argc, char *argv[])
{
	FILE *fp;
	char buf[1024];
	int len,len_flag, i, count=1;
	fp = fopen(argv[1],"r");
	fread(buf,HEADER_LEN,1,fp);
	while(1){
		if(fread(buf,4,1,fp) == 0){//len
			break;
		}
		printf("%4d\t", count++);
		len = (((unsigned char)buf[2])<<8)+(unsigned char)buf[3];
		if(fread(buf,4,1,fp) == 0);//len
		if(fread(buf,4,1,fp) == 0);//flag
		printf("%c\t", '0' + buf[3]);
		if(fread(buf,4,1,fp) == 0);//all zeros
		if(fread(buf,8,1,fp) == 0);//time stemp
		if(fread(buf,len,1,fp) == 0);//package

		if(len > 32){
			len = 32;
			len_flag = 1;
		}else
			len_flag = 0;
		for(i=0;i<len;i++){
			printf("%02x ", (unsigned char)buf[i]);
		}
		if(len_flag != 0){
			printf("...");
		}
		printf("\n");
	}
}
