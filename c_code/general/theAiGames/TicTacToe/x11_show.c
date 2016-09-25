#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "x11_show.h"

#define BOARD_BASE_X 10
#define BOARD_BASE_Y 90
#define BOARD_WIDTH  300.0
#define BOARD_HEIGHT 300.0

#define W1     (BOARD_WIDTH)
#define H1     (BOARD_HEIGHT)

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

struct ai_ttt *ai;
struct ttt *ttt;
char str[1024];
int m_field[9];
#define COL_RED       0xFF0000
#define COL_YELLOW    0xFFFF00

#define BUTTON_BASE_X 20
#define BUTTON_BASE_Y 30
enum {
	BUTTON_RESET_BOT_FIRST = 0,
	BUTTON_RESET_BOT_SECOND,
	BUTTON_UNDO,
	BUTTON_NUM,
};

static void bot_first(void);
static void bot_second(void);
static void undo(void);
struct xbutton{
	XRectangle r;
	char str[32];
	void (*func)(void);
} xbutton[BUTTON_NUM] = {
	[BUTTON_RESET_BOT_FIRST] = {
		{BUTTON_BASE_X+0,BUTTON_BASE_Y+0,70,28},
		"bot first",
		bot_first,
	},
	[BUTTON_RESET_BOT_SECOND] = {
		{BUTTON_BASE_X+100,BUTTON_BASE_Y+0,70,28},
		"bot second",
		bot_second,
	},
	[BUTTON_UNDO] = {
		{BUTTON_BASE_X+200,BUTTON_BASE_Y+0,70,28},
		"undo",
		undo,
	},
};

void init_x(void);
void close_x(void);
void redraw(void);
int isInButton(int x, int y, struct xbutton *b);
void get_m_field_str(char *str, int *m_field);
void set_m_field(struct ttt *ttt, int *m_field, point_t p);
void get_field_str(char *str, struct ttt *ttt);
void update_board(struct ttt *ttt, point_t p);

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
			bot_second();
		}
		if (event.type==KeyPress&&
				XLookupString(&event.xkey,text,255,&key,0)==1) {
			if (text[0]=='q') {
				close_x();
			}
			printf("You pressed the %c key! code=%d\n",text[0], text[0]);
		}
		if (event.type==ButtonPress) {
			int x=event.xbutton.x, y=event.xbutton.y;
			{// check cursor in board
				point_t pos = is_inTTT(x,y);
				if(pos != (point_t)-1){
					printf("%d,%d\n", POINT_X(pos),POINT_Y(pos));
					if(ttt_add(ttt, ttt_op_id(ai->id), pos) == 0){
						int r;
						update_board(ttt, pos);
						r = ttt_isMFinish(ttt);
						if(r == ID_P1 || r == ID_P2 || r == DRAW){
							printf("finished : %d\n", r);
							continue;
						}
						set_m_field(ttt, m_field, pos);
						get_m_field_str(str, m_field);
						ai_update_mboard(ai, str);
						get_field_str(str, ttt);
						ai_update_field(ai, str);
						printf("mfield=0x%08x\n", ttt->m_field);

						pos = ai_move(ai, 10000);
						ttt_add(ttt, ai->id, pos);

						set_m_field(ttt, m_field, pos);
						get_m_field_str(str, m_field);
						ai_update_mboard(ai, str);
						get_field_str(str, ttt);
						ai_update_field(ai, str);

						update_board(ttt, pos);
						r = ttt_isMFinish(ttt);
						if(r == ID_P1 || r == ID_P2 || r == DRAW){
							printf("finished : %d\n", r);
							continue;
						}
					}else{
						printf("invalid place:%d,%d\n", POINT_X(pos), POINT_Y(pos));
					}
				}
			}
			{// check cursor in button
				int b_num;
				if((b_num = isInButton(x, y, xbutton)) >= 0){
					xbutton[b_num].func();
				}
			}
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
			BOARD_WIDTH + 2*BOARD_BASE_X, BOARD_HEIGHT+BOARD_BASE_Y+10, 5,black, white);
	XSetStandardProperties(dis,win,"","",None,NULL,0,NULL);
	XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);
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
	XDrawRectangle(dis, win, gc, BOARD_BASE_X+x, BOARD_BASE_Y+y, w, h);
}

void redraw(void)
{
	int mx,my,sx,sy;
	XClearWindow(dis, win);
	XSetBackground(dis,gc,0xFFFFFF);
	XSetForeground(dis,gc,0);
	//darw board
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
	//draw button
	int i;
	for(i=0;i<BUTTON_NUM;i++){
		XDrawRectangle(dis, win, gc,
				xbutton[i].r.x, xbutton[i].r.y,
				xbutton[i].r.width, xbutton[i].r.height);
		XDrawString(dis,win,gc,
				xbutton[i].r.x+5,xbutton[i].r.y+20,
				xbutton[i].str, strlen(xbutton[i].str));
	}
}

void draw_m(int x, int y, int col)
{
	int smx = (1+2*x)*W2S + x*W2;
	int smy = (1+2*y)*H2S + y*H2;
	XSetForeground(dis,gc,col);
	draw_rect(smx,smy,W2,H2);
	draw_rect(smx-1,smy-1,W2+2,H2+2);
	draw_rect(smx-2,smy-2,W2+4,H2+4);
}
void draw_s(int x, int y, int col)
{
	int smx = (1+2*(x/3))*W2S + (x/3)*W2;
	int smy = (1+2*(y/3))*H2S + (y/3)*H2;
	int ssx = (1+2*(x%3))*W3S + (x%3)*W3;
	int ssy = (1+2*(y%3))*H3S + (y%3)*H3;
	XSetForeground(dis,gc,col);
	draw_rect(smx+ssx,smy+ssy,W3,H3);
	draw_rect(smx+ssx-1,smy+ssy-1,W3+2,H3+2);
}
void draw_p(int x, int y, int col)
{
	int smx = (1+2*(x/3))*W2S + (x/3)*W2;
	int smy = (1+2*(y/3))*H2S + (y/3)*H2;
	int ssx = (1+2*(x%3))*W3S + (x%3)*W3;
	int ssy = (1+2*(y%3))*H3S + (y%3)*H3;
	XSetForeground(dis,gc,col);
	XFillArc(dis,win,gc,
			BOARD_BASE_X+smx+ssx+2,BOARD_BASE_Y+smy+ssy+2,W3-4,H3-4,
			0,64*360);
}
point_t is_inTTT(int x, int y)
{
	int mx,my,sx,sy;
	x -= BOARD_BASE_X;
	y -= BOARD_BASE_Y;
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
						return ((mx*3+sx)<<4) + (my*3+sy);
						//return ((mx+my*3)<<4) + (sx+sy*3);
					}
				}
			}
		}
	}
	return -1;
}
int isInButton(int x, int y, struct xbutton *b)
{
	int i;
	for(i=0;i<BUTTON_NUM;i++){
		if(x>b[i].r.x && y>b[i].r.y && x<b[i].r.x+b[i].r.width && y<b[i].r.y+b[i].r.height){
			return i;
		}
	}
	return -1;
}

static void bot_init(void)
{
	int i;
	for(i=0;i<9;i++){
		m_field[i] = -1;
	}
	srand(time(0));
	if(ai){
		ai_destory(ai);
	}
	ai = ai_create();
	ai_reset(ai, 10000, 500);
	if(ttt){
		ttt_destory(ttt);
	}
	ttt = ttt_create();
	get_field_str(str, ttt);
	ai_update_field(ai, str);
	get_m_field_str(str, m_field);
	ai_update_mboard(ai, str);
}
static void bot_first(void)
{
	point_t p;
	bot_init();
	ai_set_id(ai, 1);
	p = ai_move(ai, 10000);
	ttt_add(ttt, ID_P1, p);
	set_m_field(ttt, m_field, p);
	get_m_field_str(str, m_field);
	ai_update_mboard(ai, str);
	update_board(ttt, p);
}
static void bot_second(void)
{
	bot_init();
	ai_set_id(ai, 2);
	update_board(ttt, -1);
}
static void undo(void)
{
}
void update_board(struct ttt *ttt, point_t p)
{
	int i,pos,col;
	char field[9][9];
	redraw();
	ttt_get_field(ttt, field);
	for(pos=0;pos<81;pos++){
		if(field[pos/9][pos%9] == 1){ col = COL_RED;
		}else if(field[pos/9][pos%9] == 2){ col = COL_YELLOW;
		}else{ col = 0xFFFFFF; }
		draw_p(pos%9,pos/9,col);
	}
	for(i=0;i<9;i++){
		if(m_field[i] != 0){
			draw_m(i%3,i/3,0x0000FF);
		}
	}
	if(p != -1){
		draw_s(POINT_X(p), POINT_Y(p),0xFF00FF);
	}
}
void get_field_str(char *str, struct ttt *ttt)
{
	char field[9][9];
	int i, j;
	memset(str, 0, 1024);
	ttt_get_field(ttt, field);
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			str[2*(i*9+j)] = '0'+field[i][j];
			str[2*(i*9+j)+1] = ',';
		}
	}
	strcat(str, "\n");
}
void get_m_field_str(char *str, int *m_field)
{
	int i;
	memset(str, 0, 1024);
	for(i=0;i<8;i++){
		sprintf(str,"%s%d,", str, m_field[i]);
	}
	sprintf(str,"%s%d", str, m_field[i]);
	strcat(str, "\n");
}

void set_m_field(struct ttt *ttt, int *m_field, point_t p)
{
	int i;
	if(GB2(ttt->m_field, PO2OF(ttt,p)) == NORMAL){
		memset(m_field, 0, 9*sizeof(int));
		m_field[PO2OF(ttt,p)] = -1;
	}else{
		for(i=0;i<TTT_BLK_NUM;i++){
			if(GB2(ttt->m_field, i) == NORMAL){
				m_field[i] = -1;
			}else{
				m_field[i] = 0;
			}
		}
	}
}
