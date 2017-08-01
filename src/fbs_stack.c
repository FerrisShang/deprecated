#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glib.h>
#include "fbs_stack.h"
#include "fbs_patchram.h"
#include "fbs_uart.h"
#include "fbs_hci.h"
#include "fbs_l2cap.h"
#include "fbs_ble.h"

static void* fbs_command_recv(void *p); //RFU

static struct FBS_stack_info FBS_stack_info;

static void fbs_stack_hci_cb(guint8 evt, guint8 *data, guint16 len)
{
	//return;
	tFBS_evt_header *header = (tFBS_evt_header*)data;
	switch(header->evt_code){
		default :
			break;
	}
	int i;
	for(i=0;i<len;i++){
		if(!(i%16)){printf("\n");}
		printf("%02x ", data[i]);
	} printf("\n");
	g_warning("Unprocessed HCI event message, type : 0x%02x", evt);
}

void FBS_stack_init_all(void)
{
	const gchar *btsnoop_path;
	btsnoop_path = g_getenv(FBS_BTSNOOP_PATH);
	if(btsnoop_path != NULL){
		FBS_enable_btsnoop(btsnoop_path);
	}

	FBS_hci_init(fbs_stack_hci_cb);
}

static void FBS_uart_report(guchar *data, gint len, gpointer unused)
{
	switch(FBS_UART_MSG_TYPE(data)){
		case FBS_UART_ACLDATA_PKT :
			FBS_l2cap_data_process(data, len);
			break;
		case FBS_UART_EVENT_PKT :
			FBS_hci_evt_process(data, len);
			break;
		default :
			g_warning("Unsupport UART message, type : %d", FBS_UART_MSG_TYPE(data));
			break;
	}
}

void FBS_stack_run(void)
{
	pthread_t th_uart_recv;
	pthread_t th_command_recv;
	GMainLoop *mainloop = NULL;
	mainloop = g_main_loop_new (NULL, FALSE);
	FBS_uart_init(mainloop, FBS_uart_report, NULL);
	pthread_create(&th_uart_recv, NULL, FBS_uart_recv, NULL);
	g_usleep(5000);//wait for thread running
	pthread_create(&th_command_recv, NULL, fbs_command_recv, mainloop);
	g_usleep(5000);//wait for thread running
	g_main_loop_run(mainloop);
	g_main_loop_unref(mainloop);
	pthread_join(th_uart_recv, NULL);
	pthread_join(th_command_recv, NULL);
}

static void* fbs_command_recv(void *p) //RFU
{
	GMainLoop *mainloop = p;
	while(g_main_loop_is_running(mainloop) == FALSE){ g_usleep(1000); }
	while(TRUE){
		if(g_main_loop_is_running(mainloop) == FALSE){ break; }

		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_INFO_PARAM, OCF_READ_LOCAL_VERSION), NULL, 0);
		tFBS_write_bd_addr_cp write_bd_addr = {
			{0x00,0xf5,0xf5,0xf5,0xf5,0x00},
		};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_VENDOR_CMD, OCF_WRITE_BD_ADDR),
				(gpointer)&write_bd_addr, sizeof(tFBS_write_bd_addr_cp));
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_HOST_CTL, OCF_RESET), NULL, 0);

		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_HOST_CTL, OCF_READ_LOCAL_NAME), NULL, 0);
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_INFO_PARAM, OCF_READ_BUFFER_SIZE), NULL, 0);
		tFBS_host_buffer_size_cp host_buffer_size_cp;
		FBS_get_acl_mtu(&host_buffer_size_cp.acl_mtu, &host_buffer_size_cp.acl_max_pkt);
		FBS_get_sco_mtu(&host_buffer_size_cp.sco_mtu, &host_buffer_size_cp.sco_max_pkt);
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_HOST_CTL, OCF_HOST_BUFFER_SIZE),
				(gpointer)&host_buffer_size_cp, sizeof(tFBS_host_buffer_size_cp));
		tFBS_remote_name_req_cp remote_name_req = {
			{0x4f,0xdc,0x83,0x6a,0xdf,0x24},
			0x00,
			0x00,
			0x00};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LINK_CTL, OCF_REMOTE_NAME_REQ),
				(gpointer)&remote_name_req, sizeof(tFBS_remote_name_req_cp));

		tFBS_le_set_random_address_cp le_set_random_address = {
			{0x00,0x5f,0x5f,0x5f,0x5f,0x00}};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_RANDOM_ADDRESS),
				(gpointer)&le_set_random_address, sizeof(tFBS_le_set_random_address_cp));

		tFBS_le_set_scan_parameters_cp le_set_scan_parameters = {
			0x01,
			0x0400,
			0x0040,
			0x00,
			0x00};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_PARAMETERS),
				(gpointer)&le_set_scan_parameters , sizeof(tFBS_le_set_scan_parameters_cp));

		tFBS_le_set_scan_enable_cp le_set_scan_enable= {
			0x01,
			0x00};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_ENABLE),
				(gpointer)&le_set_scan_enable, sizeof(tFBS_le_set_scan_enable_cp));

		tFBS_le_set_advertising_parameters_cp le_set_advertising_parameters = {
			0x0040,
			0x0080,
			0x00,
			0x00,
			0x00,
			{0x00,0x00,0x00,0x00,0x00,0x00},
			0x07,
			0x00};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_PARAMETERS),
				(gpointer)&le_set_advertising_parameters,
				sizeof(tFBS_le_set_advertising_parameters_cp));

		tFBS_le_set_advertising_data_cp le_set_advertising_data = {
			0x1F,
			{
				0x02,0x01,0x07,0x02,0x09, ' ',
			} };
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISING_DATA),
				(gpointer)&le_set_advertising_data,
				sizeof(tFBS_le_set_advertising_data_cp));

		tFBS_le_set_advertise_enable_cp le_set_advertise_enable = {
			0x01};
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_ADVERTISE_ENABLE),
				(gpointer)&le_set_advertise_enable, sizeof(tFBS_le_set_advertise_enable_cp));

		g_usleep(5*1000*1000);
		le_set_scan_enable.enable = 0;
		FBS_hci_send(FBS_CMD_OPCODE_PACK(OGF_LE_CTL, OCF_LE_SET_SCAN_ENABLE),
				(gpointer)&le_set_scan_enable, sizeof(tFBS_le_set_scan_enable_cp));

		g_message("Done");
		g_usleep(~0);
	}
	return NULL;
}
