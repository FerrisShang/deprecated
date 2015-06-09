#include <string.h>
#include "base.h"
#include "protocol.h"

static void pack_valid_data(void);
static void parse_valid_data(void);
static s32_t pack_comm_data(void);
static s32_t parse_comm_data(void);

static void pack_valid_data(void)
{
}

static void parse_valid_data(void)
{
	dbg_print(DBG_INFO, PRO_DBG, "%s@%d\n", __func__, __LINE__);
}

static s32_t pack_comm_data(void)
{
	return 0;
}

static s32_t parse_comm_data(void)
{
	return 0;
}

sProtocol_t* protocol_create(void)
{
	sProtocol_t* pro;
	pro = (sProtocol_t*)mem_malloc(sizeof(sProtocol_t));
	if(pro == NULL){
		dbg_print(DBG_ERROR, PRO_DBG, "not enough memory.\n");
		return pro;
	}
	memset(pro, 0,sizeof(sProtocol_t));
	return pro;
}

void protocol_destory(sProtocol_t *pro)
{
	if(pro == NULL){
		dbg_print(DBG_WARNING, PRO_DBG, "attempt to free zero pointer.\n");
	}else{
		mem_free(pro);
	}
}

s32_t proto_package_data(sProtocol_t *pro, sPro_udata_t *usr_data, s8_t *buf)
{
	pack_valid_data();
	return 0;
}

s32_t proto_parse_data(sProtocol_t *pro, sPro_udata_t *usr_data, s8_t *buf)
{
	return 0;
}
