#ifndef __STR_HANDLE_H__
#define __STR_HANDLE_H__

struct keyWord{
	 char word[12];
	 char isCtrl;
	 char isType;
};
extern struct keyWord keyWord[]; 
extern keyWordNum;

int keyWordInx(char* word);

#endif /* __STR_HANDLE_H__ */
