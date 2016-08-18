#ifndef __AI_H__
#define __AI_H__

#include "four.h"

#define DEFAULT_TIMEBANK      10000
#define DEFAULT_TIME_PER_MOVE 500

struct ai_four {
	int timebank;
	int time_per_move;
	int id;
	struct four* four;
};

struct ai_four* ai_create(void);
void ai_destory(struct ai_four *ai);
void ai_set_id(struct ai_four *ai, int id);
void ai_update_field(struct ai_four *ai, char *field);
int  ai_move(struct ai_four *ai, int time_limit_ms);
void ai_reset(struct ai_four *ai, int timebank, int time_per_move);

#endif /* __AI_H__ */
