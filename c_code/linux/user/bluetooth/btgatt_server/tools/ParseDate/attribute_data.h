#ifndef __ATTRIBUTE_DATA__
#define __ATTRIBUTE_DATA__

#include "notif_value.h"

typedef void (*FUNC_CALLBK)(void* arg);

struct resp_orig_data{
	unsigned char src_data[DATA_LENGTH];
	int data_len;
	unsigned char in_use;
};

typedef struct resp_orig_data Orig_data;

struct attribute_resp_parse_data{
	unsigned char commandid;
	unsigned char notif_uid[NOTIF_UID_LENGTH]; 	
	char* app_id_str;
	char* app_identifier;
	char* title;
	char* subtitle;
	char* message;
	char* message_size;
	char* date;
	char* positive_actionlabel;
	char* negative_actionlabel;
};

typedef struct attribute_resp_parse_data Resp_data;


struct character_data{
	unsigned char event_id;
	unsigned char event_flags;
	unsigned char category_id;
	unsigned char category_cnt;
	unsigned char notif_uid[NOTIF_UID_LENGTH];
};



extern int parse_data(void* dest_data,void* src_data,int src_len);
extern int resp_data_assembling(void* src_data,int len); 
extern int start_resp_assembler_session(void (* callback)(void* arg));
extern void resp_assembler_data_free(void* arg);
#endif/*__ATTRIBUTE_DATA__*/

