#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "md5.h"

unsigned int get_file_size(const char *path)
{  
	unsigned long filesize = -1;      
	struct stat statbuff;  
	if(stat(path, &statbuff) < 0){  
		return filesize;  
	}else{  
		filesize = statbuff.st_size;  
	}  
	return filesize;  
} 

int main(int argc, char *argv[])
{
	FILE *fp;
	unsigned char *fileBuf, *md5Buf;
	int fileSize = get_file_size(argv[1]);
	int i;
	if(argc != 2){
		printf("Invalid parameter.\n");
		return;
	}
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fp = fopen(argv[1],"rb");
		if(fp != NULL){
			fread(fileBuf,fileSize,1,fp);
			md5Buf = md5(fileBuf,fileSize);
			fclose(fp);
		}
		free(fileBuf);
	}
	for(i=0;i<16;i++){
		printf("%02x",md5Buf[i]);
	}
	printf("\n");
}
