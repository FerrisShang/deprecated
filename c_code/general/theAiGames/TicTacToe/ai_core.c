#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "ai_core.h"

#define MIN_STEP        1
#define MAX_STEP        42
#define TOTAL_TIME_MS   2500
#define HASH_BIT_SIZE   26
#define MIN_HASH_DEEP   4

enum {
	FULL = 0,
	TIED = 1,
	NORMAL_MIM = 16,
	NORMAL_MAX = NORMAL_MIM + 0x40-1,
#define rand_normal() (NORMAL_MIM+(rand()&(0x40-1)))
	WIN = 127,
};

struct position{
	point_t point;
	point_t next_p;
	int value;
};

static struct position max_score(struct ai_ttt *ai, int id, int steps);
static void dbg_steps(struct ai_ttt *ai, int step, struct position *last_pos);
static int get_hash(char *h, struct ai_ttt *ai, int step, int *score);
static int set_hash(char *h, struct ai_ttt *ai, int step, int *score);
static int timediff_ms(struct timeval *start, struct timeval *end);
char *h;

point_t cal_point(struct ai_ttt *ai, int time_limit_ms)
{
	int step, used_time;
	struct timeval t_cal, t_end;
	struct position cur_pos = {0}, last_pos;
	for(step=MIN_STEP;step<MAX_STEP;step++){
		gettimeofday(&t_cal, NULL);
		last_pos = cur_pos;
		h = calloc(1, 1ull<<HASH_BIT_SIZE);
		if(!h){
			dbg_printf("calloc memory failed\n");
			exit(0);
		}
		cur_pos = max_score(ai, ai->id, step);
		free(h);
		gettimeofday(&t_end, NULL);
		used_time = timediff_ms(&t_cal, &t_end);
		if(cur_pos.value == WIN || cur_pos.value == -WIN ||
				cur_pos.value == TIED || cur_pos.value == -TIED ||
				used_time*8 > TOTAL_TIME_MS-used_time){
			break;
		}
	}
	//select step
	if(-cur_pos.value == -WIN && step != MIN_STEP){
		dbg_printf("step =%2d point = (%d,%d) value =%4d\n",
			step, GET_P_X(last_pos.point), GET_P_Y(last_pos.point), -last_pos.value);
		dbg_steps(ai, step, &last_pos);
		return last_pos.point;
	}else{
		dbg_printf("step =%2d point = (%d,%d) value =%4d\n",
			step, GET_P_X(cur_pos.point), GET_P_Y(cur_pos.point), -cur_pos.value);
		if(-cur_pos.value == WIN){
			dbg_steps(ai, step, &cur_pos);
		}
		return cur_pos.point;
	}
}

static struct position max_score(struct ai_ttt *ai, int id, int steps)
{
	struct position p = {0};
	return p;
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

uint32_t get_hash_idx(struct ai_ttt *ai, int step)
{
	return 0;
}
static int get_hash(char *h, struct ai_ttt *ai, int step, int *score)
{
	return 0;
}

static int set_hash(char *h, struct ai_ttt *ai, int step, int *score)
{
	return 0;
}

static void dbg_steps(struct ai_ttt *ai, int step, struct position *pos)
{
	return;
}
