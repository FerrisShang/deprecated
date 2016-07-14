#include <stdio.h>
#include <errno.h>
#include "unistd.h"
#include "wi_bus.h"

#define SHMGET_KEY  0x5FD1
#define IPC_SEM_KEY 0x5FD0
#define SHMGET_SIZE 0x100000

int main(int argc, char *argv[])
{
#if 0
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
#else
#define NSEMS 10
#if 0
	int init_value[10] = {0,2,3,4,5,6,7,8,9,0};
	int semid = create_ipc_sem(IPC_SEM_KEY, NSEMS, init_value);
	printf("semid = %d\n", semid);
	usleep(4000000);
	ipc_sem_v(semid, 0);
	destroy_ipc_sem(semid);
#else
	int semid = find_ipc_sem(IPC_SEM_KEY, NSEMS);
	printf("%d e:%d\n", ipc_sem_p(semid, 0), errno);
	printf("semid = %d\n", semid);
#endif
#endif
	return 0;
}
