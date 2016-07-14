#include "ipc_sem.h"
#include <sys/types.h>
#include <sys/ipc.h>

#define SEMPERM 0666

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

int create_ipc_sem(key_t key, int nsems, int *value)
{
	int semid, i;
	semid = semget(key, nsems, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(semid < 0){
		goto semget_failed;
	}else{
		union semun arg;
		int status = 0;
		for(i=0;i<nsems;i++){
			arg.val = value[i];
			status = semctl(semid, i, SETVAL, arg);
			if(status < 0){
				goto semctl_failed;
			}
		}
	}
	return semid;
semctl_failed :
	for(i--;i>0;--i){
		semctl(semid, i, IPC_RMID);
	}
semget_failed :
	return -1;
}

int find_ipc_sem(key_t key, int nsems)
{
	int semid;
	semid = semget(key, nsems, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(semid < 0){
		semid = semget(key, nsems, SEMPERM | IPC_CREAT);
		if(semid < 0){
			goto semget_failed;
		}else{
			return semid;
		}
	}else{
		destroy_ipc_sem(semid);
		goto semget_failed;
	}
semget_failed :
	return -1;
}

int destroy_ipc_sem(int semid)
{
	return semctl(semid, 0, IPC_RMID);
}

int ipc_sem_v(int semid, int nsems)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsems;
	v_buf.sem_op  = 1;
	v_buf.sem_flg = SEM_UNDO;
	return semop(semid, &v_buf, 1);
}

int ipc_sem_p(int semid, int nsems)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsems;
	v_buf.sem_op  = -1;
	v_buf.sem_flg = SEM_UNDO;
	return semop(semid, &v_buf, 1);
}
