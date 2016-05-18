#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

#define VALUE_INF 100000000
#define GAME_MAX  1000000

void max_score(struct data *data, int id, int steps, int *value, int *idx);

#define STEP_LEN 8
int cal_col(struct data *data)
{
	int i, idx[STEP_LEN+1] = {0};
	int score[STEP_LEN+1] = {0};
	max_score(data, data->your_botid, STEP_LEN, score, idx);
#if DEBUG == 1
	printf("\nscore = %8d %8d %8d %8d %8d %8d %8d %8d\n",
			-score[0], -score[1], -score[2], -score[3],
			-score[4], -score[5], -score[6], -score[7]);
	printf("steps = %d %d %d %d %d %d %d %d \n",
			idx[0], idx[1], idx[2], idx[3],
			idx[4], idx[5], idx[6], idx[7]);
	printf("disp = %d\n", idx[STEP_LEN]);
#endif
	return idx[STEP_LEN];
}

void max_score(struct data *data, int id, int steps, int *value, int *idx)
{
	int i;
	if(steps == 0){
		value[steps] = (rand() & 0x3F);
		return;
	}
	value[steps] = -VALUE_INF;
	for(i=0;i<data->field_columns;i++){
		if(isColFull(data, i)) continue;
		add_field(data, id, i);
		if(isFinish(data, id, i)){
			idx[steps] = i;
			value[steps] = GAME_MAX;
			value[steps] = - value[steps];
			remove_field(data, id, i);
			return;
		}else{
			max_score(data, op_id(id), steps-1, value, idx);
			if(value[steps] < value[steps-1]){
				value[steps] = value[steps-1];
				idx[steps] = i;
			}
			remove_field(data, id, i);
		}
#if DEBUG == 1
		if(steps == STEP_LEN){
			printf("col%d=%d ", i, value[steps-1]);
		}
#endif
	}
	value[steps] = - value[steps];
	return;
}
