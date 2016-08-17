#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "getch.h"

static int timediff_us(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec-start->tv_sec)*1000000+(end->tv_usec-start->tv_usec);
}

int main()
{
	int key;
	struct timeval t_start, t_end;
	gettimeofday(&t_start, NULL);
	for (;;) {
		key = getkey();
		gettimeofday(&t_end, NULL);
		if(key == -1){
				continue;
		}else{
			char buf[80];
			sprintf(buf, "key =%3d(0x%02x) timediff = %d us\n", key, key,
					timediff_us(&t_start, &t_end)
				   );
			t_start = t_end;
			printf("%s", buf);
		}
	}
	return 0;
}
