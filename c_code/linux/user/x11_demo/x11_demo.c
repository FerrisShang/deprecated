/* 
   sudo apt-get install libx11-dev
   gcc x11_demo.c -lX11
*/

/* include the X library headers */
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

/* include some silly stuff */
#include <stdio.h>
#include <stdlib.h>

/* here are our X variables */
Display *dis;
int screen;
Window win;
GC gc;

/* here are our X routines declared! */
void init_x();
void close_x();
void redraw();

int main () {
	XEvent event;		/* the XEvent declaration !!! */
	KeySym key;		/* a dealie-bob to handle KeyPress Events */	
	char text[255];		/* a char buffer for KeyPress Events */

	init_x();

	while(1) {		
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
			strcpy(text,"X");
			XSetForeground(dis,gc,rand());
			XDrawString(dis,win,gc,x,y, text, strlen(text));
			XDrawPoint(dis, win, gc, x+5, y+5);
			XDrawLine(dis, win, gc, 20, 20, x, y);
			XDrawRectangle(dis, win, gc, x-10, y-10, 20, 20);
		}
	}
}

void init_x() {
/* get the colors black and white (see section for details) */        
	unsigned long black,white;

	dis=XOpenDisplay((char *)0);
   	screen=DefaultScreen(dis);
	black=BlackPixel(dis,screen),
	white=WhitePixel(dis, screen);
   	win=XCreateSimpleWindow(dis,DefaultRootWindow(dis),0,0,	
		300, 300, 5,black, white);
	XSetStandardProperties(dis,win,"Howdy","Hi",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask|KeyReleaseMask);
        gc=XCreateGC(dis, win, 0,0);        
	XSetBackground(dis,gc,white);
	XSetForeground(dis,gc,black);
	XClearWindow(dis, win);
	XMapRaised(dis, win);
};

void close_x() {
	XFreeGC(dis, gc);
	XDestroyWindow(dis,win);
	XCloseDisplay(dis);	
	exit(1);				
};

void redraw() {
	XClearWindow(dis, win);
};

