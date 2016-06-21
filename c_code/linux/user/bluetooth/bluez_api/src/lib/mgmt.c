#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include "mgmt.h"

#define MGMT_OP_SET_BONDABLE  0x0009
#define MGMT_OP_SET_LE        0x000D
#define MGMT_OP_DISCONNECT    0x0014
#define MGMT_HDR_SIZE         6

struct mgmt_hdr {
	uint16_t opcode;
	uint16_t index;
	uint16_t len;
} __attribute__((packed));

struct mgmt_mode {
	uint8_t val;
} __attribute__((packed));

struct mgmt_addr_info {
		bdaddr_t bdaddr;
			uint8_t type;
} __attribute__((packed));

struct mgmt_cp_disconnect {
		struct mgmt_addr_info addr;
} __attribute__((packed));

static int mgmt_set_mode(int hdev, int mgmt_sock, uint16_t opcode, uint8_t val)
{
	char buf[MGMT_HDR_SIZE + sizeof(struct mgmt_mode)];
	struct mgmt_hdr *hdr = (void *) buf;
	struct mgmt_mode *cp = (void *) &buf[sizeof(*hdr)];

	memset(buf, 0, sizeof(buf));
	hdr->opcode = htobs(opcode);
	hdr->index = htobs(hdev);
	hdr->len = htobs(sizeof(*cp));

	cp->val = val;

	if (write(mgmt_sock, buf, sizeof(buf)) < 0)
		return -1;

	return 0;
}

int mgmt_setup(int hdev)
{
	int dd,ret;
	struct sockaddr_hci addr;
	ret = 0;
	dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (dd < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = HCI_DEV_NONE;
	addr.hci_channel = HCI_CHANNEL_CONTROL;

	if (bind(dd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(dd);
		return -1;
	}

	ret = mgmt_set_mode(hdev, dd, MGMT_OP_SET_LE, 1);
	ret = mgmt_set_mode(hdev,dd, MGMT_OP_SET_BONDABLE, 1);

	close(dd);
	return ret;
}

int mgmt_disconnect(int hdev, bdaddr_t *bdaddr, uint8_t bdaddr_type)
{
	char buf[MGMT_HDR_SIZE + sizeof(struct mgmt_cp_disconnect)];
	struct mgmt_hdr *hdr = (void *) buf;
	struct mgmt_cp_disconnect *cp = (void *) &buf[sizeof(*hdr)];
	int dd,ret;
	struct sockaddr_hci addr;
	ret = 0;
	dd = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI);
	if (dd < 0)
		return -1;

	memset(&addr, 0, sizeof(addr));
	addr.hci_family = AF_BLUETOOTH;
	addr.hci_dev = HCI_DEV_NONE;
	addr.hci_channel = HCI_CHANNEL_CONTROL;

	if (bind(dd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
		close(dd);
		return -1;
	}
	memset(buf, 0, sizeof(buf));
	hdr->opcode = htobs(MGMT_OP_DISCONNECT);
	hdr->len = htobs(sizeof(*cp));
	hdr->index = htobs(hdev);

	bacpy(&cp->addr.bdaddr, bdaddr);
	cp->addr.type = bdaddr_type;

	if (write(dd, buf, sizeof(buf)) < 0){
		printf("%s: failed\n",__FUNCTION__);
	}
	return ret;
}
