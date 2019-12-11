#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

#include <pthread.h>
#include <fcntl.h>


#include "tools.h"

#define TABLE_LIMIT		3
#define DISHES_COUNT	10

int pip[2];


void* Washer(void* arg)
{
	int semid = *(int*)arg;
	int washer_times[DISHES_COUNT] = {
			1, 1, 1, 1, 1,
			2, 3, 4, 5, 1
	};

	int wiper_times[DISHES_COUNT] = {
			3, 3, 3, 3, 3,
			4, 4, 4, 3, 4
	};

	struct sembuf buf[2];
	buf[0].sem_op	= -1;
	buf[0].sem_flg	= 0;
	buf[0].sem_num	= 0;

	buf[1].sem_op	= 1;
	buf[1].sem_flg	= 0;
	buf[1].sem_num	= 1;

//	close(pip[0]);

	for (int i = 0; i < DISHES_COUNT; i++)
	{
		write(pip[1], &wiper_times[i], sizeof(int));
		green; printf("# Dish %d washed\n", i); reset_color;
		sleep(washer_times[i]);

		semop(semid, buf, 2);
	}

	close(pip[1]);

	return NULL;
}

void* Wiper(void* arg)
{
	int semid = *(int*)arg;

	struct sembuf buf[2];
	buf[0].sem_op	= 1;
	buf[0].sem_flg	= 0;
	buf[0].sem_num	= 0;

	buf[1].sem_op	= -1;
	buf[1].sem_flg	= 0;
	buf[1].sem_num	= 1;

//	close(pip[1]);

	int time = 0;
	for (int i = 0; i < DISHES_COUNT; i++)
	{
		read(pip[0], &time, sizeof(int));
		sleep(time);
//		sleep(wiper_times[i]);
		yellow; printf("# Dish %d wiped\n", i); reset_color;

		semop(semid, buf, 2);
	}

	close(pip[0]);

	return NULL;
}



int main()
{
	//	Create array of 2 semaphores.
	int semid = 0;
	char semname[] = "sem";
	key_t key;

	if((key = ftok(semname, 0)) < 0)
	{
		printf("# Can't generate key\n");
		exit(-1);
	}
	semid = semget(key, 2, 0666 | IPC_CREAT | IPC_EXCL);
	if (semid < 0)
	{
		printf("# Semaphore error\n");
		exit(-1);
	}

	//	Set semaphores to 1
	struct sembuf buf[2];
	buf[0].sem_op	= TABLE_LIMIT;		//	free counter
	buf[0].sem_flg	= 0;
	buf[0].sem_num	= 0;

	buf[1].sem_op	= 0;				//	non-free counter
	buf[1].sem_flg	= 0;
	buf[1].sem_num	= 1;

	semop(semid, buf, 2);

	if (pipe(pip) < 0)
	{
		printf("Can't open pipe\n");
		return -1;
	}

	//	Create threads.
	pthread_t thid1, thid2;
	pthread_create(&thid1, (pthread_attr_t*)NULL, Washer,	(void*)&semid);
	pthread_create(&thid2, (pthread_attr_t*)NULL, Wiper, 	(void*)&semid);

	pthread_join(thid1,	(void**)NULL);
	pthread_join(thid2,	(void**)NULL);

	semctl(semid, 2, IPC_RMID, NULL);

	return 0;
}
