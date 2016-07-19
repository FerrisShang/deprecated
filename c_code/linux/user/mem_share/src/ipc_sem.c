#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include "ipc_sem.h"

#define SEMPERM 0666

union semun {
	int              val;    /* Value for SETVAL */
	struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
	unsigned short  *array;  /* Array for GETALL, SETALL */
	struct seminfo  *__buf;  /* Buffer for IPC_INFO (Linux-specific) */
};

struct ipc_sem* ipc_create_sem(key_t key, int nsems)
{
	int semid;
	semid = semget(key, nsems, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(semid < 0){
		return NULL;
	}else{
		struct ipc_sem *sem;
		sem = mem_malloc(sizeof(struct ipc_sem));
		if(!sem){
			semctl(semid, 0, IPC_RMID);
			return NULL;
		}else{
			sem->key = key;
			sem->semid = semid;
			sem->nsems = nsems;
			return sem;
		}
	}
}
int ipc_init_sem(struct ipc_sem* sem, int *value)
{
	union semun arg;
	int i, res;
	for(i=0;i<sem->nsems;i++){
		arg.val = value[i];
		res = semctl(sem->semid, i, SETVAL, arg);
		if(res < 0){
			return res;
		}
	}
	return 0;
}


int ipc_find_sem(struct ipc_sem* sem)
{
	int semid;
	if(sem->key <= 0){
		return -1;
	}
	semid = semget(sem->key, sem->nsems, SEMPERM | IPC_CREAT | IPC_EXCL);
	if(semid < 0){
		semid = semget(sem->key, sem->nsems, SEMPERM | IPC_CREAT);
		if(semid < 0){
			goto semget_failed;
		}else{
			sem->semid = semid;
			return semid;
		}
	}else{
		semctl(semid, 0, IPC_RMID);
		goto semget_failed;
	}
semget_failed :
	return -1;
}

int ipc_destroy_sem(struct ipc_sem* sem)
{
	int res;
	res = semctl(sem->semid, 0, IPC_RMID);
	if(res < 0){
		return -1;
	}
	mem_free(sem);
	return 0;
}

int ipc_sem_v(int semid, int nsem)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsem;
	v_buf.sem_op  = 1;
	v_buf.sem_flg = 0;
	return semop(semid, &v_buf, 1);
}

int ipc_sem_p(int semid, int nsem)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsem;
	v_buf.sem_op  = -1;
	v_buf.sem_flg = 0;
	return semop(semid, &v_buf, 1);
}

int ipc_sem_v_undo(int semid, int nsem)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsem;
	v_buf.sem_op  = 1;
	v_buf.sem_flg = SEM_UNDO;
	return semop(semid, &v_buf, 1);
}

int ipc_sem_p_undo(int semid, int nsem)
{
	struct sembuf v_buf;
	v_buf.sem_num = nsem;
	v_buf.sem_op  = -1;
	v_buf.sem_flg = SEM_UNDO;
	return semop(semid, &v_buf, 1);
}
