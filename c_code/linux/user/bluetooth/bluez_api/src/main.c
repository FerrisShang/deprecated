#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gatt.h"
#include "mgmt.h"
#include "log.h"
#include "mem_manage.h"
#define HCI_DEV_ID 0

void init_gatt_services(void);
int main(int argc, char *argv[])
{
	mgmt_setup(HCI_DEV_ID);
	mgmt_set_name(HCI_DEV_ID,"Smart_F", "SF");
	mgmt_set_iocap(HCI_DEV_ID, SMP_IO_NO_INPUT_OUTPUT);
	le_set_random_address(HCI_DEV_ID);
	le_set_advertise_parameters(HCI_DEV_ID);
	init_gatt(HCI_DEV_ID);
	init_gatt_services();
	le_set_advertise_enable(HCI_DEV_ID);
	while(1){
		usleep(1000000);
	}
	return 0;
}
