#include <stdio.h>
#include "study.h"


int main(int argc, char *argv[]) {
	struct study_para std_para;
	strcpy(std_para.name, "123123");
	strcpy(std_para.pass, "123123");

	study((void*)&std_para);
	if(std_para.ret_value > 0){
		printf("study done.\n");
	}else{
		printf("study error.\n");
	}
	//system("pause");
	return 0;
}
