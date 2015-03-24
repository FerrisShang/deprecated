#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/soundcard.h>

#define LENGTH 1
#define RATE 44200
#define SIZE 16
#define CHANNELS 1
#define AUDIO_DEV "/dev/jz-dmic"
#define BUF_SIZE (int)(1.0*LENGTH*RATE*SIZE*CHANNELS/8.0)

#define VOL_LEVEL 0

unsigned char buf[BUF_SIZE];
int main(int argc, char* argv[])
{
	int fd_read, fd_write, fd_vol;
	int arg;

	
	printf("open "AUDIO_DEV"\n");
	fd_read = open(AUDIO_DEV, O_RDONLY);
	arg = SIZE;
	ioctl(fd_read, SOUND_PCM_WRITE_BITS, &arg);
	arg = CHANNELS;
	ioctl(fd_read, SOUND_PCM_WRITE_CHANNELS, &arg);
	arg = RATE;
	ioctl(fd_read, SOUND_PCM_WRITE_RATE, &arg);

	while(1){
		int i,all_zero_flag;
		int sum;
		read(fd_read,buf,BUF_SIZE);
		sum = 0;
		all_zero_flag = 1;
		for(i=0;i<BUF_SIZE;i++){
			if(buf[i] != 0)
				all_zero_flag = 0;
			if(buf[i]>=0)
				sum += buf[i];
			else
				sum -= buf[i];
		}
		if(all_zero_flag)
			printf("no voice input..  ");
		printf("ave = %d\n", sum/BUF_SIZE);
	}
}
