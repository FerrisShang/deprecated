#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

unsigned int get_file_size(const char *path)
{  
	unsigned long fileSize = -1;      
	struct stat statbuff;  
	if(stat(path, &statbuff) < 0){  
		return fileSize;  
	}else{  
		fileSize = statbuff.st_size;  
	}  
	return fileSize;  
} 

int main(int argc, char *argv[])
{
	int fileSize, i;
	FILE *fpFile, *fpCode;
	char *fileBuf, codeName[32];
	char buf[256];
	if(argc == 1){
		;
	}else if(argc == 2 || argc == 3){
		fileSize = get_file_size(argv[1]);
		if(fileSize < 0){
			printf("File not found.\n");
			return;
		}
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fpFile = fopen(argv[1],"rb");
		if(fpFile == NULL){
			printf("open file failed.\n");
		}
		fread(fileBuf,fileSize,1,fpFile);
		fclose(fpFile);
		if(argc == 2){
			fpCode = fopen("hex2code.bin", "wb");
		}else{
			fpCode = fopen(argv[2], "wb");
		}
		if(fpFile == NULL){
			printf("Create file failed.\n");
			return;
		}
		strcpy(buf, "{\n");
		for(i=0;i<fileSize;i++){
			if((i % 16) == 0){
				fputs(buf, fpCode);
				strcpy(buf, "\t");
			}
			sprintf(buf,"%s0x%02X", buf, (unsigned char)fileBuf[i]);
			if(((i+1)%16)==0){
				strcat(buf,",\n");
			}else{
				strcat(buf,", ");
			}
		}
		if(strlen(buf)>strlen("0x**")){
			fputs(buf, fpCode);
		}
		strcpy(buf, "\n}");
		fputs(buf, fpCode);
		fclose(fpCode);
	}
	}else{
		printf("Invalid parameter.\n");
		return;
	}
}
