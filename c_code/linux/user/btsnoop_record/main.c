#include <stdio.h>
#include "btsnoop_rec.h"
#include "test_data.h"

FILE *create_btsnoop_rec(char *path);
void close_btsnoop_rec(FILE *fp);
void record_btsnoop(FILE *fp, char *hci_data, int data_len, char data_dir);

int main(void)
{
	FILE *fp;
	fp = create_btsnoop_rec("./btsnoop_hci.log");
	if(fp == NULL){
		printf("open file error\n");
		return -1;
	}
	record_btsnoop(fp, data1, sizeof(data1), DATA_DIR_SEND);
	sleep(1);
	record_btsnoop(fp, data2, sizeof(data2), DATA_DIR_RECV);
	sleep(1);
	record_btsnoop(fp, data3, sizeof(data3), DATA_DIR_SEND);
	sleep(1);
	record_btsnoop(fp, data4, sizeof(data4), DATA_DIR_RECV);
	sleep(1);
	record_btsnoop(fp, data5, sizeof(data5), DATA_DIR_SEND);
	sleep(1);
	record_btsnoop(fp, data6, sizeof(data6), DATA_DIR_RECV);
	sleep(1);
	record_btsnoop(fp, data7, sizeof(data7), DATA_DIR_SEND);
	sleep(1);
	record_btsnoop(fp, data8, sizeof(data8), DATA_DIR_RECV);
	sleep(1);
	close_btsnoop_rec(fp);
	return 0;
}
