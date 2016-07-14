#include "stdio.h"
#include "unistd.h"
#include "ipc_shm.h"

#define SHMGET_KEY  0x5FD1
#define SHMGET_SIZE 0x100000

int main(int argc, char *argv[])
{
	char *buf;
	int i, cnt = 10;
#if 1
	buf = create_ipc_shm((key_t)(SHMGET_KEY), SHMGET_SIZE);
	if(!buf){printf("create shared memory failed\n");return 0;}else{printf("%p\n", buf);}
	while(cnt--){
		usleep(500000);
		for(i=0;i<32;i++){
			buf[i] = (cnt+i)&0xFF;
		}
	}
#else
	buf = find_ipc_shm((key_t)(SHMGET_KEY), SHMGET_SIZE);
	if(!buf){printf("find shared memory failed\n");return 0;}else{printf("%p\n", buf);}
	while(cnt--){
		for(i=0;i<32;i++){
			printf("%02x ", (unsigned char)buf[i]);
		}
		printf("\n");
		usleep(500000);
	}
#endif
	destroy_ipc_shm(buf);
	return 0;
}
