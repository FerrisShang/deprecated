#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "main.h"

int max_score(struct data *data, int id, int steps, int *value, int *idx);

static int timediff_ms(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec-start->tv_sec)*1000+(end->tv_usec-start->tv_usec)/1000;
}
#define STEP_LEN 32
#define TOTAL_TIME_MS 1250
int cal_col(struct data *data)
{
	int step, idx[STEP_LEN+1] = {0};
	int score[STEP_LEN+1] = {0};
	struct timeval t_start, t_cal, t_end;
	gettimeofday(&t_start, NULL);
	for(step=8;step<STEP_LEN;step++){
		gettimeofday(&t_cal, NULL);
		memset(idx, -1, STEP_LEN+1);
		max_score(data, data->your_botid, step, score, idx);
		gettimeofday(&t_end, NULL);
		if(idx[step] > GAME_MAX || idx[step] < -GAME_MAX ||
				timediff_ms(&t_cal, &t_end) * 8 >
				TOTAL_TIME_MS-timediff_ms(&t_start, &t_end)){
			break;
		}
	}
	if(step == STEP_LEN){
		step--;
	}
#ifdef DEBUG
	fprintf(stderr, "idx=%d,step = %d,rmt=%d, calt=%d\n", idx[step], step,
			TOTAL_TIME_MS-timediff_ms(&t_start, &t_end),
			timediff_ms(&t_cal, &t_end));
	fprintf(stderr, "\nscore = %8d %8d %8d %8d %8d %8d %8d %8d\n",
			-score[0], -score[1], -score[2], -score[3],
			-score[4], -score[5], -score[6], -score[7]);
	fprintf(stderr, "steps = %d %d %d %d %d %d %d %d \n",
			idx[0], idx[1], idx[2], idx[3],
			idx[4], idx[5], idx[6], idx[7]);
	fprintf(stderr, "disp = %d\n", idx[step]);
	fflush(stderr);
#endif
	return idx[step];
}

int max_score(struct data *data, int id, int steps, int *value, int *idx)
{
	int i, j;
	struct {
		int col;
		int value;
		int state;//1:win 0:normal -1:col_full
	} pos[FIELD_COL], tmp;
	for(i=0;i<data->field_columns;i++){
		pos[i].col = i;
		pos[i].state = -1;
		pos[i].value = -VALUE_INF;
	}
	for(i=0;i<data->field_columns;i++){
		if(isColFull(data, i)){
			pos[i].state = -1;
			continue;
		}
		add_field(data, id, i);
		if(isFinish(data, id, i)){
			idx[steps] = i;
			value[steps] = VALUE_INF;
			value[steps] = - value[steps];
			remove_field(data, id, i);
			return 1;
		}else{
			if(steps == 1){
				pos[i].value = judgement(data, id, i);
				pos[i].state = 0;
			}else{
				pos[i].state = max_score(data, op_id(id), steps-1, value, idx);
				pos[i].value = value[steps-1];
			}
			remove_field(data, id, i);
		}
	}
	//select best col
	for(i=0;i<data->field_columns-1;i++){
		for(j=data->field_columns-1;j>i;j--){
			if(pos[j].value>pos[j-1].value ||
					(pos[j].value==pos[j-1].value && (rand()&1))
			  ){
				tmp = pos[j];
				pos[j] = pos[j-1];
				pos[j-1] = tmp;
			}
		}
	}
	for(i=0;i<data->field_columns;i++){
		if(pos[i].state >= 0){
			value[steps] = pos[i].value;
			value[steps] = -value[steps];
			idx[steps] = pos[i].col;
			return 0;
		}
	}
	return 0;//must be draw
}
