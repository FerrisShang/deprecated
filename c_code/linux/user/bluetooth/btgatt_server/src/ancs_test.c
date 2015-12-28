#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"
#include "lib/l2cap.h"
#include "lib/uuid.h"

#include "src/mainloop.h"
#include "src/util.h"
#include "src/att.h"
#include "src/queue.h"
#include "src/gatt-db.h"
#include "src/gatt-client.h"

#define ATT_CID 4

#define PRLOG(...) \
	printf(__VA_ARGS__); print_prompt();

#define COLOR_OFF	"\x1B[0m"
#define COLOR_RED	"\x1B[0;91m"
#define COLOR_GREEN	"\x1B[0;92m"
#define COLOR_YELLOW	"\x1B[0;93m"
#define COLOR_BLUE	"\x1B[0;94m"
#define COLOR_MAGENTA	"\x1B[0;95m"
#define COLOR_BOLDGRAY	"\x1B[1;30m"
#define COLOR_BOLDWHITE	"\x1B[1;37m"

static bool verbose = false;

struct client {
	int fd;
	struct bt_att *att;
	struct gatt_db *db;
	struct bt_gatt_client *gatt;

	unsigned int reliable_session_id;
};

static void print_prompt(void)
{
	printf(COLOR_BLUE "[GATT client]" COLOR_OFF "# ");
	fflush(stdout);
}

static void att_disconnect_cb(int err, void *user_data)
{
	printf("Device disconnected: %s\n", strerror(err));

	mainloop_quit();
}

static void att_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_BOLDGRAY "%s" COLOR_BOLDWHITE "%s\n" COLOR_OFF, prefix, str);
}

static void gatt_debug_cb(const char *str, void *user_data)
{
	const char *prefix = user_data;

	PRLOG(COLOR_GREEN "%s%s\n" COLOR_OFF, prefix, str);
}

static void ready_cb(bool success, uint8_t att_ecode, void *user_data);
static void service_changed_cb(uint16_t start_handle, uint16_t end_handle,
							void *user_data);

static void log_service_event(struct gatt_db_attribute *attr, const char *str)
{
	char uuid_str[MAX_LEN_UUID_STR];
	bt_uuid_t uuid;
	uint16_t start, end;

	gatt_db_attribute_get_service_uuid(attr, &uuid);
	bt_uuid_to_string(&uuid, uuid_str, sizeof(uuid_str));

	gatt_db_attribute_get_service_handles(attr, &start, &end);

	PRLOG("%s - UUID: %s start: 0x%04x end: 0x%04x\n", str, uuid_str,
								start, end);
}

static void service_added_cb(struct gatt_db_attribute *attr, void *user_data)
{
	log_service_event(attr, "Service Added");
}

static void service_removed_cb(struct gatt_db_attribute *attr, void *user_data)
{
	log_service_event(attr, "Service Removed");
}

static struct client *client_create(int fd, uint16_t mtu)
{
	struct client *cli;

	cli = new0(struct client, 1);
	if (!cli) {
		fprintf(stderr, "Failed to allocate memory for client\n");
		return NULL;
	}

	cli->att = bt_att_new(fd, false);
	if (!cli->att) {
		fprintf(stderr, "Failed to initialze ATT transport layer\n");
		bt_att_unref(cli->att);
		free(cli);
		return NULL;
	}

	if (!bt_att_set_close_on_unref(cli->att, true)) {
		fprintf(stderr, "Failed to set up ATT transport layer\n");
		bt_att_unref(cli->att);
		free(cli);
		return NULL;
	}

	if (!bt_att_register_disconnect(cli->att, att_disconnect_cb, NULL,
								NULL)) {
		fprintf(stderr, "Failed to set ATT disconnect handler\n");
		bt_att_unref(cli->att);
		free(cli);
		return NULL;
	}

	cli->fd = fd;
	cli->db = gatt_db_new();
	if (!cli->db) {
		fprintf(stderr, "Failed to create GATT database\n");
		bt_att_unref(cli->att);
		free(cli);
		return NULL;
	}

	cli->gatt = bt_gatt_client_new(cli->db, cli->att, mtu);
	if (!cli->gatt) {
		fprintf(stderr, "Failed to create GATT client\n");
		gatt_db_unref(cli->db);
		bt_att_unref(cli->att);
		free(cli);
		return NULL;
	}

	gatt_db_register(cli->db, service_added_cb, service_removed_cb,
								NULL, NULL);

	if (verbose) {
		bt_att_set_debug(cli->att, att_debug_cb, "att: ", NULL);
		bt_gatt_client_set_debug(cli->gatt, gatt_debug_cb, "gatt: ",
									NULL);
	}

	bt_gatt_client_set_ready_handler(cli->gatt, ready_cb, cli, NULL);
	bt_gatt_client_set_service_changed(cli->gatt, service_changed_cb, cli,
									NULL);

	/* bt_gatt_client already holds a reference */
	gatt_db_unref(cli->db);

	return cli;
}

static void client_destroy(struct client *cli)
{
	close(cli->fd);
	free(cli->db);
	bt_gatt_client_unref(cli->gatt);
	bt_att_unref(cli->att);
	free(cli);
}

static void print_uuid(const bt_uuid_t *uuid)
{
	char uuid_str[MAX_LEN_UUID_STR];
	bt_uuid_t uuid128;

	bt_uuid_to_uuid128(uuid, &uuid128);
	bt_uuid_to_string(&uuid128, uuid_str, sizeof(uuid_str));

	printf("%s\n", uuid_str);
}

static void print_incl(struct gatt_db_attribute *attr, void *user_data)
{
	struct client *cli = user_data;
	uint16_t handle, start, end;
	struct gatt_db_attribute *service;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_incl_data(attr, &handle, &start, &end))
		return;

	service = gatt_db_get_attribute(cli->db, start);
	if (!service)
		return;

	gatt_db_attribute_get_service_uuid(service, &uuid);

	printf("\t  " COLOR_GREEN "include" COLOR_OFF " - handle: "
					"0x%04x, - start: 0x%04x, end: 0x%04x,"
					"uuid: ", handle, start, end);
	print_uuid(&uuid);
}

static void print_desc(struct gatt_db_attribute *attr, void *user_data)
{
	printf("\t\t  " COLOR_MAGENTA "descr" COLOR_OFF
					" - handle: 0x%04x, uuid: ",
					gatt_db_attribute_get_handle(attr));
	print_uuid(gatt_db_attribute_get_type(attr));
}

static void print_chrc(struct gatt_db_attribute *attr, void *user_data)
{
	uint16_t handle, value_handle;
	uint8_t properties;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_char_data(attr, &handle,
								&value_handle,
								&properties,
								&uuid))
		return;

	printf("\t  " COLOR_YELLOW "charac" COLOR_OFF
					" - start: 0x%04x, value: 0x%04x, "
					"props: 0x%02x, uuid: ",
					handle, value_handle, properties);
	print_uuid(&uuid);

	gatt_db_service_foreach_desc(attr, print_desc, NULL);
}

static void print_service(struct gatt_db_attribute *attr, void *user_data)
{
	struct client *cli = user_data;
	uint16_t start, end;
	bool primary;
	bt_uuid_t uuid;

	if (!gatt_db_attribute_get_service_data(attr, &start, &end, &primary,
									&uuid))
		return;

	printf(COLOR_RED "service" COLOR_OFF " - start: 0x%04x, "
				"end: 0x%04x, type: %s, uuid: ",
				start, end, primary ? "primary" : "secondary");
	print_uuid(&uuid);

	gatt_db_service_foreach_incl(attr, print_incl, cli);
	gatt_db_service_foreach_char(attr, print_chrc, NULL);

	printf("\n");
}

static void print_services(struct client *cli)
{
	printf("\n");

	gatt_db_foreach_service(cli->db, NULL, print_service, cli);
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

	print_services(cli);
	print_prompt();
}

static void service_changed_cb(uint16_t start_handle, uint16_t end_handle,
								void *user_data)
{
	struct client *cli = user_data;

	printf("\nService Changed handled - start: 0x%04x end: 0x%04x\n",
						start_handle, end_handle);

	gatt_db_foreach_service_in_range(cli->db, NULL, print_service, cli,
						start_handle, end_handle);
	print_prompt();
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

	printf("Started listening on ATT channel. Waiting for connections\n");

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
static void le_set_advdata(int hdev)
{
	struct hci_request rq;
	le_set_advertising_data_cp cp;
	uint8_t status;
	int dd, err, ret;
	const char adv_len = 31;
	const char advdata[]={
		0x02, 0x01, 0x02, 0x0c, 0x09, 0x40, 0x41, 0x42,
		0x43, 0x44, 0x45, 0x46, 0x47, 0x2d, 0x5f, 0x5f,
		0x05, 0x12, 0x08, 0x08, 0x09, 0x08,
	};

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
	const char adv_len = 24;
	const char advdata[]={
		0x05, 0x12, 0x08, 0x08, 0x09, 0x08, 0x11, 0x15,
		0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4,
		0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79,
	};

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

	dd = hci_open_dev(hdev);
	if (dd < 0) {
		err = -errno;
		fprintf(stderr, "Could not open device: %s(%d)\n",
							strerror(-err), -err);
		exit(1);
	}

	/* Random seed for generating fake random address */
	srand(time(NULL));
	memset(&cp, 0, sizeof(cp));
	cp.bdaddr.b[0] = 0x75;        cp.bdaddr.b[1] = 0x1+rand()%254;
	cp.bdaddr.b[2] = rand()&0xFF; cp.bdaddr.b[3] = rand()&0xFF;
	cp.bdaddr.b[4] = rand()&0xFF; cp.bdaddr.b[5] = rand()&0xFF;

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
	adv_params_cp.own_bdaddr_type = LE_PUBLIC_ADDRESS;//pair fail if set LE_RANDOM_ADDRESS

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
		fprintf(stderr,
			"LE set advertise enable on hci%d returned status %d\n",
								hdev, status);
		return;
	}
}
int main(int argc, char *argv[])
{
	uint16_t mtu = 0;
	bdaddr_t src_addr;
	int dev_id = 0;
	int fd;
	struct client *cli;
	hci_devba(dev_id, &src_addr);

	le_addr(0);
	printf("setting address..\n");
	le_set_advdata(0);
	printf("setting advertise data..\n");
	le_set_scanresp(0);
	printf("setting scan response data..\n");
	le_adv(0);
	printf("advertising..\n");
	printf("listening..\n");
	hci_devba(0, &src_addr);
	mainloop_init();
	fd = l2cap_le_att_listen_and_accept(&src_addr, BT_SECURITY_HIGH, BDADDR_LE_PUBLIC);//BDADDR_LE_RANDOM);
	cli = client_create(fd, mtu);
	if (!cli) {
		close(fd);
		return EXIT_FAILURE;
	}
	while(bt_att_get_security(cli->att) != BT_SECURITY_HIGH){//wait until paring success
		sleep(1);
	}
	mainloop_run();

	printf("\n\nShutting down...\n");

	client_destroy(cli);

	return EXIT_SUCCESS;
}
