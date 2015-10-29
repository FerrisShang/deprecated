#include<stdio.h>
#include<stdlib.h>
#include<sys/dir.h>
#include<sys/stat.h>
#include<string.h>
#include "fileUtil.h"

#define recursive 1
static int IS_DIR(const char *path)
{
	struct stat st;
	lstat(path,&st);
	return S_ISDIR(st.st_mode);
}
static void List_Files_Core(const char *path, fileHandle func, void *funcPara)
{
	DIR *pdir;
	struct dirent *pdirent;
	char temp[256];
	pdir = opendir(path);
	if(pdir){
		while(pdirent = readdir(pdir)){ 
			if(strcmp(pdirent->d_name, ".") == 0
					|| strcmp(pdirent->d_name, "..") == 0)
				continue;
			sprintf(temp, "%s/%s", path, pdirent->d_name);
			if(IS_DIR(temp)) {
				List_Files_Core(temp, func, funcPara);
			}else{
				func(temp, funcPara);
			}
		}
	}else{
		printf("opendir error:%s\n", path);
	}
	closedir(pdir);
}

void List_Files(const char *path, fileHandle func, void *funcPara)
{
	int len;
	char temp[256];
	len = strlen(path);
	strcpy(temp, path);
	if(temp[len - 1] == '/') temp[len -1] = '\0';
	if(IS_DIR(temp)) { 
		List_Files_Core(temp, func, funcPara);
	}else{
		func(path, funcPara);
	}
}

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
