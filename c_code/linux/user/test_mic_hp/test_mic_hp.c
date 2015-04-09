#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/soundcard.h>
#include <string.h>

#define LENGTH 2
#define RATE 16000
#define SIZE 16
#define CHANNELS 2
#define AUDIO_DEV "/dev/jz-dmic"
#define BUF_SIZE (int)(1.0*LENGTH*RATE*SIZE*CHANNELS/8.0)

#define MIXER_DEV "/dev/snd/mixer"
#define VOL_LEVEL 10

int main(int argc, char* argv[])
{
	char *buf;
	char audio_dev[32];
	char mixer_dev[32];
	int rate, buf_size, vol_level;
	int fd_read, fd_write, fd_vol;
	int arg, i;

	if(argc >= 2)
		strcpy(audio_dev, argv[1]);
	else
		strcpy(audio_dev, AUDIO_DEV);
	if(argc >= 3)
		rate = atoi(argv[2]);
	else
		rate = RATE;
	if(argc >= 5){
		strcpy(mixer_dev, argv[3]);
		vol_level = atoi(argv[4]);
	}else{
		strcpy(mixer_dev, MIXER_DEV);
		vol_level = VOL_LEVEL;
	}
	//set vol
	if((fd_vol = open(mixer_dev, O_RDONLY)) < 0){
		perror("open mixer file failed");
	}else{
		ioctl(fd_vol, MIXER_WRITE(0), &vol_level);
		close(fd_vol);
	}

	buf_size  = (int)(1.0*LENGTH*rate*SIZE*CHANNELS/8.0);
	buf = malloc(buf_size);
	if(buf < 0){
		perror("malloc failed");
		exit(1);
	}
	if((fd_write = open(audio_dev,O_WRONLY)) < 0){
		perror("open dsp file for writing failed");
	}else{
		arg = SIZE;
		ioctl(fd_write , SOUND_PCM_WRITE_BITS, &arg);
		arg = CHANNELS;
		ioctl(fd_write , SOUND_PCM_WRITE_CHANNELS, &arg);
		arg = rate;
		ioctl(fd_write , SOUND_PCM_WRITE_RATE, &arg);
	}
	if((fd_read = open(audio_dev,O_RDONLY)) < 0){
		perror("open dsp file for reading failed");
		for(i=0;i<buf_size;i++){
			buf[i] = rand();
		}
	}

	while(1){
		if(fd_read < 0 && fd_write < 0)exit(1);
		if(fd_read > 0){
			int sum = 0;
			printf("recording..\n");
			read(fd_read, buf, buf_size);
			for(i=0;i<buf_size;i++){
				sum += buf[i];
			}
			if(sum = 0){
				printf("warning ======================> no sound detected.\n");
			}
		}
		if(fd_write > 0){
			printf("playing..\n");
			write(fd_write,buf,buf_size);
		}
	}
}
