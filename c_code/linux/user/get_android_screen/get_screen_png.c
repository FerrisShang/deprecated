#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "get_screen_png.h"

#define MAX_BUF_SIZE (8<<20)
static struct screen_png screen;
static char *file_org_buf;
static char *file_new_buf;

static int get_file_size(const char *path);
typedef unsigned int UINT32;
typedef unsigned char UINT8;
#define STREAM_TO_UINT32(u32, p) {\
	u32 = (((UINT32)(UINT8)(*(p))) +\
			((((UINT32)(UINT8)(*((p) + 1)))) << 8) +\
			((((UINT32)(UINT8)(*((p) + 2)))) << 16) +\
			((((UINT32)(UINT8)(*((p) + 3)))) << 24));\
		(p) += 4;\
}

struct screen_png* get_screen_png(void)
{
	FILE *fp;
	char *p,*q;
	int size, i;
	system("rm -rf " SCREEN_HEX_FILE_NAME);
	system(STR_GET_SCREEN_PREFIX " "
			STR_GET_SCREEN_HEX_CMD " > "
			SCREEN_HEX_FILE_NAME);
	size = get_file_size(SCREEN_HEX_FILE_NAME);
	if(size <= 0){
		fprintf(stderr, "get screenshot failed\n");
		return NULL;
	}
	if(file_org_buf == 0){
		file_org_buf = malloc(MAX_BUF_SIZE);
		file_new_buf = malloc(MAX_BUF_SIZE);
		if(!file_org_buf || !file_new_buf){
			fprintf(stderr, "malloc error\n");
			return NULL;
		}
	}
	fp = fopen(SCREEN_HEX_FILE_NAME, "rb");
	if(fp != NULL){
		fread(file_org_buf, size, 1, fp);
		fclose(fp);
	}
	p = file_new_buf;
	q = file_org_buf;
	for(i=0;i<size-1;i++){
		if(*q == 0x0D && *(q+1) == 0x0A){
			q++;
			continue;
		}
		*p = *q;
		p++;
		q++;
	}
	*p = *q; //last byte
	screen.size = size;
	screen.data = file_new_buf;
	return &screen;
}

static int get_file_size(const char *path)
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

