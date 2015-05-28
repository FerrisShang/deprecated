
#define PREAMBLE_POS 0
#define LEN_POS 2
#define VERSION_POS 4
#define CMD_POS 8
#define DATA_POS 10

#define PREAMBLE {0x4A, 0x5A}
#define DEVICE_ID {0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, \
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31}
#define VERSION {0x00, 0x00, 0x01 ,0x01}

#define CMD_LOGIN			 0x01
#define CMD_ACK_LOGIN		 0x02
#define CMD_SET_STATUS		 0x03
#define CMD_GET_STATUS		 0x04
#define CMD_REPORT_STATUS	 0x05

#define HEARTBEAT_INTV		 12
int str_login(char *buf);
int str_report(char *buf);
