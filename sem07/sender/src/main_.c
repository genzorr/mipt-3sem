#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>

#include "tools.h"

void* fun1(void* arg)
{
	int* sems = (int*)arg;

	struct sembuf buf;
	for (;;)
	{
		static int i = 0;

		buf.sem_op = -1;
		buf.sem_flg = 0;
		buf.sem_num = 0;

		semop(sems[0], &buf, 1);
		semop(sems[1], &buf, 1);

		buf.sem_op = 1;
		semop(sems[0], &buf, 1);
		semop(sems[1], &buf, 1);

		printf("thread1 %d\n", i++);
	}

	return NULL;
}


void* fun2(void* arg)
{
	int* sems = (int*)arg;

	struct sembuf buf;
	for (;;)
	{
		static int i = 0;

		buf.sem_op = -1;
		buf.sem_flg = 0;
		buf.sem_num = 0;

		semop(sems[1], &buf, 1);
		semop(sems[0], &buf, 1);

		buf.sem_op = 1;
		semop(sems[0], &buf, 1);
		semop(sems[1], &buf, 1);

		printf("thread2 %d\n", i++);
	}

	return NULL;
}


int main()
{
	int semid1, semid2;
	char pathname1[] = "semtest1";
	char pathname2[] = "semtest2";
	key_t key1, key2;

	//	if opening with creat and excl -> semid == -1

	if((key1 = ftok(pathname1, 0)) < 0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}
	semid1 = semget(key1, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if ((semid1 == -1) && (errno == EEXIST))
	{
		printf("sem1 deleted\n");
		int error = semctl(semid1, 0, IPC_RMID);
		if (error)
		{
			perror("semctl");
			printf("error on removing semaphore\n");
		}
		semid1 = semget(key1, 1, 0666 | IPC_CREAT);
	}

	if((key2 = ftok(pathname2, 0)) < 0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}
	semid2 = semget(key2, 1, 0666 | IPC_CREAT | IPC_EXCL);
	if ((semid2 == -1) && (errno == EEXIST))
	{
		printf("sem2 deleted\n");
		int error = semctl(semid2, 0, IPC_RMID);
		if (error)
			printf("error on removing semaphore\n");
		sleep(15);
		semid2 = semget(key2, 1, 0666 | IPC_CREAT);
	}

	printf("%d %d\n", semid1, semid2);
	sleep(2);

	int sems[2] = {semid1, semid2};

	//	Set semaphores to 1
	struct sembuf mybuf;
	mybuf.sem_op = 	1;
	mybuf.sem_flg = 0;
	mybuf.sem_num = 0;
	semop(sems[0], &mybuf, 1);
	semop(sems[1], &mybuf, 1);

	pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, fun1,	(void*)&sems);
	pthread_create(&thid2, (pthread_attr_t*)NULL, fun2, (void*)&sems);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

	return 0;
}
