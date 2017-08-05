#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "fbs_patchram.h"

//#define dbg_printf(...) printf("patchram:" __VA_ARGS__)
#define dbg_printf(...)

static struct termios termios;
static unsigned char read_buf[32];
static int read_num;
static int len;

static const char hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
static const char hci_baud[] = { 0x01, 0x18, 0xfc, 0x06, 0, 0, 0xc0, 0xc6, 0x2d, 0 };
static const char hci_download[] = { 0x01, 0x2e, 0xfc, 0x00 };
//static const char hci_read_name[] = { 0x01, 0x14, 0x0c, 0x00 };
//static const char hci_write_addr[] = { 0x01,0x01,0xfc,0x06,0x12,0x34,0x56,0x78,0x90,0x5f };
//static const char hci_read_addr[] = { 0x01, 0x09, 0x10, 0x00 };
//static const char hci_write_sleep_mode[] = { 0x01, 0x27, 0xfc, 0x0c, 0x01, 0x01,
//	0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
//static const char hci_write_sco_pcm_int[] = {0x01,0x1C,0xFC,0x05,0,0,0,0,0};
//static const char hci_write_pcm_data_format[] = {0x01,0x1e,0xFC,0x05,0,0,0,0,0};
//static const char hci_write_i2spcm_interface_param[] = {0x01,0x6d,0xFC,0x04,0,0,0,0};

static void fbs_init_uart(int uart_fd, int baud)
{
	tcflush(uart_fd, TCIOFLUSH);
	tcgetattr(uart_fd, &termios);

	cfmakeraw(&termios);

	termios.c_cflag |= CRTSCTS;
	tcsetattr(uart_fd, TCSANOW, &termios);
	tcflush(uart_fd, TCIOFLUSH);
	tcsetattr(uart_fd, TCSANOW, &termios);
	tcflush(uart_fd, TCIOFLUSH);
	tcflush(uart_fd, TCIOFLUSH);
	cfsetospeed(&termios, baud);
	cfsetispeed(&termios, baud);
	tcsetattr(uart_fd, TCSANOW, &termios);
}

static int read_event(int fd, unsigned char *buffer)
{
	int idx = 0;
	int len = 3;
	int count = 0;
	while ((count = read(fd, &buffer[idx], len)) < len) {
		if(count <= 0){
			return -1;
		}
		idx += count;
		len -= count;
	}
	idx += count;
	len = buffer[2];
	while ((count = read(fd, &buffer[idx], len)) < len) {
		if(count <= 0){
			return -1;
		}
		idx += count;
		len -= count;
	}
	idx += count;
	return idx;
}
static void* fbs_patchram_read_event(void *p)
{
	int uart_fd = (int)(size_t)p;
	read_num = read_event(uart_fd, read_buf);
	return NULL;
}

static int fbs_adapt_uart(int uart_fd, int tout_ms, int *data_len, unsigned char *read_buf)
{
	int cnt = tout_ms / 10;
	memset(read_buf, -1, sizeof(read_buf));
	*data_len = 0;
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	while(cnt--){
		if(*data_len == 7 && read_buf[6] == 0){
			return 0;
		}else{
			usleep(10000);
		}
	}
	return -1;
}

static int fbs_adapt_baud(int uart_fd, int baud)
{
	int idx;
	int baud_para[] = {
		B115200,
		B3000000,
	};
	pthread_t th_read_event;
	pthread_create(&th_read_event, NULL, fbs_patchram_read_event, (void*)(size_t)uart_fd);
	usleep(10000);//wait for thread startup
	fbs_init_uart(uart_fd, baud);
	if(fbs_adapt_uart(uart_fd, 100, &read_num, read_buf) == 0){
		dbg_printf("Adapt baud successful %d\n", baud);
		return 0;
	}
	for(idx=0;idx<sizeof(baud_para)/sizeof(baud_para[0]);idx++){
		if(baud != baud_para[idx]){
			fbs_init_uart(uart_fd, baud_para[idx]);
			if(fbs_adapt_uart(uart_fd, 100, &read_num, read_buf) == 0){
				dbg_printf("Adapt baud successful %d\n", baud);
				return 0;
			}
		}
	}
	return -1;
}

int FBS_patchram(tFBS_patchram *info)
{
	FILE *fp;
	int uart_fd = -1;
	unsigned char buffer[512];
	printf("Device:%s\nFirmware:%s\n", info->uart.dev_path, info->uart.fw_path);
	if(info->type != FBS_PATCHRAM_TYPE_UART){
		return FBS_PATCHRAM_ERR_UNSUPPORT;
	}
	uart_fd = open(info->uart.dev_path, O_RDWR | O_NOCTTY);
	if(uart_fd <= 0){
		return FBS_PATCHRAM_ERR_OPEN_UART;
	}
	if(fbs_adapt_baud(uart_fd, B115200) < 0){
		close(uart_fd);
		//try again
		usleep(10000);
		uart_fd = open(info->uart.dev_path, O_RDWR | O_NOCTTY);
		if(uart_fd <= 0){
			return FBS_PATCHRAM_ERR_OPEN_UART;
		}
		if(fbs_adapt_baud(uart_fd, B115200) < 0){
			close(uart_fd);
			return FBS_PATCHRAM_ERR_COMM;
		}
	}
	len = write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);
	fbs_init_uart(uart_fd, B3000000);
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);
	dbg_printf("Downloading patchram ...\n");
	len = write(uart_fd, hci_download, sizeof(hci_download));
	read_event(uart_fd, buffer);
	fp = fopen(info->uart.fw_path, "r");
	if(!fp){
		close(uart_fd);
		return FBS_PATCHRAM_ERR_OPEN_FILE;
	}
	while(fread(&buffer[1], 3, 1, fp)){
		buffer[0] = 1;
		len = fread(&buffer[4], buffer[3], 1, fp);
		len = write(uart_fd, buffer, buffer[3] + 4);
		read_event(uart_fd, buffer);
	}
	fclose(fp);
	dbg_printf("Download successful\n");
	usleep(150000);
	if(fbs_adapt_baud(uart_fd, B115200) < 0){
		close(uart_fd);
		printf("Communicating failed, firmware maybe error\n");
		return FBS_PATCHRAM_ERR_COMM;
	}
	len = write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);
	fbs_init_uart(uart_fd, B3000000);
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);
	close(uart_fd);
	return FBS_PATCHRAM_SUCCESS;
}
