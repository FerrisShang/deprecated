#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ttt.h"

struct ttt* ttt_create(void)
{
	struct ttt *ttt;
	ttt = malloc(sizeof(struct ttt));
	if(!ttt){
		return NULL;
	}
	memset(ttt, 0, sizeof(struct ttt));
	return ttt;
}

void ttt_destory(struct ttt *ttt)
{
	free(ttt);
}

void ttt_reset(struct ttt *ttt)
{
	return;
}

int ttt_add(struct ttt *ttt, int id, point_t p)
{
	return 0;
}

int ttt_remove(struct ttt *ttt, int id, point_t p)
{
	return 0;
}

int ttt_isFinish(struct ttt *ttt, int id, point_t last_point)
{
	return 0;
}

int ttt_set_field(struct ttt *ttt, char field[][9])
{
	ttt_reset(ttt);
	return 0;
}
int ttt_get_field(struct ttt *ttt, char field[][9])
{
	return 0;
}
