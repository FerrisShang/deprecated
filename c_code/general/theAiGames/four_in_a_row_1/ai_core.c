#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "ai_core.h"

#define MAX_STEP        42
#define MIN_STEP        8
#define TOTAL_TIME_MS   2500
#define HASH_BIT_SIZE   26
#define MIN_HASH_DEEP   4
#define STATIC_HASH_MEM 1

enum {
	ERROR = 0,
	TIED = 1,
	NORMAL_MIM = 16,
	NORMAL_MAX = NORMAL_MIM + 0x40,
#define rand_normal() (NORMAL_MIM+(rand()&(0x40-1)))
	WIN = 127,
};

static int max_score(struct ai_four *ai, int id, int steps, int *value, int *idx);
static int judgement(struct ai_four *ai, int id, int col);
int get_hash(char *h, struct ai_four *ai, int step, int *score);
int set_hash(char *h, struct ai_four *ai, int step, int *score);

static int timediff_ms(struct timeval *start, struct timeval *end);
#if STATIC_HASH_MEM == 1
char h[1ull<<HASH_BIT_SIZE];
#else
char *h;
#endif

int cal_col(struct ai_four *ai, int time_limit_ms)
{
	int step, idx[MAX_STEP+1] = {0};
	int score[MAX_STEP+1] = {0};
	int rec_idx[MAX_STEP+1] = {0};
	int used_time;
	struct timeval t_cal, t_end;
	for(step=MIN_STEP;step<MAX_STEP;step++){
		gettimeofday(&t_cal, NULL);
		memset(idx, -1, MAX_STEP+1);
#if STATIC_HASH_MEM == 1
		memset(h, 0, sizeof(h));
		max_score(ai, ai->id, step, score, idx);
		rec_idx[step] = idx[step];
#else
		h = calloc(1, 1ull<<HASH_BIT_SIZE);
		if(!h){
			dbg_printf("calloc memory failed\n");
		}
		max_score(ai, ai->id, step, score, idx);
		rec_idx[step] = idx[step];
		free(h);
#endif
		gettimeofday(&t_end, NULL);
		used_time = timediff_ms(&t_cal, &t_end);
		if(score[step] == WIN || score[step] == -WIN ||
				score[step] == TIED || score[step] == -TIED ||
				used_time*8 > TOTAL_TIME_MS-used_time){
			break;
		}
	}
	if(step == MAX_STEP){
		step--;
	}
	dbg_printf("step =%2d colume = %d value =%4d\n", step, idx[step], -score[step]);
	if(score[step] == WIN || score[step] == -WIN){//for debug
		int i;
		for(i=0;i<step;i++){
			dbg_printf("%d ", idx[step-i-1]);
		}
		dbg_printf("\n");
	}
	if(-score[step] == -WIN && step != MIN_STEP){
		return rec_idx[step-1];
	}else{
		return rec_idx[step];
	}
}

static int max_score(struct ai_four *ai, int id, int steps, int *value, int *idx)
{
	int i, j;
	struct {
		int col;
		int value;
	} pos[FOUR_COL], tmp;
	for(i=0;i<FOUR_COL;i++){
		pos[i].col = i;
		pos[i].value = ERROR;
	}
	for(i=0;i<FOUR_COL;i++){
		if(four_add(ai->four, id, i) < 0){
			continue;
		}
		if(steps <= MIN_HASH_DEEP || get_hash(h, ai, steps, &pos[i].value) < 0){
			if(four_isFinish(ai->four, id, i)){
				pos[i].value = WIN;
			}else{
				if(steps == 1){
					pos[i].value = judgement(ai, id, i);
				}else{
					pos[i].value = max_score(ai, four_op_id(id), steps-1, value, idx);
				}
			}
			if(steps > MIN_HASH_DEEP){
				set_hash(h, ai, steps, &pos[i].value);
			}
		}
		four_remove(ai->four, id, i);
		if(pos[i].value == WIN){
			break;
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
		if(pos[i].value != 0){
			value[steps] = pos[i].value;
			value[steps] = -value[steps];
			idx[steps] = pos[i].col;
			return value[steps];
		}
	}
	//must be tied
	value[steps] = TIED;
	value[steps] = -value[steps];
	return value[steps];
}
static int timediff_ms(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec-start->tv_sec)*1000+(end->tv_usec-start->tv_usec)/1000;
}

uint32_t hash(void *key, size_t len)
{
	uint32_t hash, i;
	for(hash = i = 0; i < len; ++i)
	{
		hash += ((char*)key)[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

uint32_t get_hash_idx(struct ai_four *ai, int step)
{
	uint32_t h_idx, i;
	char buf[13];
	unsigned short *f1 = ai->four->field_h[1];
	unsigned short *f2 = ai->four->field_h[2];
	for(i=0;i<6;i++){
		buf[i] = (char)f1[i];
		buf[i+6] = (char)f2[i];
	}
	buf[12] = step;
	h_idx = hash(buf, 13);
	return h_idx & ((1ull<<HASH_BIT_SIZE)-1);
}
int get_hash(char *h, struct ai_four *ai, int step, int *score)
{
	uint32_t h_idx;
	char value;
	h_idx = get_hash_idx(ai, step);
	value = h[h_idx];
	if(value == 0){
		return -1;
	}else{
		*score = value;
	}
	return 0;
}

int set_hash(char *h, struct ai_four *ai, int step, int *score)
{
	uint32_t h_idx;
	h_idx = get_hash_idx(ai, step);
#if 1
	if(h[h_idx]!=0){
		dbg_printf("hash replaced.\n");
	}
#endif
	h[h_idx] = *score;
	return 0;
}

static int judgement(struct ai_four *ai, int id, int col)
{
	return rand_normal();
}

