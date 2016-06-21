#include <stdio.h>
#include <unistd.h>
#include "l2cap.h"
#include "bt_util.h"
#include "hci.h"
#include "log.h"

#define ATT_CID 4

int l2cap_le_att_listen_and_accept(int hdev, int security,
		void (*connected_cb)(int fd, bdaddr_t addr))
{
	int sk;
	struct sockaddr_l2 srcaddr;
	socklen_t optlen;
	struct bt_security btsec;
	//bdaddr_t rand_addr;
	bdaddr_t src_addr;

	//hci_le_get_random_address(&rand_addr);
	hci_devba(hdev, &src_addr);
	sk = socket(PF_BLUETOOTH, SOCK_SEQPACKET, BTPROTO_L2CAP);
	if (sk < 0) {
		Log.e("Failed to create L2CAP socket");
		return -1;
	}

	/* Set up source address */
	memset(&srcaddr, 0, sizeof(srcaddr));
	srcaddr.l2_family = AF_BLUETOOTH;
	srcaddr.l2_cid = htobs(ATT_CID);
	srcaddr.l2_bdaddr_type = BDADDR_LE_RANDOM;
	bacpy(&srcaddr.l2_bdaddr, &src_addr);

	/* Set the security level */
	memset(&btsec, 0, sizeof(btsec));
	btsec.level = security;
	if (setsockopt(sk, SOL_BLUETOOTH, BT_SECURITY, &btsec,
				sizeof(btsec)) != 0) {
		Log.e("Failed to set L2CAP security level");
		goto fail;
	}

	if (bind(sk, (struct sockaddr *) &srcaddr, sizeof(srcaddr)) < 0) {
		Log.e("Failed to bind L2CAP socket");
		goto fail;
	}

	if (listen(sk, 10) < 0) {
		Log.e("Listening on socket failed");
		goto fail;
	}
	Log.v("Started listening on ATT channel. Waiting for connections");

	while(1){
		struct sockaddr_l2 addr;
		char ba[18];
		int nsk;
		memset(&addr, 0, sizeof(addr));
		optlen = sizeof(addr);
		nsk = accept(sk, (struct sockaddr *) &addr, &optlen);
		if (nsk < 0) {
			Log.e("Accept failed");
			goto fail;
		}
		ba2str(&addr.l2_bdaddr, ba);
		Log.v("Connect from %s\n", ba);
		connected_cb(nsk, addr.l2_bdaddr);
	}
fail:
	close(sk);
	return -1;
}

int le_set_random_address(int hdev)
{
	int timeout = 1000;
	int dd;
	bdaddr_t src_addr;
	dd = hci_open_dev(hdev);
	if (dd < 0) {
		Log.e("Could not open device");
		return -1;
	}
	hci_devba(hdev, &src_addr);
	//translate public address to random address
	src_addr.b[5] = ~src_addr.b[5];
	src_addr.b[5] &= 0x7F;
	src_addr.b[5] |= 0x40;
	hci_le_set_random_address(dd, &src_addr, timeout);
	hci_close_dev(dd);
	return 0;
}

int le_set_advertise_parameters(int hdev)
{
	int timeout = 1000;
	int dd;
	bdaddr_t direct_bdaddr = {{0}};
	dd = hci_open_dev(hdev);
	if (dd < 0) {
		Log.e("Could not open device");
		return -1;
	}
	hci_le_set_advertise_parameters(dd,
			0x0020, 0x0040,
			0,      LE_RANDOM_ADDRESS,
			0,      direct_bdaddr,
			7,      0, timeout);
	hci_close_dev(dd);
	return 0;
}

int le_set_advertise_enable(int hdev)
{
	int timeout = 1000;
	int dd;
	dd = hci_open_dev(hdev);
	if (dd < 0) {
		Log.e("Could not open device");
		return -1;
	}
	hci_le_set_advertise_enable(dd, 1, timeout);
	hci_close_dev(dd);
	return 0;
}
void dump_btaddr(char *prefix, bdaddr_t *addr){
	char ba[18];
	ba2str(addr, ba);
	Log.v("%s %s", prefix, ba);
}

