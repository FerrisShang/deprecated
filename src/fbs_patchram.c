#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fbs_patchram.h"

static struct termios termios;
static int read_num;

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
	if(baud == 3000000){
		cfsetospeed(&termios, B3000000);
		cfsetispeed(&termios, B3000000);
	}else{ //default baud 115200
		cfsetospeed(&termios, B115200);
		cfsetispeed(&termios, B115200);
	}
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
static void* thread_read_event(void *p)
{
	unsigned char buf[32];
	int uart_fd = (int)(size_t)p;
	read_num = read_event(uart_fd, buf);
	return NULL;
}

static int fbs_wait_uart(int tout_ms, int *data_len)
{
	int cnt = tout_ms / 10;
	while(cnt--){
		if(*data_len != 0){
			return 0;
		}else{
			usleep(10000);
		}
	}
	return -1;
}

static int fbs_adapt_baud(int uart_fd)
{
	int len;
	pthread_t th_read_event;
	fbs_init_uart(uart_fd, 115200);
	pthread_create(&th_read_event, NULL, thread_read_event, (void*)(size_t)uart_fd);
	usleep(10000);//wait for thread startup
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	if(fbs_wait_uart(100, &read_num) < 0){
		fbs_init_uart(uart_fd, 3000000);
		len = write(uart_fd, hci_reset, sizeof(hci_reset));
		if(fbs_wait_uart(100, &read_num) < 0){
			return -1;
		}
	}
	return 0;
}

int FBS_patchram(tFBS_patchram *info)
{
	FILE *fp;
	int uart_fd = -1;
	int len;
	unsigned char buffer[512];
	if(info->type != FBS_PATCHRAM_TYPE_UART){
		return FBS_PATCHRAM_ERR_UNSUPPORT;
	}
	uart_fd = open(info->uart.dev_path, O_RDWR | O_NOCTTY);
	if(uart_fd <= 0){
		return FBS_PATCHRAM_ERR_OPEN_UART;
	}
	if(fbs_adapt_baud(uart_fd) < 0){
		close(uart_fd);
		return FBS_PATCHRAM_ERR_COMM;
	}
	len = write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);
	fbs_init_uart(uart_fd, 3000000);
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);
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
	usleep(20000);
	fbs_init_uart(uart_fd, 115200);
	len = write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);
	fbs_init_uart(uart_fd, 3000000);
	len = write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);
	close(uart_fd);
	return FBS_PATCHRAM_SUCCESS;
}
