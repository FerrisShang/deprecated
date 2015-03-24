#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/input.h>
#include <termios.h>
#include <ctype.h>
#include "ShootImage.h"

#define SHOOT_KEY_VAL		 116
#define DEF_FORMAT_INDEX	 0
#define DEF_SHOOT_INTERVAL	 30
#define DEF_INPUT_DEV		 "/dev/input/event0"
#define DEF_SAVE_PATH		 "/picture"

struct pic_format{
	char *info;
	int width;
	int height;
};
struct pic_format pic_format[] = {
	{
		"1280*720",
		1280,
		720,
	},
	{
		"640*480",
		640,
		480,
	},
};
int pic_format_index = DEF_FORMAT_INDEX;
int timer_interval_s = DEF_SHOOT_INTERVAL;
char save_path[64]   = DEF_SAVE_PATH;
char input_dev[64]   = DEF_INPUT_DEV;

void take_photo(void)
{
	unsigned char *path = (unsigned char*)save_path;
	shootimage_icam(path,
	pic_format[pic_format_index].width,
	pic_format[pic_format_index].height);
}

void timer_handle(int signo)
{
	printf("shoot image.\n");
	take_photo();
}

int main(int argc, char *argv[])
{

	int keys_fd, num, i;
	char ch, timer_start = 0;
	struct input_event t;
	struct itimerval value;
	opterr = 0;
	while((ch = getopt(argc,argv,"t:f:p:d:h")) != -1){
		switch(ch)
		{
			case 'h'://help
				printf(
						"Usage: shootimg [OPTION]... \n\n"
						" -h     print this message\n"
						" -t     shoot interval, default is %ds\n"
						" -f     photo format, default is %s\n"
						" -p     save path, default is %s\n"
						" -d     input event device, default is %s\n\n"
						"Supported photo formats are: 1080*720, 640*480\n",
						DEF_SHOOT_INTERVAL,
						pic_format[DEF_FORMAT_INDEX].info,
						DEF_SAVE_PATH,
						DEF_INPUT_DEV
					  );
				return 0;
				break;
			case 't'://interval time
				num = atoi(optarg);
				if(num == 0){
					printf("Invalid interval time, setting 30s\n");
				}else{
					timer_interval_s = num;
				}
				break;
			case 'p'://save_path
				strcpy((char*)save_path, optarg);
				break;
			case 'd'://input_dev
				strcpy(input_dev, optarg);
				break;
			case 'f'://format
				pic_format_index = -1;
				for(i=0;i<sizeof(pic_format)/sizeof(pic_format[0]);i++){
					if(!strcmp(optarg, pic_format[i].info)){
						pic_format_index = i;
						break;
					}
				}
				if(pic_format_index == -1){
					printf("Invalid format, setting %s\n", pic_format[0].info);
					pic_format_index = DEF_FORMAT_INDEX;
				}
				break;
			default:
				break;
		}
	}
	printf(
			"shootimg started.\n"
			"  Interval     : %ds\n"
			"  Save path    : %s\n"
			"  Input device : %s\n"
			"  Format       : %s\n"
			"press power key to start/stop taking photo.\n",
			timer_interval_s, save_path,
			input_dev, pic_format[pic_format_index].info
		  );
	keys_fd = open (input_dev, O_RDONLY);
	if (keys_fd <= 0){
		printf ("open %s device error!\n", input_dev);
		return 0;
	}
	while(1){
		if (read (keys_fd, &t, sizeof (t)) == sizeof (t)){
			if (t.type == EV_KEY){
				//printf ("key_val=%d %s\n", t.code, (t.value) ? "Pressed" : "Released");
				if (t.code == SHOOT_KEY_VAL && t.value == 1){
					if(timer_start == 0){
						timer_start = 1;
						printf("start take photo.\n");
						value.it_value.tv_sec = 1;
						value.it_value.tv_usec = 0;
						value.it_interval.tv_sec = timer_interval_s;
						value.it_interval.tv_usec = 0;
						signal(SIGALRM, timer_handle);
						setitimer(ITIMER_REAL, &value, NULL);
					}else{
						timer_start = 0;
						printf("stop take photo.\n");
						value.it_value.tv_sec = 0;
						value.it_value.tv_usec = 0;
						value.it_interval.tv_sec = 0;
						value.it_interval.tv_usec = 0;
						setitimer(ITIMER_REAL, &value, NULL);
					}
				}
			}
		}
	}
	close (keys_fd);
	return 0;
}
