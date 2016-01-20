#include <stdio.h>
#include <string.h>
#include "AncsParser.h"
#include "Parser_utils.h"

/*CommandID Values*/
enum {
	COMMANDID_GETNOTIFICATION_ATTRIBUITES = 0,
	COMMANDID_GETAPP_ATTRIBUITES          = 1,
	COMMANDID_PERFORM_NOTIFICATION_ACTION = 2,
};

/*NotificationAttributeID Values*/
enum {
	NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER       = 0,
	NOTIFICATION_ATTRIBUITE_ID_TITLE               = 1,
	NOTIFICATION_ATTRIBUITE_ID_SUBTITLE            = 2,
	NOTIFICATION_ATTRIBUITE_ID_MESSAGE             = 3,
	NOTIFICATION_ATTRIBUITE_ID_MESSAGESIZE         = 4,
	NOTIFICATION_ATTRIBUITE_ID_DATE                = 5,
	NOTIFICATION_ATTRIBUITE_ID_POSITIVEACTIONLABLE = 6,
	NOTIFICATION_ATTRIBUITE_ID_NEGATIVEACTIONLABLE = 7,
};

enum {
	STATE_DATA_ADDED_NEW,
	STATE_DATA_CONTINUE,
	STATE_DATA_DONE,
	STATE_DATA_ERROR,
};

#define NOTIF_DATA_LEN    8

typedef struct {
	UINT8  event_id;
	UINT8  event_flags;
	UINT8  category_id;
	UINT8  category_cnt;
	UINT32 notif_uid;
}notifData_t;

struct notifMsg{
	int state;
	char buf[DATA_BUF_BUF_SIZE];
	int bufLen;
};
static struct notifMsg notifMsg;

static void parseNotifData(const char *srcData, int srcDataLen, notifData_t *notifData)
{
	UINT8 *p = (UINT8*)srcData;
	if(srcDataLen != NOTIF_DATA_LEN){
		notifData->event_id = EVENTID_NOTIFI_UNKNOWN;
		return;
	}
	STREAM_TO_UINT8(notifData->event_id, p);
	STREAM_TO_UINT8(notifData->event_flags, p);
	STREAM_TO_UINT8(notifData->category_id, p);
	STREAM_TO_UINT8(notifData->category_cnt, p);
	STREAM_TO_UINT32(notifData->notif_uid, p);
}

void notif_req_assembling(const char *srcData, int srcDataLen, getNotifCmd_t *getNotifCmd)
{
	UINT8 *p = getNotifCmd->req_buf;
	notifData_t notifData;
	parseNotifData(srcData, srcDataLen, &notifData);
	getNotifCmd->event = notifData.event_id;
	if(notifData.event_id != EVENTID_NOTIFI_ADDED){
		return;
	}
	getNotifCmd->notif_uid = notifData.notif_uid;
	UINT8_TO_STREAM(p, COMMANDID_GETNOTIFICATION_ATTRIBUITES);
	UINT32_TO_STREAM(p, getNotifCmd->notif_uid);

	UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER);
	UINT16_TO_STREAM(p, APPID_CAP);

	UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_TITLE);
	UINT16_TO_STREAM(p, TITLE_CAP);

	UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_SUBTITLE);
	UINT16_TO_STREAM(p, SUBTITLE_CAP);

	UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_DATE);
	UINT16_TO_STREAM(p, DATE_CAP);

	//UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_MESSAGESIZE);
	//UINT16_TO_STREAM(p, MSG_SIZE_CAP);

	UINT8_TO_STREAM(p, NOTIFICATION_ATTRIBUITE_ID_MESSAGE);
	UINT16_TO_STREAM(p, MSG_CAP);
	getNotifCmd->req_buf_len = p - getNotifCmd->req_buf;
}
static int timeDiffMs(struct timeval *new, struct timeval *old)
{
	int res;
	if(new->tv_sec - old->tv_sec > 0xFFFF){
		return 0xFFFF;
	}else{
		res  = (new->tv_sec - old->tv_sec) * 1000;
		res += (new->tv_usec - old->tv_usec)/1000;
		return res;
	}
}

static int checkIntegral(char *data, int len)
{
	int remain_len = len;
	char *p = data;
	int attrLen;
	UINT8 attrId;
	if(remain_len < sizeof(UINT8)+sizeof(UINT32) || remain_len > DATA_BUF_BUF_SIZE){
		return STATE_DATA_ERROR;
	}
	if(data[0] != COMMANDID_GETNOTIFICATION_ATTRIBUITES){
		return STATE_DATA_ERROR;
	}
	remain_len -= 5;
	p += sizeof(UINT8)+sizeof(UINT32);
	while(1){
		if(remain_len < sizeof(UINT8)+sizeof(UINT16)){
			return STATE_DATA_CONTINUE;
		}
		STREAM_TO_UINT8(attrId, p);
		remain_len -= sizeof(UINT8);
		STREAM_TO_UINT16(attrLen, p);
		remain_len -= sizeof(UINT16);

		switch(attrId){
			case NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER :
				if(attrLen > APPID_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				break;
			case NOTIFICATION_ATTRIBUITE_ID_TITLE :
				if(attrLen > TITLE_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				break;
			case NOTIFICATION_ATTRIBUITE_ID_SUBTITLE :
				if(attrLen > SUBTITLE_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				break;
			case NOTIFICATION_ATTRIBUITE_ID_DATE :
				if(attrLen > DATE_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				break;
			case NOTIFICATION_ATTRIBUITE_ID_MESSAGESIZE :
				if(attrLen > MSG_SIZE_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				break;
			case NOTIFICATION_ATTRIBUITE_ID_MESSAGE :
				if(attrLen > MSG_CAP){
					return STATE_DATA_ERROR;
				}
				if(attrLen > remain_len){
					return STATE_DATA_CONTINUE;
				}
				p += attrLen;
				remain_len -= attrLen;
				if(remain_len == 0){
					return STATE_DATA_DONE;
				}else if(remain_len > 0){
					return STATE_DATA_CONTINUE;
				}else{
					return STATE_DATA_ERROR;
				}
				break;
			default :
				p += attrLen;
				remain_len -= attrLen;
				break;
		}
	}
	return 1;
}
void resp_data_assembling(const char *srcData, int srcDataLen, resp_data_func_t resp_data_cb, void *user_data)
{
#define MAX_PARTS_TOUT_MS 2000
	static struct timeval last_tv;  
	static resp_data_t getNotifCmd;

	UINT8 *p;
	struct timeval new_tv;  
	int remain_len, attrLen, res;
	UINT8 commId;
	UINT8 attrId;

	gettimeofday(&new_tv, NULL);
	if(notifMsg.state != STATE_DATA_CONTINUE ||
			timeDiffMs(&new_tv, &last_tv) > MAX_PARTS_TOUT_MS){
		//receive a new notification
		memset(&getNotifCmd, 0, sizeof(resp_data_t));
		notifMsg.bufLen = 0;
		notifMsg.state = STATE_DATA_ADDED_NEW;
	}
	last_tv = new_tv;
	if(notifMsg.bufLen+srcDataLen > DATA_BUF_BUF_SIZE){
		notifMsg.state = STATE_DATA_ERROR;
		printf("Message structure error %d.\n", __LINE__);
		return;
	}
	memcpy(&notifMsg.buf[notifMsg.bufLen], srcData, srcDataLen);
	notifMsg.bufLen += srcDataLen;

	res = checkIntegral(notifMsg.buf, notifMsg.bufLen);
	if(res == STATE_DATA_CONTINUE){
		notifMsg.state = STATE_DATA_CONTINUE;
		return;
	}else if(res == STATE_DATA_ERROR){
		printf("Message structure error %d.\n", __LINE__);
		notifMsg.state = STATE_DATA_ERROR;
		return;
	}

	p = (UINT8*)notifMsg.buf;
	remain_len = notifMsg.bufLen;

	STREAM_TO_UINT8(commId, p);
	remain_len -= 1;
	if(commId != COMMANDID_GETNOTIFICATION_ATTRIBUITES){
		printf("Message structure error %d.\n", __LINE__);
		notifMsg.state = STATE_DATA_ERROR;
		return;
	}
	STREAM_TO_UINT32(getNotifCmd.notif_uid, p);
	remain_len -= sizeof(UINT32);

	while(1){
		if(remain_len > 0){
			STREAM_TO_UINT8(attrId, p);
			remain_len -= sizeof(UINT8);
			STREAM_TO_UINT16(attrLen, p);
			remain_len -= sizeof(UINT16);
			switch(attrId){
				case NOTIFICATION_ATTRIBUITE_ID_APPIDENTIFIER :
					if(attrLen > APPID_CAP){
						notifMsg.state = STATE_DATA_ERROR;
						return;
					}
					STREAM_TO_ARRAY(getNotifCmd.appId, p, attrLen);
					getNotifCmd.appId[attrLen] = 0;
					break;
				case NOTIFICATION_ATTRIBUITE_ID_TITLE :
					if(attrLen > TITLE_CAP){
						notifMsg.state = STATE_DATA_ERROR;
						return;
					}
					STREAM_TO_ARRAY(getNotifCmd.title, p, attrLen);
					getNotifCmd.title[attrLen] = 0;
					break;
				case NOTIFICATION_ATTRIBUITE_ID_SUBTITLE :
					if(attrLen > SUBTITLE_CAP){
						notifMsg.state = STATE_DATA_ERROR;
						return;
					}
					STREAM_TO_ARRAY(getNotifCmd.subtitle, p, attrLen);
					getNotifCmd.subtitle[attrLen] = 0;
					break;
				case NOTIFICATION_ATTRIBUITE_ID_DATE :
					if(attrLen > DATE_CAP){
						notifMsg.state = STATE_DATA_ERROR;
						return;
					}
					STREAM_TO_ARRAY(getNotifCmd.date, p, attrLen);
					getNotifCmd.date[attrLen] = 0;
					break;
				case NOTIFICATION_ATTRIBUITE_ID_MESSAGE :
					if(attrLen > MSG_CAP){
						notifMsg.state = STATE_DATA_ERROR;
						return;
					}
					getNotifCmd.msg_len = attrLen;
					STREAM_TO_ARRAY(getNotifCmd.message, p, attrLen);
					getNotifCmd.message[attrLen] = 0;
					break;
				default :
					p += attrLen;
					break;
			}
			remain_len -= attrLen;
		}else if(remain_len == 0){
			notifMsg.state = STATE_DATA_ADDED_NEW;
			if(resp_data_cb){
				resp_data_cb(&getNotifCmd, user_data);
			}
			return;
		}else{
			printf("Message structure error.\n");
			notifMsg.state = STATE_DATA_ERROR;
			return;
		}
	}
}
