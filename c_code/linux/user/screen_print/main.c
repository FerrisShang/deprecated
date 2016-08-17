#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "screen.h"

static void show(int direct);

int main()
{
	int key;
	for (;;) {
		key = getkey();
		switch(key){
			case 'w':
			case 'W':
				show(0);
				break;
			case 'd':
			case 'D':
				show(1);
				break;
			case 's':
			case 'S':
				show(2);
				break;
			case 'a':
			case 'A':
				show(3);
				break;
			default:
				break;
		}
	}
	return 0;
}

static void show(int direct)
{
	static int x, y;
	if(direct == 0){
		if(y>0)y--;
	}else if(direct == 1){
		if(x<80)x+=2;
	}else if(direct == 2){
		if(y<30)y++;
	}else if(direct == 3){
		if(x>0)x-=2;
	}
	screen_clear();
	screen_go(x, y);
	screen_print("① ");
}
