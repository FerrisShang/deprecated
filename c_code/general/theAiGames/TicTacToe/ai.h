#ifndef __AI_H__
#define __AI_H__

#include "ttt.h"

#define DEFAULT_TIMEBANK      10000
#define DEFAULT_TIME_PER_MOVE 500

struct ai_ttt {
	int timebank;
	int time_per_move;
	int id;
	struct ttt* ttt;
};

struct ai_ttt* ai_create(void);
void ai_destory(struct ai_ttt *ai);
void ai_set_id(struct ai_ttt *ai, int id);
void ai_update_field(struct ai_ttt *ai, char *field);
char* ai_get_field(struct ai_ttt *ai);
point_t ai_move(struct ai_ttt *ai, int time_limit_ms);
void ai_reset(struct ai_ttt *ai, int timebank, int time_per_move);

#endif /* __AI_H__ */
