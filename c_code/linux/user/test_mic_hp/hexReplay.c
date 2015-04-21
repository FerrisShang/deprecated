#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include <string.h>

int main(int argc, char* argv[])
{
	char audio_dev_w[32];
	char *buf;
	int fd_write, i, arg, buf_size;
	FILE *fp;

	if(argc != 7){
		printf("Usage: dspSetting [device] [rate] [channels] [bits] [file] [file size]\n");
		return;
	}
	strcpy(audio_dev_w, argv[1]);
	printf("device : %s\n", audio_dev_w);
	if((fd_write = open(audio_dev_w, O_WRONLY)) < 0){
		perror("open dsp file for reading failed");
		return;
	}else{
		arg = atoi(argv[2]);
		printf("RATE   : %d\n", arg);
		if(ioctl(fd_write, SNDCTL_DSP_SPEED, &arg)<0){
			perror("setting error\n");
			return;
		}
		arg = atoi(argv[3]);
		printf("CHANNEL: %d\n", arg);
		if(ioctl(fd_write, SNDCTL_DSP_CHANNELS, &arg)<0){
			perror("setting error\n");
			return;
		}
		if(atoi(argv[4]) == 8)
			arg = AFMT_U8;
		else
			arg = AFMT_S16_LE;
		printf("BITS   : %d\n", arg);
		if(ioctl(fd_write, SNDCTL_DSP_SETFMT, &arg)<0){
			perror("setting error\n");
			return;
		}
		printf("setting done..\n");
		fp = fopen(argv[5],"r");
		if(fp<=0){
			perror("open file failed\n");
			return;
		}
		buf_size = atoi(argv[6]);
		buf = malloc(buf_size);
		if(buf<=0){
			perror("locate memory failed\n");
			return;
		}
		for(i=0;i<buf_size/4096;i++){
			fread(buf, 1, 4096, fp);
			write(fd_write, buf, 4096);
		}
		free(buf);
		close(fd_write);
	}
}
