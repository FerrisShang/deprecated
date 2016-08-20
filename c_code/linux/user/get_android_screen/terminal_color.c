#include <stdio.h>

int col_256to6_map[1<<8];
static int col_map[6][6][6];

void init_col(void)
{
#define COL_OFFSET 16
	int i, j, k;
	for(i=0;i<6;i++){
		for(j=0;j<6;j++){
			for(k=0;k<6;k++){
				col_map[i][j][k] = COL_OFFSET + i*36 + j*6 + k; 
			}
		}
	}
	for(i=0;i<1<<8;i++){
		if(i<0x5F){
			col_256to6_map[i] = 0;
		}else if(i<0x87){
			col_256to6_map[i] = 1;
		}else if(i<0xAF){
			col_256to6_map[i] = 2;
		}else if(i<0xD7){
			col_256to6_map[i] = 3;
		}else if(i<0xFF){
			col_256to6_map[i] = 4;
		}else{
			col_256to6_map[i] = 5;
		}
	}
}
void set_color(int r, int g, int b)
{
	int color = col_map[r][g][b];
	printf("\x1b[48;5;%dm", color);
}
void set_color_8bit(unsigned char r, unsigned char g, unsigned char b)
{
	set_color(col_256to6_map[r], col_256to6_map[g], col_256to6_map[b]);
}


