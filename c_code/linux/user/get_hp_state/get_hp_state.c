#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <linux/soundcard.h>

#define AUDIO_DEV "/dev/dsp"
int main(int argc, char* argv[])
{
	int fd_read;
	int arg, i;
	fd_read = open(AUDIO_DEV, O_RDONLY);
	for(i=3;i<3;i++){
		ioctl(fd_read, i, &arg);
		printf("%d = %d\n", i, arg);
	}
}
