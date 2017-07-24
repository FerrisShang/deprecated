#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct termios termios;
char tty_dev[] = "/dev/ttyS0";
char patchram_file[256] = "/lib/firmware/BCM4343A0_001.001.034.0056.0221_26M_ORC.hcd";

void init_uart(int uart_fd)
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
	cfsetospeed(&termios, B115200);
	cfsetispeed(&termios, B115200);
	tcsetattr(uart_fd, TCSANOW, &termios);
}
int read_event(int fd, unsigned char *buffer)
{
	int i = 0;
	int len = 3;
	int count = 0;
	while ((count = read(fd, &buffer[i], len)) < len) {
		if(count < 0){sleep(1);printf("%d %d\n", i,len);}else if(count == 0){printf("x\n");}
		else{
		i += count;
		len -= count;
		}
	}
	i += count;
	len = buffer[2];

	while ((count = read(fd, &buffer[i], len)) < len) {
		if(count <= 0){printf("x\n");}
		i += count;
		len -= count;
	}
	i += count;

	int x;for(x=0;x<i;x++){ printf("%02x ", buffer[x]); } printf("\n");
	return i;
}
int main(int argc, char *argv[])
{
	FILE *fp;
	int uart_fd = -1;
	int len, i;
	char hci_reset[] = { 0x01, 0x03, 0x0c, 0x00 };
	char hci_baud[] = { 0x01, 0x18, 0xfc, 0x06, 0, 0, 0xc0, 0xc6, 0x2d, 0 };
	char hci_download[] = { 0x01, 0x2e, 0xfc, 0x00 };
	char hci_read_name[] = { 0x01, 0x14, 0x0c, 0x00 };
	char hci_write_addr[] = { 0x01, 0x01, 0xfc, 0x06, 0x12, 0x34, 0x56, 0x78, 0x90, 0x5f };
	char hci_read_addr[] = { 0x01, 0x09, 0x10, 0x00 };
	char hci_write_sleep_mode[] = { 0x01, 0x27, 0xfc, 0x0c, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
	char hci_write_sco_pcm_int[] = {0x01,0x1C,0xFC,0x05,0x00,0x00,0x00,0x00,0x00};
	char hci_write_pcm_data_format[] = {0x01,0x1e,0xFC,0x05,0x00,0x00,0x00,0x00,0x00};
	char hci_write_i2spcm_interface_param[] = {0x01,0x6d,0xFC,0x04,0x00,0x00,0x00,0x00};

	system("echo 1 > /sys/class/rfkill/rfkill0/state");
	if(argc > 1){
		strcpy(tty_dev, argv[1]);
	}
	if(argc > 2){
		strcpy(patchram_file, argv[2]);
	}

	unsigned char buffer[512];
	uart_fd = open(tty_dev, O_RDWR | O_NOCTTY);
	if(uart_fd <= 0){
		printf("uart = %d failed\n", uart_fd);
		return -1;
	}
	init_uart(uart_fd);
	write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);
	write(uart_fd, hci_read_name, sizeof(hci_read_name));
	len = read_event(uart_fd, buffer);

	write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);

	cfsetospeed(&termios, B3000000);
	cfsetispeed(&termios, B3000000);
	tcsetattr(uart_fd, TCSANOW, &termios);

	write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);

	write(uart_fd, hci_download, sizeof(hci_download));
	read_event(uart_fd, buffer);

	fp = fopen(patchram_file, "r");
	//fp = fopen("/lib/firmware/BCM4343A1_001.002.009.0025.0059.hcd", "r");
	if(!fp){
		printf("read file failed\n");
	}
	while(fread(&buffer[1], 3, 1, fp)){
		len = buffer[3];
		buffer[0] = 1;
		fread(&buffer[4], len, 1, fp);
		write(uart_fd, buffer, len + 4);
		len = read_event(uart_fd, buffer);
	}
	usleep(20000);
	cfsetospeed(&termios, B115200);
	cfsetispeed(&termios, B115200);
	tcsetattr(uart_fd, TCSANOW, &termios);

	write(uart_fd, hci_baud, sizeof(hci_baud));
	read_event(uart_fd, buffer);

	cfsetospeed(&termios, B3000000);
	cfsetispeed(&termios, B3000000);
	tcsetattr(uart_fd, TCSANOW, &termios);

	write(uart_fd, hci_reset, sizeof(hci_reset));
	read_event(uart_fd, buffer);

	write(uart_fd, hci_read_name, sizeof(hci_read_name));
	len = read_event(uart_fd, buffer);
	puts(&buffer[7]);

	write(uart_fd, hci_write_addr, sizeof(hci_write_addr));
	len = read_event(uart_fd, buffer);

	write(uart_fd, hci_read_addr, sizeof(hci_read_addr));
	len = read_event(uart_fd, buffer);

	fclose(fp);
	close(uart_fd);
	return 0;
}
