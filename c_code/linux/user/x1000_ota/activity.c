#include "base.h"
#include "activity.h"
#include "ota_para.h"

s32_t activity_device(sProtocol_t *pro, sSocket_t *sock)
{
	s8_t state;
	if(0 != read_activity_state(&state)){
		return ACTIVITY_ERROR; //read activity state failed
	}
	if(state == ACTIVITY_STATE_NOT_ACT){
		;
	}
	return ACTIVITY_SUCCESS;
}
