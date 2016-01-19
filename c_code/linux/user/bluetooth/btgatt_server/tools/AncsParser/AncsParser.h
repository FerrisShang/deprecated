#ifndef __ANCS_PARSER_H__
#define __ANCS_PARSER_H__

#include <sys/time.h>
#include <time.h>
#include "Parser_utils.h"

#define GET_NOTIF_CMD_BUF_SIZE 24
#define DATA_BUF_BUF_SIZE 1024

/*capacity*/
#define APPID_CAP        31
#define TITLE_CAP        31
#define SUBTITLE_CAP     31
#define MSG_CAP          511
#define MSG_SIZE_CAP      7
#define DATE_CAP         31

enum {
	EVENTID_NOTIFI_ADDED    = 0,
	EVENTID_NOTIFI_MODIFIED = 1,
	EVENTID_NOTIFI_REMOVED  = 2,
	EVENTID_NOTIFI_UNKNOWN,
};

typedef struct {
	UINT8  event;
	UINT32 notif_uid;
	UINT8  req_buf[GET_NOTIF_CMD_BUF_SIZE];
	UINT8  req_buf_len;
}getNotifCmd_t;

enum {
	STATE_DATA_ADDED_NEW,
	STATE_DATA_CONTINUE,
	STATE_DATA_ERROR,
};

typedef struct {
	UINT8 state;
	UINT32 notif_uid;
	char appId[APPID_CAP+1];
	char title[TITLE_CAP+1];
	char subtitle[SUBTITLE_CAP+1];
	char message[MSG_CAP+1];
	int msg_len;
	char date[DATE_CAP+1];
	char buf[DATA_BUF_BUF_SIZE];
	int bufLen;
}resp_data_t;

typedef void (*resp_data_func_t)(resp_data_t *getNotifCmd, void *user_data);
void notif_req_assembling(const char *srcData, int srcDataLen, getNotifCmd_t *getNotifCmd);
void resp_data_assembling(const char *srcData, int srcDataLen, resp_data_func_t resp_data_cb, void *user_data);

#endif /* __ANCS_PARSER_H__ */
