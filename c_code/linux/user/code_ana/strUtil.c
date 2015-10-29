#include <string.h>
#include <stdio.h>
#include "strUtil.h"

char isVarChar(char ch)
{
	if(ch >= 'a' && ch <= 'z') return 1;
	if(ch >= 'A' && ch <= 'Z') return 1;
	if(ch >= '_') return 1;
	return 0;
}

char isPrtChar(char ch)
{
	if(ch >= ' ' && ch <= '~') return 1;
	return 0;
}

char isVisChar(char ch)
{
	if(ch > ' ' && ch <= '~') return 1;
	return 0;
}

char *strbVis(char *str, int len)
{
	while(len){
		len--;
		str--;
		if(isVisChar(*str)){
			return str;
		}
	}
	return 0;
}
