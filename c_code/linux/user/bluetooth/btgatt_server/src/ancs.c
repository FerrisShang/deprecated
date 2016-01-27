#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>

#include "src/ancs.h"

#define ANCS_MTU 158
#define ANCS_DEV_ID 0
#define ATT_CID 4
#define ANCS_VERBOSE false

#define PRLOG(...) \
	printf(__VA_ARGS__); 

struct client {
	int fd;
	struct bt_att *att;
	struct gatt_db *db;
	struct bt_gatt_client *gatt;

	unsigned int reliable_session_id;
	ancs_func_t ancs_cb;
};

struct ancs_handle {
	uint16_t notify;
	uint16_t data;
	uint16_t ctrl;
};

struct ancs_handle ancs_handle;


static void att_disconnect_cb(int err, void *user_data)
{
	printf("Device disconnected: %s\n", strerror(err));

	mainloop_quit();
}

static void att_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG("%s" "%s\n" , prefix, str);
}

static void gatt_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG("%s%s\n" , prefix, str);
}

static void log_service_event(struct gatt_db_attribute *attr, const char *str)
{
	char uuid_str[MAX_LEN_UUID_STR];
	bt_uuid_t uuid;
	uint16_t start, end;

	gatt_db_attribute_get_service_uuid(attr, &uuid);
	bt_uuid_to_string(&uuid, uuid_str, sizeof(uuid_str));

	gatt_db_attribute_get_service_handles(attr, &start, &end);

	//PRLOG("%s - UUID: %s start: 0x%04x end: 0x%04x\n", str, uuid_str, start, end);
}

static void service_added_cb(struct gatt_db_attribute *attr, void *user_data)
{
	log_service_event(attr, "Service Added");
}

static void service_removed_cb(struct gatt_db_attribute *attr, void *user_data)
{
	log_service_event(attr, "Service Removed");
}

static void ancs_notify_cb(uint16_t value_handle,
		const uint8_t *value, uint16_t length,
		void *user_data)
{
	struct client *cli = user_data;
	getNotifCmd_t getNotifCmd;
	notif_req_assembling((char*)value, length, &getNotifCmd);
	if(getNotifCmd.event == EVENTID_NOTIFI_ADDED){
		bt_gatt_client_write_value(cli->gatt, ancs_handle.ctrl,
				getNotifCmd.req_buf,getNotifCmd.req_buf_len,
				NULL, NULL, NULL);
	}else if(getNotifCmd.event == EVENTID_NOTIFI_REMOVED){
	}else{
		printf("getNotifCmd.event = %d\n", getNotifCmd.event);
	}
}

static void ancs_data_cb(uint16_t value_handle,
		const uint8_t *value, uint16_t length,
		void *user_data)
{
	struct client *cli = user_data;
	resp_data_assembling((char*)value,length, cli->ancs_cb, NULL);
}

static void register_notify_cb(uint16_t att_ecode, void *user_data)
{
	if (att_ecode) {
		printf("Failed to register notify handler "
					"- error code: 0x%02x\n", att_ecode);
		return;
	}
	//printf("Registered notify handler!\n");
}
static void ancs_notify(struct gatt_db_attribute *attr, void *user_data)
{
	uint16_t handle, value_handle;
	uint8_t properties;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_char_data(attr, &handle,
								&value_handle,
								&properties,
								&uuid))
		return;
	char uuid_str[MAX_LEN_UUID_STR];
	bt_uuid_t uuid128;
	bt_uuid_to_uuid128(&uuid, &uuid128);
	bt_uuid_to_string(&uuid128, uuid_str, sizeof(uuid_str));
	//puts(uuid_str);
	if(ancs_handle.data == 0 &&
			!strcmp(uuid_str, "22eac6e9-24d6-4bb5-be44-b36ace7c7bfb")){
		ancs_handle.data = value_handle;
	}else if(ancs_handle.notify == 0 &&
			!strcmp(uuid_str, "9fbf120d-6301-42d9-8c58-25e699a21dbd")){
		ancs_handle.notify = value_handle;
	}else if(ancs_handle.ctrl == 0 &&
			!strcmp(uuid_str, "69d1d8f3-45e1-49a8-9821-9bbdfdaad9d9")){
		ancs_handle.ctrl = value_handle;
	}
}

void subject_chara(struct gatt_db_attribute *attrib, void *user_data)
{
	struct client *cli = user_data;
	int res;
	gatt_db_service_foreach_char(attrib, ancs_notify, cli);
	while(1){
		//wait until paring success
		res = bt_att_get_security(cli->att);
		if(res == BT_SECURITY_HIGH || res < 0){
			break;
		}
		sleep(1);
	}

	bt_gatt_client_register_notify(cli->gatt , ancs_handle.data,
			register_notify_cb,
			ancs_data_cb, cli, NULL);

	bt_gatt_client_register_notify(cli->gatt , ancs_handle.notify,
			register_notify_cb,
			ancs_notify_cb, cli, NULL);

}

void ancs_subject(struct client *cli)
{
	bt_uuid_t uuid;
	bt_string_to_uuid(&uuid, "7905f431-b5ce-4e99-a40f-4b1e122d00d0");
	gatt_db_foreach_service(cli->gatt->db, &uuid, subject_chara, cli);
}

static void ready_cb(bool success, uint8_t att_ecode, void *user_data)
{
	struct client *cli = user_data;

	if (!success) {
		PRLOG("GATT discovery procedures failed - error code: 0x%02x\n",
								att_ecode);
		return;
	}

	PRLOG("GATT discovery procedures complete\n");
	ancs_subject(cli);
}

static struct client *client_create(int fd, uint16_t mtu, ancs_func_t ancs_cb)
{
	struct client *cli;

	cli = new0(struct client, 1);
	if (!cli) {
		fprintf(stderr, "Failed to allocate memory for client\n");
		return NULL;
	}

	cli->ancs_cb = ancs_cb;
	cli->att = bt_att_new(fd, false);
	if (!cli->att) {
		fprintf(stderr, "Failed to initialze ATT transport layer\n");
		bt_att_unref(cli->att);
		mem_free(cli);
		return NULL;
	}

	if (!bt_att_set_close_on_unref(cli->att, true)) {
		fprintf(stderr, "Failed to set up ATT transport layer\n");
		bt_att_unref(cli->att);
		mem_free(cli);
		return NULL;
	}

	if (!bt_att_register_disconnect(cli->att, att_disconnect_cb, NULL,
								NULL)) {
		fprintf(stderr, "Failed to set ATT disconnect handler\n");
		bt_att_unref(cli->att);
		mem_free(cli);
		return NULL;
	}

	cli->fd = fd;
	cli->db = gatt_db_new();
	if (!cli->db) {
		fprintf(stderr, "Failed to create GATT database\n");
		bt_att_unref(cli->att);
		mem_free(cli);
		return NULL;
	}

	cli->gatt = bt_gatt_client_new(cli->db, cli->att, mtu);
	if (!cli->gatt) {
		fprintf(stderr, "Failed to create GATT client\n");
		gatt_db_unref(cli->db);
		bt_att_unref(cli->att);
		mem_free(cli);
		return NULL;
	}

	gatt_db_register(cli->db, service_added_cb, service_removed_cb,
								NULL, NULL);

	if (ANCS_VERBOSE) {
		bt_att_set_debug(cli->att, att_debug_cb, "att: ", NULL);
		bt_gatt_client_set_debug(cli->gatt, gatt_debug_cb, "gatt: ",
									NULL);
	}

	bt_gatt_client_set_ready_handler(cli->gatt, ready_cb, cli, NULL);

	/* bt_gatt_client already holds a reference */
	gatt_db_unref(cli->db);

	return cli;
}

static void client_destroy(struct client *cli)
{
	close(cli->fd);
	mem_free(cli->db);
	bt_gatt_client_unref(cli->gatt);
	bt_att_unref(cli->att);
	mem_free(cli);
}

static int l2cap_le_att_listen_and_accept(bdaddr_t *src, int sec,
							uint8_t src_type)
{
	int sk, nsk;
	struct sockaddr_l2 srcaddr, addr;
	socklen_t optlen;
	struct bt_security btsec;
	char ba[18];

	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sk < 0) {
		perror("Failed to create L2CAP socket");
		return -1;
	}

	/* Set up source address */
	memset(&srcaddr, 0, sizeof(srcaddr));
	srcaddr.l2_family = AF_BLUETOOTH;
	srcaddr.l2_cid = htobs(ATT_CID);
	srcaddr.l2_bdaddr_type = src_type;
	bacpy(&srcaddr.l2_bdaddr, src);

	/* Set the security level */
	memset(&btsec, 0, sizeof(btsec));
	btsec.level = sec;
	if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &btsec,
							sizeof(btsec)) != 0) {
		fprintf(stderr, "Failed to set L2CAP security level\n");
		goto fail;
	}

	if (bind(sk, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
		perror("Failed to bind L2CAP socket");
		goto fail;
	}

	if (listen(sk, 10) < 0) {
		perror("Listening on socket failed");
		goto fail;
	}

	//printf("Started listening on ATT channel. Waiting for connections\n");

	memset(&addr, 0, sizeof(addr));
	optlen = sizeof(addr);
	nsk = accept(sk, (struct sockaddr *) &addr, &optlen);
	if (nsk < 0) {
		perror("Accept failed");
		goto fail;
	}


	bacpy(src, &addr.l2_bdaddr);
	ba2str(&addr.l2_bdaddr, ba);
	printf("Connect from %s\n", ba);
	close(sk);

	return nsk;

fail:
	close(sk);
	return -1;
}
static void le_set_advdata(int hdev, char *name)
{
	struct hci_request rq;
	le_set_advertising_data_cp cp;
	uint8_t status;
	int dd, err, ret;
	char advdata[31]={
		0x02, 0x01, 0x02,
	};
	const char adv_len = sizeof(advdata);
	int remain_len = adv_len;
	int name_len = strlen(name);
	remain_len -= 3;
	if(name_len>0 && name_len+2<=remain_len){
		advdata[3] = name_len+1;
		advdata[4] = 0x09;
		memcpy(&advdata[5], name, name_len);
	}else{
		printf("warning : name length error \n");
	}

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		err = -errno;
		fprintf(stderr, "Could not open device: %s(%d)\n",
							strerror(-err), -err);
		exit(1);
	}

	cp.length = adv_len;
	memcpy(cp.data, advdata, adv_len);

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISING_DATA;
	rq.cparam = &cp;
	rq.clen = LE_SET_ADVERTISING_DATA_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);
	if (status || ret < 0) {
		err = -errno;
		fprintf(stderr, "Can't set advertising data for hci%d: "
				"%s (%d)\n", hdev, strerror(-err), -err);
		exit(1);
	}

	hci_close_dev(dd);
}

static void le_set_scanresp(int hdev)
{
	struct hci_request rq;
	le_set_scan_response_data_cp cp;
	uint8_t status;
	int dd, err, ret;
	const char advdata[]={
		0x11, 0x15, 0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b,
		0x0f, 0xa4, 0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4,
		0x05, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	const char adv_len = sizeof(advdata);

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		err = -errno;
		fprintf(stderr, "Could not open device: %s(%d)\n",
							strerror(-err), -err);
		exit(1);
	}

	cp.length = adv_len;
	memcpy(cp.data, advdata, adv_len);

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_SCAN_RESPONSE_DATA;
	rq.cparam = &cp;
	rq.clen = LE_SET_SCAN_RESPONSE_DATA_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);
	if (status || ret < 0) {
		err = -errno;
		fprintf(stderr, "Can't set advertising data for hci%d: "
				"%s (%d)\n", hdev, strerror(-err), -err);
		exit(1);
	}

	hci_close_dev(dd);
}

static void le_addr(int hdev)
{
	struct hci_request rq;
	le_set_random_address_cp cp;
	uint8_t status;
	int dd, err, ret;
	bdaddr_t src_addr;

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		err = -errno;
		fprintf(stderr, "Could not open device: %s(%d)\n",
							strerror(-err), -err);
		exit(1);
	}

	hci_devba(ANCS_DEV_ID, &src_addr);
	memcpy(&cp.bdaddr, &src_addr, sizeof(bdaddr_t));
	cp.bdaddr.b[5] = ~cp.bdaddr.b[5];
	cp.bdaddr.b[5] &= 0x7F;
	cp.bdaddr.b[5] |= 0x40;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_RANDOM_ADDRESS;
	rq.cparam = &cp;
	rq.clen = LE_SET_RANDOM_ADDRESS_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);
	if (status || ret < 0) {
		err = -errno;
		fprintf(stderr, "Can't set random address for hci%d: "
				"%s (%d)\n", hdev, strerror(-err), -err);
		exit(1);
	}

	hci_close_dev(dd);
}

static void le_adv(int hdev)
{
	struct hci_request rq;
	le_set_advertise_enable_cp advertise_cp;
	le_set_advertising_parameters_cp adv_params_cp;
	uint8_t status;
	int dd, ret;

	if (hdev < 0)
		hdev = hci_get_route(NULL);

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		perror("Could not open device");
		exit(1);
	}

	memset(&adv_params_cp, 0, sizeof(adv_params_cp));
	adv_params_cp.min_interval = htobs(0x0020);
	adv_params_cp.max_interval = htobs(0x0040);
	adv_params_cp.chan_map = 7;
	adv_params_cp.advtype = 0;
	adv_params_cp.own_bdaddr_type = LE_RANDOM_ADDRESS;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISING_PARAMETERS;
	rq.cparam = &adv_params_cp;
	rq.clen = LE_SET_ADVERTISING_PARAMETERS_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);
	if (ret < 0)
		goto done;

	memset(&advertise_cp, 0, sizeof(advertise_cp));
	advertise_cp.enable = 0x01;

	memset(&rq, 0, sizeof(rq));
	rq.ogf = OGF_LE_CTL;
	rq.ocf = OCF_LE_SET_ADVERTISE_ENABLE;
	rq.cparam = &advertise_cp;
	rq.clen = LE_SET_ADVERTISE_ENABLE_CP_SIZE;
	rq.rparam = &status;
	rq.rlen = 1;

	ret = hci_send_req(dd, &rq, 1000);

done:
	hci_close_dev(dd);

	if (ret < 0) {
		fprintf(stderr, "Can't set advertise mode on hci%d: %s (%d)\n",
						hdev, strerror(errno), errno);
		exit(1);
	}

	if (status) {
		//fprintf(stderr, "LE set advertise enable on hci%d returned status %d\n", hdev, status);
		return;
	}
}

int ancs_start(ancs_func_t ancs_cb, char *le_name)
{
	bdaddr_t src_addr;
	int fd;
	struct client *cli;
	memset(&ancs_handle, 0, sizeof(struct ancs_handle));
	hci_devba(ANCS_DEV_ID, &src_addr);
	le_addr(ANCS_DEV_ID);
	le_set_scanresp(ANCS_DEV_ID);
	le_adv(ANCS_DEV_ID);
	le_set_advdata(ANCS_DEV_ID, le_name);
	mainloop_init();
	printf("Ancs listening..\n");
	fd = l2cap_le_att_listen_and_accept(&src_addr, BT_SECURITY_HIGH, BDADDR_LE_RANDOM);
	cli = client_create(fd, ANCS_MTU, ancs_cb);
	if (!cli) {
		close(fd);
		return EXIT_FAILURE;
	}
	mainloop_run();

	printf("Ancs shutting down...\n");

	client_destroy(cli);

	return EXIT_SUCCESS;
}
