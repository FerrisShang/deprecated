#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fileUtil.h"
#include "strUtil.h"
#define COM_SYMBOL "list"
 
int findFunc(const char* file, int size)
{
	char *pos;
	static char byte[256]={0};
	while(1){
		pos = strstr(file, "return");
		if(pos == 0)
			return 1;
		char *tPos;
		tPos = strbVis(pos, 16);
		if(tPos != 0){
			if(byte[(unsigned char)*tPos]==0){
				byte[(unsigned char)*tPos] = 1;
				tPos[9] = 0;
				puts(tPos-10);
			}
		}
		file = pos+1;
	}
}
int fileFunc(const char* path,void *para)
{
	FILE *fp;
	char *fileBuf;
	int fileSize;
	int nameLen;
	nameLen = strlen(path);
	if(path[nameLen-2] != '.')return 0;
	if(path[nameLen-1] != 'c' && path[nameLen-1] != 'h')return 0;
	//printf("%s\n", path);
	fileSize = get_file_size(path);
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fp = fopen(path,"rb");
		if(fp != NULL){
			if(fread(fileBuf,fileSize,1,fp)==0){
				return 0;
			}
			findFunc(fileBuf,fileSize);
			fclose(fp);
		}
	}
	free(fileBuf);
	return 0;
}
int main(int argc, char *argv[])
{
	if(argc < 2){
		List_Files("./",(fileHandle)fileFunc,(void*)0);
	}else{
		List_Files(argv[1],(fileHandle)fileFunc,(void*)0);
	}
	return 0;
}
