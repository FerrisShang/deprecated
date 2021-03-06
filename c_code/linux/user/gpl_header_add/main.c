#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "list_file.h"

char *AUTHOR  = "Shang Feng";
char *EMAIL   = "feng.shang@ingenic.com";
char *COMPANY = "Ingenic Semiconductor CO.,LTD.";
#define IDX_FILENAME 1
#define IDX_COMPANY  3
#define IDX_AUTHER   18

char *gpl_str[] = {
	"/*",
	" *",
	" *",
	" * Copyright (C)",
	" *",
	" * This program is free software: you can redistribute it and/or modify",
	" * it under the terms of the GNU General Public License as published by",
	" * the Free Software Foundation, either version 3 of the License, or",
	" * (at your option) any later version.",
	" *",
	" * This program is distributed in the hope that it will be useful,",
	" * 	 but WITHOUT ANY WARRANTY; without even the implied warranty of",
	" * 	 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the",
	" * 	 GNU General Public License for more details.",
	" *",
	" * 	 You should have received a copy of the GNU General Public License",
	" * 	 along with this program.  If not, see <http://www.gnu.org/licenses/>.",
	" *",
	" * Author:",
	" *",
	" */",
};

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
int processFunc(char *path, char* buf, int size)
{
	FILE *fp;
	int i;
	for(i=0;i<(int)(size-strlen(COMPANY));i++){
		if(!memcmp(&buf[i], COMPANY, strlen(COMPANY))){
			return 0;
		}
	}
	fp = fopen(path,"wb");
	if(fp != NULL){
		char tStr[1024], tFileName[80];
		int idx, j;
		for(i=0;i<sizeof(gpl_str)/sizeof(gpl_str[0]);i++){
			if(i == IDX_FILENAME){
				strcpy(tFileName, path);
				idx = 0;
				for(j=strlen(tFileName)-1;j>=0;j--){
					if(tFileName[j] == '/'){
						idx = j+1;
						break;
					}
				}
				sprintf(tStr, "%s %s\n", gpl_str[i], &tFileName[idx]);
				fwrite(tStr, strlen(tStr), 1, fp);
			}else if(i == IDX_COMPANY){
				time_t t = time(NULL);
				struct tm tm = *localtime(&t);
				sprintf(tStr, "%s %d %s\n", gpl_str[i],
						tm.tm_year+1900, COMPANY);
				fwrite(tStr, strlen(tStr), 1, fp);
			}else if(i == IDX_AUTHER){
				sprintf(tStr, "%s %s <%s>\n", gpl_str[i], AUTHOR, EMAIL);
				fwrite(tStr, strlen(tStr), 1, fp);
			}else{
				fwrite(gpl_str[i], strlen(gpl_str[i]), 1, fp);
				fwrite("\n", strlen("\n"), 1, fp);
			}
		}
		fwrite(buf,size,1,fp);
		fclose(fp);
	}
	return 0;
}
int fileFunc(const char* path,void *para)
{
	FILE *fp;
	char *fileBuf;
	int fileSize = get_file_size(path);
	if(!(fileSize > 0 &&
				(strstr(path, ".c")-path+strlen(".c")==strlen(path) ||
				 strstr(path, ".h")-path+strlen(".h")==strlen(path) ||
				 strstr(path, ".cpp")-path+strlen(".cpp")==strlen(path))
		)){
		return -1;
	}
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fp = fopen(path,"rb");
		if(fp != NULL){
			fread(fileBuf,fileSize,1,fp);
			fclose(fp);
			processFunc((char*)path, fileBuf,fileSize);
		}
	}
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
