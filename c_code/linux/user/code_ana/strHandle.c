#include "strHandle.h"
#include "string.h"

struct keyWord keyWord[] = {
	{"auto",		0, 1},
	{"break",		1, 0},
	{"case",		1, 0},
	{"char",		0, 1},
	{"const",		0, 1},
	{"continue",	1, 0},
	{"default",		0, 0},
	{"do",			1, 0},
	{"double",		0, 1},
	{"else",		1, 0},
	{"enum",		0, 0},
	{"extern",		0, 1},
	{"float",		0, 1},
	{"for",			1, 0},
	{"goto",		0, 0},
	{"if",			1, 0},
	{"int",			0, 1},
	{"long",		0, 1},
	{"register",	0, 1},
	{"return",		1, 0},
	{"short",		0, 1},
	{"signed",		0, 1},
	{"sizeof",		0, 0},
	{"static",		0, 0},
	{"struct",		0, 1},
	{"switch",		1, 0},
	{"typedef",		0, 0},
	{"unsigned",	0, 1},
	{"union",		0, 1},
	{"void",		0, 1},
	{"volatile",	0, 1},
	{"while",		0, 0}
};

int keyWordNum = (sizeof(keyWord)/sizeof(keyWord[0]));

int keyWordInx(char* word)
{
	 int i;
	 for(i=0;i<keyWordNum;i++){
		  if(0 == strcmp(keyWord[i].word,word)){
			   return i;
		  }
	 }
	 return -1;
}





