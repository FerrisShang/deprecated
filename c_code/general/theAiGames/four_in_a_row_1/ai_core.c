#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "ai_core.h"

#define VALUE_INF     100000000
#define GAME_MAX      1000000
#define STEP_MINIUS   10000

#define MAX_STEP      42
#define TOTAL_TIME_MS 2000

static int max_score(struct ai_four *ai, int id, int steps, int *value, int *idx);
static int judgement(struct ai_four *ai, int id, int col);
static int timediff_ms(struct timeval *start, struct timeval *end);

int cal_col(struct ai_four *ai, int time_limit_ms)
{
	int step, idx[MAX_STEP+1] = {0};
	int score[MAX_STEP+1] = {0};
	int used_time;
	struct timeval t_cal, t_end;
	for(step=8;step<MAX_STEP;step++){
		gettimeofday(&t_cal, NULL);
		memset(idx, -1, MAX_STEP+1);
		max_score(ai, ai->id, step, score, idx);
		gettimeofday(&t_end, NULL);
		used_time = timediff_ms(&t_cal, &t_end);
		if(idx[step] > GAME_MAX || idx[step] < -GAME_MAX ||
				used_time*8 > TOTAL_TIME_MS-used_time){
			break;
		}
	}
	if(step == MAX_STEP){
		step--;
	}
	return idx[step];
}

static int max_score(struct ai_four *ai, int id, int steps, int *value, int *idx)
{
	int i, j;
	struct {
		int col;
		int value;
		int state;//1:win 0:normal -1:col_full
	} pos[FOUR_COL], tmp;
	for(i=0;i<FOUR_COL;i++){
		pos[i].col = i;
		pos[i].state = -1;
		pos[i].value = -VALUE_INF;
	}
	for(i=0;i<FOUR_COL;i++){
		if(four_add(ai->four, id, i) < 0){
			pos[i].state = -1;
			continue;
		}
		if(four_isFinish(ai->four, id, i)){
			idx[steps] = i;
			value[steps] = VALUE_INF;
			value[steps] = - value[steps];
			four_remove(ai->four, id, i);
			return 1;
		}else{
			if(steps == 1){
				pos[i].value = judgement(ai, id, i);
				pos[i].state = 0;
			}else{
				pos[i].state = max_score(ai, four_op_id(id), steps-1, value, idx);
				pos[i].value = value[steps-1];
			}
			four_remove(ai->four, id, i);
		}
	}
	//select best col
	for(i=0;i<FOUR_COL-1;i++){
		for(j=FOUR_COL-1;j>i;j--){
			if(pos[j].value>pos[j-1].value ||
					(pos[j].value==pos[j-1].value && (rand()&1))
			  ){
				tmp = pos[j];
				pos[j] = pos[j-1];
				pos[j-1] = tmp;
			}
		}
	}
	for(i=0;i<FOUR_COL;i++){
		if(pos[i].state >= 0){
			value[steps] = pos[i].value;
			value[steps] = -value[steps];
			idx[steps] = pos[i].col;
			return 0;
		}
	}
	return 0;//must be tied
}
static int timediff_ms(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec-start->tv_sec)*1000+(end->tv_usec-start->tv_usec)/1000;
}
static int judgement(struct ai_four *ai, int id, int col)
{
	return (rand()&0xFFFF);
}

