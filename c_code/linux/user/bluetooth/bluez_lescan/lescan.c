#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/socket.h>

#include "lib/bluetooth.h"
#include "lib/hci.h"
#include "lib/hci_lib.h"

static int err, dd;
static uint8_t own_type = LE_RANDOM_ADDRESS;
static uint8_t scan_type = 0x00; /* Passive */
static uint8_t filter_type = 0;
static uint8_t filter_policy = 0x00;
static uint16_t interval = htobs(0x0020);
static uint16_t window = htobs(0x0020);
static uint8_t filter_dup = 0x00;

#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */
#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */

static void eir_parse_name(uint8_t *eir, size_t eir_len,
						char *buf, size_t buf_len)
{
	size_t offset;

	offset = 0;
	while (offset < eir_len) {
		uint8_t field_len = eir[0];
		size_t name_len;

		/* Check for the end of EIR */
		if (field_len == 0)
			break;

		if (offset + field_len > eir_len)
			goto failed;

		switch (eir[1]) {
		case EIR_NAME_SHORT:
		case EIR_NAME_COMPLETE:
			name_len = field_len - 1;
			if (name_len > buf_len)
				goto failed;

			memcpy(buf, &eir[2], name_len);
			return;
		}

		offset += field_len + 1;
		eir += field_len + 1;
	}

failed:
	snprintf(buf, buf_len, "(unknown)");
}

static void eir_parse_iBeacon(uint8_t *eir, size_t eir_len,
						char *buf, size_t buf_len)
{
	size_t offset;

	offset = 0;
	while (offset < eir_len) {
		uint8_t field_len = eir[0];
		size_t name_len;

		/* Check for the end of EIR */
		if (field_len == 0)
			break;

		if (offset + field_len > eir_len)
			goto failed;

		switch (eir[1]) {
		case 0xFF:
			if(eir[2]==0x4c && eir[3]==0x00){//Company identifier code (0x004C=Apple)
				if(eir[4] == 0x02 && eir[5] == 0x15){//iBeacon advertisement indicator
					sprintf(buf, "\n    iBeacon - UUID:"
							"%02x%02x%02x%02x%02x%02x%02x%02x"
							"%02x%02x%02x%02x%02x%02x%02x%02x "
							"major:0x%02x%02x "
							"minor:0x%02x%02x "
							"TxPower:0x%02x",
							eir[6],eir[7],eir[8],eir[9],
							eir[10],eir[11],eir[12],eir[13],
							eir[14],eir[15],eir[16],eir[17],
							eir[18],eir[19],eir[20],eir[21],
							eir[23],eir[22],
							eir[25],eir[24],
							eir[26]);
					return;
				}
			}
		}
		offset += field_len + 1;
		eir += field_len + 1;
	}

failed:
	snprintf(buf, buf_len, "(not iBeacon)");
}
static int print_advertising_devices(int dd, uint8_t filter_type)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	socklen_t olen;
	int len;

	olen = sizeof(of);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
		printf("Could not get socket options\n");
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
		printf("Could not set socket options\n");
		return -1;
	}
	while (1) {
		evt_le_meta_event *meta;
		le_advertising_info *info;
		char addr[18];

		while ((len = read(dd, buf, sizeof(buf))) < 0) {
			if (errno == EINTR) {
				len = 0;
				goto done;
			}

			if (errno == EAGAIN || errno == EINTR)
				continue;
			goto done;
		}

		int i;
		//for(i=1;i<len;i++){printf("%02x ", buf[i]);if(!(i%16))printf("\n");}printf("\n");
		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		meta = (void *) ptr;

		if (meta->subevent != 0x02)
			goto done;

		/* Ignoring multiple reports */
		info = (le_advertising_info *) (meta->data + 1);
		char name[30];

		memset(name, 0, sizeof(name));

		ba2str(&info->bdaddr, addr);
		eir_parse_name(info->data, info->length,
				name, sizeof(name) - 1);

		char iBeacon[256];
		eir_parse_iBeacon(info->data, info->length,
				iBeacon, sizeof(iBeacon) - 1);

		printf("%s %s %s\n", addr, name, iBeacon);
	}

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if (len < 0)
		return -1;

	return 0;
}
static void cmd_lescan(int dev_id, int argc, char **argv)
{

	dd = hci_open_dev(dev_id);
	if (dd < 0) {
		perror("Could not open device");
		exit(1);
	}
	err = hci_le_set_scan_parameters(dd, scan_type, interval, window,
						own_type, filter_policy, 10000);
	if (err < 0) {
		perror("Set scan parameters failed");
		exit(1);
	}

	err = hci_le_set_scan_enable(dd, 0x01, filter_dup, 10000);
	if (err < 0) {
		perror("Enable scan failed");
		exit(1);
	}

	printf("LE Scan ...\n");

	err = print_advertising_devices(dd, filter_type);
	if (err < 0) {
		perror("Could not receive advertising events");
		exit(1);
	}

	err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 10000);
	if (err < 0) {
		perror("Disable scan failed");
		exit(1);
	}

	hci_close_dev(dd);
}

static void signal_handler_fun(int signal_num)
{
	err = hci_le_set_scan_enable(dd, 0x00, filter_dup, 10000);
	if (err < 0) {
		perror("Disable scan failed");
		exit(1);
	}
	hci_close_dev(dd);
	exit(0);
}
int main(int argc, char *argv[])
{
	int dev_id = 0;
	signal(SIGINT, signal_handler_fun);
	cmd_lescan(dev_id, argc, argv);
	return 0;
}
