#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "chk_code.h"
#include "para.h"

static void img_filter(unsigned char *img)
{
	int i,j;
	unsigned char *tbuf;
	tbuf = malloc(CODE_IMG_HEIGHT*CODE_IMG_WIDTH);
	memcpy(tbuf, img, CODE_IMG_HEIGHT*CODE_IMG_WIDTH);
	for(i=LETTER_OFF_Y;i<LETTER_OFF_Y+LETTER_HEIGHT;i++){
		for(j=LETTER_OFF_X;j<LETTER_OFF_X+LETTER_WIDTH*4;j++){
			char cnt = 0;
			if(tbuf[(i+1)*CODE_IMG_WIDTH+j]==0){ cnt++; }
			if(tbuf[(i-1)*CODE_IMG_WIDTH+j]==0){ cnt++; }
			if(tbuf[i*CODE_IMG_WIDTH+(j+1)]==0){ cnt++; }
			if(tbuf[i*CODE_IMG_WIDTH+(j-1)]==0){ cnt++; }
			if(cnt >=3){ img[i*CODE_IMG_WIDTH+j] = 0; }
		}
	}
	free(tbuf);
}

static void code_grey(unsigned char *img, unsigned char *buf)
{
	int i,j;
	for(i=0;i<CODE_IMG_HEIGHT;i++){
		for(j=0;j<CODE_IMG_WIDTH;j++){
			if(buf[3*(i*CODE_IMG_WIDTH+j)]<CODE_IMG_GREY_LEVEL ||
					buf[3*(i*CODE_IMG_WIDTH+j)+1]<CODE_IMG_GREY_LEVEL ||
					buf[3*(i*CODE_IMG_WIDTH+j)+2]<CODE_IMG_GREY_LEVEL){
				img[i*CODE_IMG_WIDTH+j] = 1;
			}else{
				img[i*CODE_IMG_WIDTH+j] = 0;
			}
		}
	}
}
static char cal_code(float *para, unsigned char *data)
{
	float max = -10e10, ans[CODE_MAX] = {0.0};
	int i,j;
	char ret;
	for(i=0;i<CODE_MAX;i++){
		ans[i] = 0;
		for(j=0;j<LETTER_WIDTH*LETTER_HEIGHT;j++){
			ans[i] += (float)para[i*(LETTER_WIDTH*LETTER_HEIGHT)+j]*data[j];
		}
	}
	for(i=0;i<CODE_MAX;i++){
		if(max < ans[i]){
			max = ans[i];
			j = i;
		}
	}
	if(j < 9)
		ret = j+'0'+1;
	else
		ret = j-9+'A';
	return ret;
}

char check_code_data(unsigned char* buf, int width, int height, char *ret_code)
{
	unsigned char *img, *p, data[1024];
	int i,j,k;
	if(width != CODE_IMG_WIDTH || height != CODE_IMG_HEIGHT){
		return -1;
	}
	img = (unsigned char*)malloc(CODE_IMG_WIDTH*CODE_IMG_HEIGHT);
	if(img == 0){
		return -1;
	}
	code_grey(img, buf);
	img_filter(img);
	for(i=0;i<4;i++){
		p = data;
		for(k=LETTER_OFF_Y;k<LETTER_OFF_Y+LETTER_HEIGHT;k++){
			for(j=LETTER_OFF_X+i*LETTER_WIDTH;j<LETTER_OFF_X+(i+1)*LETTER_WIDTH;j++){
				*p++ = img[k*CODE_IMG_WIDTH+j];
			}
		}
		ret_code[i] = cal_code(code_para, data);
	}
	free(img);
	return 0;
}
