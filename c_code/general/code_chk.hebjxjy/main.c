#include "stdio.h"
#include "chk_code.h"
#include "test_buf.h"

int main (int argc, char *argv[])
{
	int i,j;
	char code[5] = "xxxx";
	for(i=0;i<CODE_IMG_HEIGHT;i++){
		for(j=0;j<CODE_IMG_WIDTH;j++){
			if((test_buf[(i*CODE_IMG_WIDTH+j)*3+0] < 128)||
					(test_buf[(i*CODE_IMG_WIDTH+j)*3+1] < 128)||
					(test_buf[(i*CODE_IMG_WIDTH+j)*3+2] < 128)){
				printf("*");
			}else
				printf(" ");
		}
		printf("\n");
	}
	check_code_data(test_buf, CODE_IMG_WIDTH, CODE_IMG_HEIGHT, code);
	printf("%s\n", code);
	return 0;
}
