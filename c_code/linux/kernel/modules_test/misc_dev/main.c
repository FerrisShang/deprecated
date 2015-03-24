#include <stdio.h>
#include <fcntl.h>

#define DEV "/dev/misc_dev"

int main(int argc, char *argv[])
{
	int fd;
	fd = open(DEV, O_RDWR);
	read(fd, "read file", 10);
	write(fd, "write file", 11);
	fseek(fd, 17, SEEK_CUR);
	//printf("feof = %d\n", feof(fd));
	close(fd);
}
