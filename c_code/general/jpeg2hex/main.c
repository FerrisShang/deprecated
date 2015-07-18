#include "stdio.h"
#include "djpeg_if.h"
#include "jpg_def.h"

int main (int argc, char *argv[])
{
	unsigned char outbuf[100*1024];
	struct djpeg_infile infile;
	struct djpeg_outfile outfile;

	infile.buf = jpg_def;//jpg_def is a jpg image dumpped as an array
	infile.len = sizeof(jpg_def);
	outfile.buf = outbuf;

	if(djpeg(&infile, &outfile)<0){
		printf("tran_error\n");
		return 0;
	}
	printf("output:w-%d,h-%d\n", outfile.width, outfile.height);
	int i,j;
	for(i=0;i<outfile.height;i++){
		for(j=0;j<outfile.width;j++){
			int idx = i*outfile.width+j;
			if((idx % outfile.width) == 0)
				printf("\n");
			if((((int)outbuf[idx*3]+(int)outbuf[idx*3+1]+(int)outbuf[idx*3+2])/3) < 0x80){
				printf("*");
			}else{
				printf(" ");
			}
		}
	}
	printf("\n");
	return 0;
}
