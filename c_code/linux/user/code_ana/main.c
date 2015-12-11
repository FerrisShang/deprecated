#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "fileUtil.h"
#include "strUtil.h"
#define COM_SYMBOL "list"
#define ENTER 0x0a

char *addString = "\nALOGD(\"fsDbg:%s,%s,%d\",__FILE__,__func__,__LINE__);";
//char *addString = "\nFSHANG_FOR_DEBUG(__FILE__,__func__,__LINE__);";
//char *addString = "LogMsg(0,\"fsDbg: %%s,%%s,%%d\",__FILE__,__func__,__LINE__);\n";
//char *addStringAtHeader = "#include \"fshang.h\"\n";
char *addStringAtHeader = "";

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
	char isPreCharStr = 0;
	struct char_status state;

	memset(&state, 0, sizeof(struct char_status));
	state.isStartOfLine = 1;

	for(tNum=0;tNum<size;tNum++){
		if(isVisChar(file[tNum])){
			state.lastPrtChar = file[tNum];
		}
		if(state.isMark_one == 1){
			if(file[tNum] == ENTER){
				state.isMark_one = 0;
			}
			preChar = file[tNum];
			continue;
		}else if(state.isMark_multi == 1){
			if(file[tNum] == '/' && preChar == '*'){
				state.isMark_multi = 0;
			}
			preChar = file[tNum];
			continue;
		}else if(file[tNum] == '*' && preChar == '/'){
			state.isMark_multi = 1;
			preChar = file[tNum];
			continue;
		}else if(file[tNum] == '/' && preChar == '/'){
			state.isMark_one = 1;
			preChar = file[tNum];
			continue;
		}

		if(file[tNum] == ENTER){
			state.isStartOfLine = 1;
		}else if(state.isStartOfLine == 1 && isVisChar(file[tNum] == 1)){
			state.isStartOfLine = 0;
		}

		if(state.isMacro == 1 && file[tNum] == ENTER){
			if(preChar != '\\'){
				state.isMacro = 0;
			}
		}else if(file[tNum] == '#' &&
				(tNum == 0 || state.isStartOfLine == 1)){
			state.isMacro = 1;
		}

		if(state.isQuotes == 0 && file[tNum] == '\''){
			if(state.issQuotes == 1){
				if(isPreCharStr == 0 && preChar == '\\'){
					isPreCharStr = 1;
				}else{
					state.issQuotes = !state.issQuotes;
					isPreCharStr = 0;
				}
			}else{
				state.issQuotes = !state.issQuotes;
				isPreCharStr = 0;
			}
		}else if(state.issQuotes == 0 && file[tNum] == '\"'){
			if(state.isQuotes == 1){
				if(isPreCharStr == 0 && preChar == '\\'){
					isPreCharStr = 1;
				}else{
					state.isQuotes = !state.isQuotes;
					isPreCharStr = 0;
				}
			}else{
				state.isQuotes = !state.isQuotes;
				isPreCharStr = 0;
			}
		}else if((state.issQuotes == 1 || state.isQuotes == 1) && 
				isPreCharStr == 0 && preChar == '\\'){
			isPreCharStr = 1;
		}else{
			isPreCharStr = 0;
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

int findfunc(const struct char_status *pChStatus, int statusSize, int *buf, int bufSize)
{
	int i;
	int funcNum = 0;
	for(i=1;i<statusSize;i++){
		if(pChStatus[i].lastPrtChar == '{' &&
				pChStatus[i-1].lastPrtChar == ')' &&
				0 == pChStatus[i].parent_lev &&
				1 == pChStatus[i].brack_lev &&
				0 == pChStatus[i].isMark_one &&
				0 == pChStatus[i].isMark_multi &&
				0 == pChStatus[i].isMacro &&
				0 == pChStatus[i].isQuotes
				){
			buf[funcNum] = i;
			funcNum++;
			if(funcNum == bufSize){
				printf("function number reach the max buffer size !\n");
				return funcNum;
			}
		}
	}
	return funcNum;
}

void prt_func(const char *file, int size, int *funcBuf, int funcBufNum)
{
	int i, parentCnt;
	char *pBk = NULL, bkChar;
	for(i=0;i<funcBufNum;i++){
		char *p = (char*)&file[funcBuf[i]];
		parentCnt = 0;
		while(*p){
			if(*p == ')'){
				parentCnt++;
			}else if(*p == '('){
				parentCnt--;
				if(parentCnt == 0){
					bkChar = *p;
					pBk = p;
					*p = '\0';
					break;
				}
			}
			p--;
		}

		while(!isVarChar(*--p));
		while(isVarChar(*--p));
		p++;
		puts(p);
		if(pBk){
			*pBk = bkChar;
		}
	}
}
void addPrt(const char *filePath, const char *file, int size, int *funcBuf, int funcNum)
{
	FILE *fp;
	int i;
	/* // backup files
	char strBuf[80];
	int ret;
	sprintf(strBuf, "mv %s %s.bk", filePath, filePath);
	ret = system(strBuf);
	if(ret != 0){
		printf("process command failed. ret = %d", ret);
	}
	*/
	fp = fopen(filePath, "wb");
	if(fp != NULL){
		int curPos = 0;
		int writeSize;
		if(strlen(addStringAtHeader) > 0){
			fwrite(addStringAtHeader, strlen(addStringAtHeader), 1, fp);
		}
		for(i=0;i<funcNum;i++){
			writeSize = funcBuf[i] - curPos + 1;
			fwrite(&file[curPos], writeSize, 1, fp);
			curPos += writeSize;
			if(strlen(addString) > 0){
				fwrite(addString, strlen(addString), 1, fp);
			}
		}
		writeSize = size - curPos;
		if(writeSize > 0){
			fwrite(&file[curPos], writeSize, 1, fp);
		}
		fclose(fp);
	}else{
		printf("open file: %s error\n", filePath);
	}
}

struct char_status chStatusBuf[1024*1024];
int fileProcess(const char *filePath, const char *file, int size)
{
#define FUNC_BUF_SIZE 1000
	struct char_status *pChStatus;
	int funcNum, funcBuf[FUNC_BUF_SIZE];
	pChStatus = (struct char_status *)malloc(size * sizeof(struct char_status));
	//pChStatus = &chStatusBuf;
	memset(pChStatus, 0, size * sizeof(struct char_status));
	if(pChStatus == NULL){
		printf("malloc failed.");
		return -1;
	}
	prepro(file, size, pChStatus);
	funcNum = findfunc(pChStatus, size, funcBuf, FUNC_BUF_SIZE);
	printf("%s : %d\n", filePath, funcNum);
	prt_func(file, size, funcBuf, funcNum);
	//addPrt(filePath, file, size, funcBuf, funcNum);
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
	if(path[nameLen-1] != 'c' /* && path[nameLen-1] != 'h' */)return 0;
	//printf("%s\n", path);
	fileSize = get_file_size(path);
	fileBuf = (char*)malloc(fileSize);
	if(fileBuf != NULL){
		fp = fopen(path,"rb");
		if(fp != NULL){
			if(fread(fileBuf,fileSize,1,fp)==0){
				return 0;
			}
			fileProcess(path, fileBuf,fileSize);
			fclose(fp);
		}
	}else{
		printf("malloc failed.");
	}
	free(fileBuf);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc < 2){
		List_Files("./",(fileHandle)fileFunc,(void*)0);
	}else{
		if(argc > 2){
			addString = argv[3];
		}
		if(argc > 3){
			addStringAtHeader = argv[4];
		}
		List_Files(argv[1],(fileHandle)fileFunc,(void*)0);
	}
	return 0;
}
