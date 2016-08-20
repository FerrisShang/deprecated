#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "terminal_color.h"
#include "get_screen_png.h"
#include "png_read.h"

#define SIZE 20
#if 0
#define WIDTH  1080
#define HEIGHT 1920
#else
#define WIDTH  1440
#define HEIGHT 2560
#endif

void parse_png(struct png *png, void *pdata)
{
	printf("width = %d\nheight = %d\n", png->width, png->height);
	int show[HEIGHT/SIZE][WIDTH/SIZE][3];
	int i, j, m, n;
	unsigned char (*p)[WIDTH][4];
	p = (unsigned char (*)[WIDTH][4])png->image;
	memset(show, 0, sizeof(show));

	for(i=0;i<HEIGHT;i+=SIZE){
		for(j=0;j<WIDTH;j+=SIZE){
			int r=0,g=0,b=0;
			for(m=0;m<SIZE;m++){
				for(n=0;n<SIZE;n++){
					r += p[i+m][j+n][0];
					g += p[i+m][j+n][1];
					b += p[i+m][j+n][2];
				}
			}
			r /= (SIZE*SIZE);
			g /= (SIZE*SIZE);
			b /= (SIZE*SIZE);
			show[i/SIZE][j/SIZE][0] = r;
			show[i/SIZE][j/SIZE][1] = g;
			show[i/SIZE][j/SIZE][2] = b;
		}
	}
#if 0
	printf("%d\n", time(NULL));
#else
	system("clear");
	for(i=0;i<HEIGHT/SIZE;i++){
		for(j=0;j<WIDTH/SIZE;j++){
			set_color_8bit(show[i][j][0], show[i][j][1], show[i][j][2]);
			printf("  ");
		}
		set_color_8bit(0,0,0);
		printf("\n");
	}
#endif
}

int main(int argc, char *argv[])
{
	struct screen_png *screen;
	int res;
	init_col();
	while(1){
		screen = get_screen_png();
		if(!screen){
			printf("get screen failed\n");
			return -1;
		}
		res = png_read(screen->data, screen->size, parse_png, NULL);
		if(res < 0){
			printf("read png file failed\n");
			return -1;
		}
	}
}
