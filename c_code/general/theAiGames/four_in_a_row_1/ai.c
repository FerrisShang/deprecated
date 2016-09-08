#include <stdlib.h>
#include <sys/time.h>
#include "ai.h"
#include "ai_core.h"

struct ai_four* ai_create(void)
{
	struct ai_four *ai;
	ai = malloc(sizeof(struct ai_four));
	if(!ai){
		return NULL;
	}
	ai->four = four_create();
	if(!ai->four){
		free(ai);
		return NULL;
	}
	return ai;
}
void ai_reset(struct ai_four *ai, int timebank, int time_per_move)
{
	if(ai){
		ai->timebank = timebank;
		ai->time_per_move = time_per_move;
	}
}
void ai_destory(struct ai_four *ai)
{
	if(ai && ai->four){
		four_destory(ai->four);
	}
	if(ai){
		free(ai);
	}
}
void ai_set_id(struct ai_four *ai, int id)
{
	if(ai){
		ai->id = id;
	}
}
void ai_update_field(struct ai_four *ai, char *field)
{
	char f[FOUR_ROW][FOUR_COL];
	int i, j, id, idx;
	if(ai && ai->four){
		for(j=0;j<FOUR_ROW;j++){
			for(i=0;i<FOUR_COL;i++){
				idx = 2 * (FOUR_COL*j + i);
				id = field[idx] - '0';
				f[FOUR_ROW-j-1][i] = id;
			}
		}
		four_set_field(ai->four, f);
	}
}
char* ai_get_field(struct ai_four *ai)
{
	static char field[2*FOUR_ROW*FOUR_COL+1];
	char f[FOUR_ROW][FOUR_COL];
	int i,j;
	four_get_field(ai->four, f);
	for(j=0;j<FOUR_ROW;j++){
		for(i=0;i<FOUR_COL;i++){
			int idx = i+j*FOUR_COL;
			field[idx*2] = f[FOUR_ROW-j-1][i] + '0';
			if(i==FOUR_COL-1){
				field[idx*2+1] = ';';
			}else{
				field[idx*2+1] = ',';
			}
		}
	}
	return field;
}
int ai_move(struct ai_four *ai, int time_limit_ms)
{
	int col;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	col = cal_col(ai, time_limit_ms);
	gettimeofday(&end, NULL);
	ai->timebank -= (end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
	ai->timebank += ai->time_per_move;
	if(ai->timebank < 0){
		dbg_printf("timeout %d\n", ai->timebank);
	}
	return col;
}
