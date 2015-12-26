#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "attribute_data.h"

int print_character(void *src_data)
{
	struct character_data char_data;

	if(src_data == NULL){
		DEBUG("%s: src_data is NULL\n",__func__);
		return -1;
	}

	parse_character(&char_data, src_data);
	DEBUG("=============================\n");
	DEBUG("event_id = %x\n",char_data.event_id);
	DEBUG("event_flags = %x\n",char_data.event_flags);
	DEBUG("category_id = %x\n",char_data.category_id);
	DEBUG("category_cnt = %d\n",char_data.category_cnt);
	DEBUG("notif_uid = %x-%x-%x-%x\n", char_data.notif_uid[0], 
					char_data.notif_uid[1],char_data.notif_uid[2],char_data.notif_uid[3]);
	DEBUG("=============================\n");
}


static unsigned char notif_req_buf[NOTIF_REQ_LENGTH];
int notif_req_assembling(void** dest_req, int* req_len , void* src_data, int flag)
{
	int rlen = CMD_ID_LENGTH + NOTIF_UID_LENGTH; 

	DEBUG("%s, %d\n",__func__,__LINE__);
	if(src_data == NULL){
		DEBUG("%s: src_data is NULL\n",__func__);
		return -1;
	}

	memset(notif_req_buf, 0, NOTIF_REQ_LENGTH);

	DEBUG("%s, %d\n",__func__,__LINE__);
	notif_req_buf[0] = COMMANDID_GETNOTIFICATION_ATTRIBUITES; 
	memcpy(notif_req_buf + 1, src_data + 4, NOTIF_UID_LENGTH);

	if(flag & FLAG_APPID){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER;
		notif_req_buf[rlen + 1] = APPID_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}

	DEBUG("%s, %d\n",__func__,__LINE__);
	if(flag & FLAG_TITLE){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_TITLE;
		notif_req_buf[rlen + 1] = TITLE_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}
	if(flag & FLAG_SUBTITLE){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_SUBTITLE;
		notif_req_buf[rlen + 1] = SUBTITLE_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}

	DEBUG("%s, %d\n",__func__,__LINE__);
	if(flag & FLAG_MESSAGE){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_MESSAGE;
		notif_req_buf[rlen + 1] = MSG_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}
	if(flag & FLAG_MESGSIZE){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_MESSAGESIZE;
		notif_req_buf[rlen + 1] = MSG_SIZE_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}
	if(flag & FLAG_DATE){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_DATE;
		notif_req_buf[rlen + 1] = DATE_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}
	if(flag & FLAG_POS_ACT_LAB){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_POSITIVEACTIONLABLE;
		notif_req_buf[rlen + 1] = POS_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}
	DEBUG("%s, %d\n",__func__,__LINE__);
	if(flag & FLAG_NEG_ACT_LAB){
		notif_req_buf[rlen] = NOTIFICATION_ATTRIBUITE_ID_NEGATIVEACTIONLABLE;
		notif_req_buf[rlen + 1] = NEG_CAP;
		notif_req_buf[rlen + 2] = 0;

		rlen += ATTR_UNIT_LENGTH;
	}


	DEBUG("%s, %d\n",__func__,__LINE__);
	*dest_req = notif_req_buf;
	*req_len = rlen;

	return 0;
}


int app_req_assembling(void* dest_req, int* req_len , void* src_data, unsigned char* app_id_str, int flag)
{
	return 0;
}




