#include <stdlib.h>
#include <sys/time.h>
#include "ai.h"
#include "ai_core.h"

struct ai_ttt* ai_create(void)
{
	struct ai_ttt *ai;
	ai = malloc(sizeof(struct ai_ttt));
	if(!ai){
		return NULL;
	}
	ai->ttt = ttt_create();
	if(!ai->ttt){
		free(ai);
		return NULL;
	}
	return ai;
}
void ai_reset(struct ai_ttt *ai, int timebank, int time_per_move)
{
	if(ai){
		ai->timebank = timebank;
		ai->time_per_move = time_per_move;
	}
}
void ai_destory(struct ai_ttt *ai)
{
	if(ai && ai->ttt){
		ttt_destory(ai->ttt);
	}
	if(ai){
		free(ai);
	}
}
void ai_set_id(struct ai_ttt *ai, int id)
{
	if(ai){
		ai->id = id;
	}
}
void ai_update_field(struct ai_ttt *ai, char *field)
{
	char f[9][9];
	int i, j, id, idx;
	if(ai && ai->ttt){
		for(j=0;j<9;j++){
			for(i=0;i<9;i++){
				idx = 2 * (9*j + i);
				id = field[idx] - '0';
				f[j][i] = id;
			}
		}
		ttt_set_field(ai->ttt, f);
	}
}
char* ai_get_field(struct ai_ttt *ai)
{
	static char field[2*9*9+1];
	char f[9][9];
	int i,j;
	ttt_get_field(ai->ttt, f);
	for(j=0;j<9;j++){
		for(i=0;i<9;i++){
			int idx = i+j*9;
			field[idx*2] = f[j][i] + '0';
			if(i==9-1){
				field[idx*2+1] = ';';
			}else{
				field[idx*2+1] = ',';
			}
		}
	}
	return field;
}
point_t ai_move(struct ai_ttt *ai, int time_limit_ms)
{
	int col;
	struct timeval start, end;
	gettimeofday(&start, NULL);
	col = cal_point(ai, time_limit_ms);
	gettimeofday(&end, NULL);
	ai->timebank -= (end.tv_sec-start.tv_sec)*1000+(end.tv_usec-start.tv_usec)/1000;
	ai->timebank += ai->time_per_move;
	if(ai->timebank < 0){
		dbg_printf("timeout %d\n", ai->timebank);
	}
	return col;
}
