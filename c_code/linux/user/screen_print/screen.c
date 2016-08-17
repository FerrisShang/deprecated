#include <stdio.h>
#include <string.h>
#include <termios.h>
#include "screen.h"

#define MAX_WIDTH  80
#define MAX_HEIGHT 32

static char screen[MAX_HEIGHT+1][MAX_WIDTH+1];
static int pos[2];//0 for y, 1 for x
int getkey(void) {
	int character;
	struct termios orig_term_attr;
	struct termios new_term_attr;

	/* set the terminal to raw mode */
	tcgetattr(fileno(stdin), &orig_term_attr);
	memcpy(&new_term_attr, &orig_term_attr, sizeof(struct termios));
	new_term_attr.c_lflag &= ~(ECHO|ICANON);
	new_term_attr.c_cc[VTIME] = 1;
	new_term_attr.c_cc[VMIN] = 0;
	tcsetattr(fileno(stdin), TCSANOW, &new_term_attr);

	/* read a character from the stdin stream without blocking */
	/*   returns EOF (-1) if no character is available */
	character = fgetc(stdin);

	/* restore the original terminal attributes */
	tcsetattr(fileno(stdin), TCSANOW, &orig_term_attr);

	return character;
}

void screen_clear(void)
{
	memset(screen, ' ', sizeof(screen));
}
static void screen_add(const char *fmt, ...)
{
	char buf[MAX_WIDTH+1];
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(buf, MAX_WIDTH, fmt, ap);
	memcpy(&screen[pos[0]][pos[1]], buf, strlen(buf));
	va_end(ap);
}
static void screen_update(void)
{
	int i, j;
	system("clear");
	for(i=0;i<MAX_HEIGHT;i++){
		for(j=0;j<MAX_WIDTH;j++){
			if(screen[i][j] >= 0 && screen[i][j] < 31){
				putchar(' ');
			}else{
				putchar(screen[i][j]);
			}
		}
		printf("\n");
	}
}
void screen_print(const char *fmt, ...)
{
	screen_add(fmt);
	screen_update();
}
void screen_go(int x, int y)
{
	pos[0] = y;
	pos[1] = x;
}
