#include <stdio.h>
#include <stdlib.h>
#include <string.h>



#include "notif_value.h"
#include "attribute_data.h"





static int parse_notif_data(void* dest_data,void* src_data, int src_len)
{
	Resp_data * resp_data = dest_data;
	unsigned char* sdata = src_data;
	int offset = 0;
	int attid = 0;
	int att_len = 0;
	unsigned char * att_buf = NULL;

	DEBUG("%s, src_len = %d\n",__func__,src_len);
	if(src_len < CMD_ID_LENGTH + NOTIF_UID_LENGTH)
		return -1;

	resp_data->commandid = sdata[offset] & 0xff;
	offset += CMD_ID_LENGTH;


//	resp_data->notif_uid = ((sdata[offset + 3] & 0xff) << 24) |
//								((sdata[offset + 2] & 0xff) << 16) |
//								((sdata[offset + 1] & 0xff) << 8) |
//								(sdata[offset] & 0xff) ;
	
	memcpy(resp_data->notif_uid , sdata + offset, NOTIF_UID_LENGTH);
	offset += NOTIF_UID_LENGTH;

	while(offset + 1 < src_len){
	
		attid = sdata[offset];
		offset++;


		att_len = ((sdata[offset + 1] & 0xff) << 8) | (sdata[offset] & 0xff);
		offset += 2;	


		if(att_len < 0){
			return -1;
		}else if(att_len == 0){
			continue;
		}else{
			att_buf = calloc(1,att_len + 1);
		}

		if(att_buf == NULL)
			return -1;

		memcpy(att_buf,sdata + offset,att_len);
		att_buf[att_len] = '\0';

		switch(attid){
			case NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER:
				if(resp_data->app_identifier != NULL)
					free(resp_data->app_identifier);
				resp_data->app_identifier = att_buf;	
				break;

			case NOTIFICATION_ATTRIBUITE_ID_TITLE:
				if(resp_data->title != NULL)
					free(resp_data->title);
				resp_data->title = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_SUBTITLE:
				if(resp_data->subtitle != NULL)
					free(resp_data->subtitle);
				resp_data->subtitle = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_MESSAGE:
				if(resp_data->message != NULL)
					free(resp_data->message);
				resp_data->message = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_MESSAGESIZE:
				if(resp_data->message_size)
					free(resp_data->message_size);
				resp_data->message_size = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_DATE:
				if(resp_data->date != NULL)
					free(resp_data->date);
				resp_data->date = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_POSITIVEACTIONLABLE:
				if(resp_data->positive_actionlabel != NULL)
					free(resp_data->positive_actionlabel);
				resp_data->positive_actionlabel = att_buf;
				break;

			case NOTIFICATION_ATTRIBUITE_ID_NEGATIVEACTIONLABLE:
				if(resp_data->negative_actionlabel)
					free(resp_data->negative_actionlabel);
				resp_data->negative_actionlabel = att_buf;
				break;

			default:
				break;
		}

		att_buf = NULL;
		offset += att_len;

	}
	

	return 0;
}

static int parse_app_data(void* dest_data,void* src_data,int src_len)
{
	return 0;
}




int parse_character(void* dest_data,  void* src_data)
{
	unsigned char* sdata = src_data;
	struct character_data * char_data = dest_data;

	if(char_data == NULL){
		DEBUG("%s :char_data is NULL\n",__func__);
		return -1;
	}

	char_data->event_id = sdata[0];
	char_data->event_flags = sdata[1];
	char_data->category_id = sdata[2];
	char_data->category_cnt = sdata[3];
	memcpy(&char_data->notif_uid, sdata + 4,  NOTIF_UID_LENGTH);
	return 0;

}

int  parse_data(void* dest_data,void* src_data,int src_len)
{
	int ret = 0;
	unsigned char* sdata = src_data;

	if(NULL == src_data || NULL == dest_data)
		return -1;
	
	if ((sdata[0] & 0xff) == COMMANDID_GETNOTIFICATION_ATTRIBUITES){
		ret = parse_notif_data(dest_data,sdata,src_len);
	}else if((sdata[0] & 0xff) == COMMANDID_GETNOTIFICATION_ATTRIBUITES){
		ret = parse_app_data(dest_data,sdata,src_len);
	}
	
	
	return ret;
}



void free_attribute_data(Resp_data* data)
{
	if(data == NULL)
		return;

	if(data->app_id_str != NULL)
		free(data->app_id_str);
	
	if(data->app_identifier != NULL)
		free(data->app_identifier);

	if(data->title != NULL)
		free(data->title);

	if(data->subtitle != NULL)
		free(data->subtitle);

	if(data->message != NULL)
		free(data->message);

	if(data->message_size != NULL)
		free(data->message_size);

	if(data->date != NULL)
		free(data->date);

	if(data->positive_actionlabel != NULL)
		free(data->positive_actionlabel);

	if(data->negative_actionlabel != NULL)
		free(data->negative_actionlabel);
	

	return;
}
