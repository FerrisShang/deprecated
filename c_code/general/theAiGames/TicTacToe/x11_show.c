#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <stdlib.h>
#include "x11_show.h"

#define BASE_X 10
#define BASE_Y 90
#define WIDTH  300.0
#define HEIGHT 300.0

#define W1     (WIDTH)
#define H1     (HEIGHT)

#define W2     (W1*5/(3*6))
#define W2S    (W1/(3*12))
#define W3     (W2*5/(3*6))
#define W3S    (W2/(3*12))
#define H2     (H1*5/(3*6))
#define H2S    (H1/(3*12))
#define H3     (H2*5/(3*6))
#define H3S    (H2/(3*12))

static Display *dis;
static int screen;
static Window win;
static GC gc;

void init_x(void);
void close_x(void);
void redraw(void);

int x11_show(void)
{
	XEvent event;
	KeySym key;
	char text[255];
	init_x();
	while(1){
		XNextEvent(dis, &event);

		if (event.type==Expose && event.xexpose.count==0) {
			redraw();
		}
		if (event.type==KeyPress&&
				XLookupString(&event.xkey,text,255,&key,0)==1) {
			if (text[0]=='q') {
				close_x();
			}
			printf("You pressed the %c key! code=%d\n",text[0], text[0]);
		}
		if (event.type==KeyRelease){
			printf("You release the %d key! \n", event.xkey.keycode);
		}
		if (event.type==ButtonPress) {
			int x=event.xbutton.x, y=event.xbutton.y;
			printf("%d\n", is_inTTT(x,y));
			/*
			strcpy(text,"X");
			XSetForeground(dis,gc,rand());
			XDrawString(dis,win,gc,x,y, text, strlen(text));
			XDrawPoint(dis, win, gc, x+5, y+5);
			XDrawLine(dis, win, gc, 20, 20, x, y);
			XDrawRectangle(dis, win, gc, x-10, y-10, 20, 20);
			*/
		}
	}
}

void init_x(void)
{
	/* get the colors black and white (see section for details) */
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
	screen=DefaultScreen(dis);
	black=BlackPixel(dis, screen);
	white=WhitePixel(dis, screen);
	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,
			WIDTH + 2*BASE_X, HEIGHT+BASE_Y+10, 5,black, white);
	XSetStandardProperties(dis,win,"","",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask|KeyReleaseMask);
	gc=XCreateGC(dis, win, 0,0);
	XSetBackground(dis,gc,white);
	XSetForeground(dis,gc,black);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x(void)
{
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);
	exit(1);
};

static void draw_rect(int x, int y, int w, int h)
{
	XDrawRectangle(dis, win, gc, BASE_X+x, BASE_Y+y, w, h);
}

void redraw(void)
{
	int mx,my,sx,sy;
	XClearWindow(dis, win);

	draw_rect(0,0,W1,H1);
	for(mx=0;mx<3;mx++){
		for(my=0;my<3;my++){
			int smx = (1+2*mx)*W2S + mx*W2;
			int smy = (1+2*my)*H2S + my*H2;
			draw_rect(smx,smy,W2,H2);
			for(sx=0;sx<3;sx++){
				for(sy=0;sy<3;sy++){
					int ssx = (1+2*sx)*W3S + sx*W3;
					int ssy = (1+2*sy)*H3S + sy*H3;
					draw_rect(smx+ssx,smy+ssy,W3,H3);
				}
			}
		}
	}
	int i;
	for(i=0;i<9;i++){
		draw_s(i,0,rand());
		draw_m(i/3,i%3,rand());
	}
}

void draw_m(int x, int y, int col)
{
	int smx = (1+2*x)*W2S + x*W2;
	int smy = (1+2*y)*H2S + y*H2;
	XSetForeground(dis,gc,col);
	draw_rect(smx,smy,W2,H2);
}
void draw_s(int x, int y, int col)
{
	int smx = (1+2*(x/3))*W2S + (x/3)*W2;
	int smy = (1+2*(y/3))*H2S + (y/3)*H2;
	int ssx = (1+2*(x%3))*W3S + (x%3)*W3;
	int ssy = (1+2*(y%3))*H3S + (y%3)*H3;
	XSetForeground(dis,gc,col);
	draw_rect(smx+ssx,smy+ssy,W3,H3);
}
void draw_p(int x, int y, int col)
{

}
int is_inTTT(int x, int y)
{
	int mx,my,sx,sy;
	x -= BASE_X;
	y -= BASE_Y;
	for(mx=0;mx<3;mx++){
		for(my=0;my<3;my++){
			int smx = (1+2*mx)*W2S + mx*W2;
			int smy = (1+2*my)*H2S + my*H2;
			for(sx=0;sx<3;sx++){
				for(sy=0;sy<3;sy++){
					int ssx = (1+2*sx)*W3S + sx*W3;
					int ssy = (1+2*sy)*H3S + sy*H3;
					if(x>=smx+ssx && x<=smx+ssx+W3
							&& y>=smy+ssy && y<=smy+ssy+H3){
						return mx*3+sx + (my*3+sy)*9;
					}
				}
			}
		}
	}
	return -1;
}
