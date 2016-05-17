#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

#define VALUE_INF 1e100
#define GAME_MAX  1e10

#if 0
void cal8(struct data *data, int col, float *score)
{
	int i;
	int op_id = 3-data->your_botid;
	if(data->field[0][col] != 0) return;
	update_field(data, op_id, col);
	if(isFinish(data, col)){
		*score -= 128;
	}else{
		//dump_field(data);
	}
	update_field(data, -1, col);
}
void cal7(struct data *data, int col, float *score)
{
	int i;
	if(data->field[0][col] != 0) return;
	update_field(data, data->your_botid, col);
	if(isFinish(data, col)){
		*score += 128;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal8(data, i, score);
		}
	}
	update_field(data, -1, col);
}
void cal6(struct data *data, int col, float *score)
{
	int i;
	int op_id = 3-data->your_botid;
	if(data->field[0][col] != 0) return;
	update_field(data, op_id, col);
	if(isFinish(data, col)){
		*score -= 256;
	}else{
		//dump_field(data);
		/*
		for(i=0;i<data->field_columns;i++){
			cal7(data, i, score);
		}
		*/
	}
	update_field(data, -1, col);
}
void cal5(struct data *data, int col, float *score)
{
	int i;
	if(data->field[0][col] != 0) return;
	update_field(data, data->your_botid, col);
	if(isFinish(data, col)){
		*score += 256;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal6(data, i, score);
		}
	}
	update_field(data, -1, col);
}

void cal4(struct data *data, int col, float *score)
{
	int i;
	int op_id = 3-data->your_botid;
	if(data->field[0][col] != 0) return;
	update_field(data, op_id, col);
	if(isFinish(data, col)){
		*score -= 512;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal5(data, i, score);
		}
	}
	update_field(data, -1, col);
}
void cal3(struct data *data, int col, float *score)
{
	int i;
	if(data->field[0][col] != 0) return;
	update_field(data, data->your_botid, col);
	if(isFinish(data, col)){
		*score += 512;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal4(data, i, score);
		}
	}
	update_field(data, -1, col);
}
void cal2(struct data *data, int col, float *score)
{
	int i;
	int op_id = 3-data->your_botid;
	if(data->field[0][col] != 0) {return;}
	update_field(data, op_id, col);
	if(isFinish(data, col)){
		*score -= 1e9;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal3(data, i, score);
		}
	}
	update_field(data, -1, col);
} 
void cal1(struct data *data, int col, float *score)
{
	int i;
	if(data->field[0][col] != 0){
		score[col] = -1e100;
		return;
	}
	update_field(data, data->your_botid, col);
	if(isFinish(data, col)){
		score[col] += 1e9;
	}else{
		for(i=0;i<data->field_columns;i++){
			cal2(data, i, &score[col]);
		}
	}
	update_field(data, -1, col);
}
#endif

float max_score(struct data *data, int id, int steps);
float min_score(struct data *data, int id, int steps);

int cal_col(struct data *data)
{
	int i, max_idx = 0;
	float max = -VALUE_INF, score[7];
	for(i=0;i<data->field_columns;i++){
		score[i] = max_score(data, data->your_botid, 6);
	}
	for(i=0;i<data->field_columns;i++){
		printf("%f, ", score[i]);
		if(max < score[i]){
			max = score[i];
			max_idx = i;
		}
	}
	printf("\n");
	return max_idx;
}

#define op_id(id) (3-id)
float max_score(struct data *data, int id, int steps)
{
	int i;
	float tmp,max_value = -VALUE_INF;
	if(steps == 0) return 0;
	for(i=0;i<data->field_columns;i++){
		if(data->field[0][i] != 0) continue;
		update_field(data, id, i);
		if(isFinish(data, i)){
			update_field(data, -1, i);
			return VALUE_INF;
		}else{
			tmp = min_score(data, op_id(id), steps-1) + (rand() & 0x0F);
			if(tmp > max_value){
				max_value = tmp;
			}
			update_field(data, -1, i);
		}
	}
	return max_value;
}

float min_score(struct data *data, int id, int steps)
{
	int i;
	float tmp,min_value = +VALUE_INF;
	if(steps == 0) return 0;
	for(i=0;i<data->field_columns;i++){
		if(data->field[0][i] != 0) continue;
		update_field(data, id, i);
		if(isFinish(data, i)){
			update_field(data, -1, i);
			return -VALUE_INF;
		}else{
			tmp = max_score(data, op_id(id), steps-1) + (rand() & 0x0F);
			if(tmp < min_value){
				min_value = tmp;
			}
			update_field(data, -1, i);
		}
	}
	return min_value;
}
