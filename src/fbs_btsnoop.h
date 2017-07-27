#ifndef __BTSNOOP_REC_H__
#define __BTSNOOP_REC_H__

#include <stdio.h>
#define BTDATA_DIR_SEND 0
#define BTDATA_DIR_RECV 1

struct msg_btsnoop {
	char* hci_data;
	int data_len;
	char data_dir;
	long long timestamp;
};

FILE *create_btsnoop_rec(char *path);
void close_btsnoop_rec(FILE *fp);
struct msg_btsnoop* new_btsnoop_msg(char *hci_data, int data_len, char dir);
void free_btsnoop_msg(struct msg_btsnoop *msg);
void record_btsnoop(FILE *fp, struct msg_btsnoop *msg);
void flush_btsnoop(FILE *fp);

#endif /* __BTSNOOP_REC_H__ */
