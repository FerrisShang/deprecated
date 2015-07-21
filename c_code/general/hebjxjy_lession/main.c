#include <stdio.h>
#include "study.h"


int main(int argc, char *argv[]) {
	struct study_para std_para;
	strcpy(std_para.name, "130302197211203941");
	strcpy(std_para.pass, "203941");
	strcpy(std_para.sel_courses, "id=72&id=73&id=74&id=76&id=78&id=79&id=123&id=94");

	study((void*)&std_para);
	if(std_para.ret_value > 0){
		printf("study done.\n");
	}else{
		printf("study error.\n");
	}
	//system("pause");
	return 0;
}
