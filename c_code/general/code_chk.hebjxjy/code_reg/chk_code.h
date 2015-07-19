#ifndef __CHK_CODE_H__
#define __CHK_CODE_H__

#define CODE_MAX 35
#define CODE_IMG_WIDTH 60
#define CODE_IMG_HEIGHT 25
#define LETTER_OFF_X 11
#define LETTER_OFF_Y 6
#define LETTER_WIDTH 10
#define LETTER_HEIGHT 17

#define CODE_IMG_GREY_LEVEL 128ul

char check_code_data(unsigned char* buf, int width, int height, char *ret_code);

#endif /* __CHK_CODE_H__ */
