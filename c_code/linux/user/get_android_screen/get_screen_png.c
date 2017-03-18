#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "get_screen_png.h"

#define MAX_BUF_SIZE (16<<20)
static struct screen_png screen;
static char *file_org_buf;
static char *file_new_buf;

static int get_file_size(const char *path);

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
	if(q[0] != (char)0x89 || q[1] != (char)0x50 || q[2] != (char)0x4e ||
			q[3] != (char)0x47 || q[4] != (char)0x0d || q[5] != (char)0x0a ||
			q[6] != (char)0x1a || q[7] != (char)0x0a){
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
		p++;
		screen.size = p - file_new_buf;
		screen.data = file_new_buf;
	}else{
		screen.size = size;
		screen.data = file_org_buf;
	}
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
