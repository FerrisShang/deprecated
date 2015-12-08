#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fileUtil.h"
#include "strUtil.h"
#define COM_SYMBOL "list"
#define ENTER 0x0a

struct char_status{
	char isMark_one;
	char isMark_multi;
	char isMacro;
	char isQuotes;
	char issQuotes;
	char isStartOfLine;
	char isStartOfCom;
	char lastPrtChar;
	char parent_lev;
	char brack_lev;
};

void prepro(const char* file, int size, struct char_status *cState)
{
	int tNum;
	char preChar = '\0';
	struct char_status state;

	memset(&state, 0, sizeof(struct char_status));
	state.isStartOfLine = 1;

	for(tNum=0;tNum<size;tNum++){
		if(isPrtChar(file[tNum])){
			state.lastPrtChar = file[tNum];
		}
		if(state.isMark_one == 1){
			if(file[tNum] == ENTER){
				state.isMark_one = 0;
			}
			continue;
		}else if(state.isMark_multi == 1){
			if(file[tNum] == '/' && preChar == '*'){
				state.isMark_multi = 0;
			}
			continue;
		}else if(file[tNum] == '*' && preChar == '/'){
			state.isMark_multi = 1;
			continue;
		}else if(file[tNum] == '/' && preChar == '/'){
			state.isMark_one = 1;
			continue;
		}

		if(file[tNum] == ENTER){
			state.isStartOfLine = 1;
			if(state.isMacro == 1 && preChar == '\\'){
				state.isMacro = 0;
			}
		}else if(state.isStartOfLine == 1 && isVisChar(file[tNum] == 1)){
			state.isStartOfLine = 0;
		}

		if(state.isMacro == 1 && file[tNum] == ENTER){
			if(preChar != '\\'){
				state.isMacro = 0;
			}
		}else if(file[tNum] == '#' && cState[tNum-1].isStartOfLine == 1){
			state.isMacro = 1;
		}

		if(file[tNum] == '\'' && preChar != '\\'){
			state.issQuotes = !state.issQuotes;
		}else if(file[tNum] == '\"' && preChar != '\\'){
			state.isQuotes = !state.isQuotes;
		}
		if(tNum >0 && state.isQuotes == 0 && state.issQuotes == 0){
			if(file[tNum] == '{'){
				state.brack_lev++;
			}else if(file[tNum] == '}'){
				state.brack_lev--;
			}else if(file[tNum] == '('){
				state.parent_lev++;
			}else if(file[tNum] == ')'){
				state.parent_lev--;
			}
		}
#if 0
		if(file[tNum] == ';' && parent_lev == 0 &&
				state.isMark_one == 0 && state.isMark_multi == 0){
			isStartOfCom = 1;
		}
#endif
		cState[tNum] = state;
		preChar = file[tNum];
	}
}

int fileProcess(const char* file, int size)
{
	struct char_status *pChStatus;
	int i;
	pChStatus = (struct char_status *)malloc(size * sizeof(struct char_status));
	prepro(file, size, pChStatus);
	for(i=1;i<size;i++){
		if(pChStatus[i].lastPrtChar == '{' &&
				pChStatus[i-1].lastPrtChar == ')' &&
				0 == pChStatus[i].parent_lev &&
				1 == pChStatus[i].brack_lev &&
				0 == pChStatus[i].isMark_one &&
				0 == pChStatus[i].isMark_multi &&
				0 == pChStatus[i].isMacro &&
				0 == pChStatus[i].isQuotes
				){
			printf("!!!==========\n");
		}
		//putchar(pChStatus[i].lastPrtChar);
	}
	free(pChStatus);
	return 0;
}

int findRet(const char* file, int size)
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
			fileProcess(fileBuf,fileSize);
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
