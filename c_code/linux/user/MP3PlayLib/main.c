#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <linux/soundcard.h>
#include "string.h"
#include "mp3dec.h"
#define out(...) do{fprintf(stderr, __VA_ARGS__);fflush(stderr);}while(0)
#define BUF_SIZE ((1<<16)-1)
int main(int argc, char *argv[])
{
    int fd, pcm;
	int bytes_left, sync;
	char* file_data;
	char outputBuf[BUF_SIZE];
	MPEG_DECODE_PARAM param;
	if (argc < 2) { out("Error: no input file specified!\n"); return 1; }
	fd = open(argv[1], O_RDONLY);
	if (fd < 0) { out("Error: cannot open %s !\n", argv[1]); return 1; }
	bytes_left = lseek(fd, 0, SEEK_END);
	file_data = (char*)mmap(0, bytes_left, PROT_READ, MAP_PRIVATE, fd, 0);

	mp3DecodeInit();
	MPEG_DECODE_INFO info;
	mp3FindSync((char*)file_data, bytes_left, &sync);
	file_data += sync;
	bytes_left -= sync;
	memset(&info, 0, sizeof(info));
	mp3GetDecodeInfo((char*)file_data, bytes_left, &info, 0);
	out("Channels:%d Bps:%d Frequency:%d BitRate:%d\n",
			info.channels, info.bitsPerSample, info.frequency, info.bitRate);
	out("MinInputSize:%d MaxInputSize:%d OutputSize:%d\n",
			info.minInputSize, info.maxInputSize, info.outputSize);
	mp3DecodeStart((char*)file_data, bytes_left);

    pcm = open("/dev/dsp", O_WRONLY);
    if (pcm < 0) out("cannot open DSP");

    int value = AFMT_S16_LE;
    if (ioctl(pcm, SNDCTL_DSP_SETFMT, &value) < 0)
        out("cannot set audio format");

    if (ioctl(pcm, SNDCTL_DSP_CHANNELS, &info.channels) < 0)
        out("cannot set audio channels");

    if (ioctl(pcm, SNDCTL_DSP_SPEED, &info.frequency) < 0)
        out("cannot set audio sample rate");

	do{
		int i;
		param.inputBuf = file_data;
		param.inputSize = info.maxInputSize;
		param.outputBuf = outputBuf;
		param.outputSize = info.outputSize;
		int res = mp3DecodeFrame(&param);
		if(!res){
			out("fatal err:%d\n", mp3GetLastError()); break;
		}
		for(i=0;i<param.outputSize/2;i++){
			//printf("%d ", ((signed short*)param.outputBuf)[i]);
			((signed short*)param.outputBuf)[i] =
				(((signed short*)param.outputBuf)[i]>>2);
		}
		write(pcm, (const void *) param.outputBuf, param.outputSize);
		file_data += param.inputSize;
		bytes_left -= param.inputSize;
		if(bytes_left <= 0){ printf("decode done\n"); break; }
	}while(param.outputSize != 0);
	return 0;
}
