#include <stdio.h>
#include <glib.h>
#include "fbs_stack.h"

extern guint FBS_hci_send(guchar *data, guint len);//Test

struct FBS_stack_info {
	guint (*hci_send)(guchar *data, guint len);
};

static struct FBS_stack_info FBS_stack_info;
#define HCI_SEND_DATA_LEN(d) (d[0]==1?(4+d[3]):(5+d[3]+(d[4]<<8)))
#define HCI_RECV_DATA_LEN(d) (d[0]==4?(3+d[2]):(5+d[3]+(d[4]<<8)))
void FBS_stack_init_all(void)
{
	FBS_stack_info.hci_send = FBS_hci_send;
}

void FBS_hci_report(guchar *user_data)
{
	int i;
	for(i=0;i<HCI_RECV_DATA_LEN(user_data);i++){
		if(!(i%16)) printf("\n");
		printf("%02x ", user_data[i]);
	}
	printf("\n");
}
void FBS_hci_send_raw(guchar *user_data)
{
	FBS_stack_info.hci_send(user_data, HCI_SEND_DATA_LEN(user_data));
}
