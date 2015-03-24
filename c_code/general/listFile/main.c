#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "list_file.h"
#include "strHandle.h"

#define COM_SYMBOL "list"
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
int findFunc(const char* file, int size)
{
	;
}
int fileFunc(const char* path,void *para)
{
	FILE *fp;
	char *fileBuf;
	int fileSize = get_file_size(path);
	int i;
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fp = fopen(path,"rb");
		if(fp != NULL){
			fread(fileBuf,fileSize,1,fp);
			findFunc(fileBuf,fileSize);
			fclose(fp);
		}
	}
}
int main(int argc, char *argv[])
{
	char comBuf[256];
	if(argc < 2){
		List_Files("./",(fileHandle)fileFunc,(void*)0);
	}else{
		List_Files(argv[1],(fileHandle)fileFunc,(void*)0);
	}
	while(1){
		printf("%s>",COM_SYMBOL);
		scanf("%s",comBuf);
		printf("input : %s\n",comBuf);
	}
	return 0;
}
