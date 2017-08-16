#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <sys/stat.h>
#include "wav.h"

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
	char *in_buf;
	char *out_buf;
	int len;
	if(argc != 6){
		printf("Usage : %s pcm_file wav_file ch rate bit\n", argv[0]);
		return 0;
	}
	FILE *fpr = fopen(argv[1], "r");
	if(!fpr){
		printf("Read file %s failed\n", argv[1]);
		return 0;
	}
	len = get_file_size(argv[1]);
	in_buf = malloc(len);
	out_buf = malloc(len+WAV_HEAD_SIZE);
	len = fread(in_buf, 1, len, fpr);
	printf("read:%d\n", len);
	init_wav_header(out_buf, len, atoi(argv[3]), atoi(argv[4]), atoi(argv[5]));
	memcpy(&out_buf[WAV_HEAD_SIZE], in_buf, len);
	FILE *fpw = fopen(argv[2], "w");
	if(!fpw){
		printf("Open file %s failed\n", argv[2]);
		return 0;
	}
	fwrite(out_buf, WAV_HEAD_SIZE+len, 1, fpw);
	fclose(fpr);
	fclose(fpw);
	free(out_buf);
	free(in_buf);
}

