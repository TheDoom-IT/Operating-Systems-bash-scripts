#include <stdio.h>
#include <sys/sem.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/stat.h>

union semun
{
	int val;			   /* Value for SETVAL */
	struct semid_ds *buf;  /* Buffer for IPC_STAT, IPC_SET */
	unsigned short *array; /* Array for GETALL, SETALL */
	struct seminfo *__buf; /* Buffer for IPC_INFO
							  (Linux-specific) */
};

void grab_forks(int left_fork_id);
void put_away_forks(int left_fork_id);
void philosopher(int id);

void lock_both(int, int);
void unlock_both(int, int);
void do_two_semop(int, int, int);

const int SIZE = 5;
const int proj_id = 123456;
int semaphore;

int main()
{
	key_t key = ftok("/home/mion/s/242/dbadurek/projects/eopsy/task5/main.c", proj_id);
	if (key == -1)
	{
		printf("Parent: Unable to generate key\n");
		return 1;
	}
	printf("Key: %d\n", key);
	printf("Parent: Creating semaphores\n");
	semaphore = semget(key, SIZE, IPC_CREAT | S_IRUSR | S_IWUSR);
	if (semaphore == -1)
	{
		printf("Failed to create a semaphores\n");
		return 1;
	}
	// set initial value of semaphore
	union semun value;
	value.val = 1;
	for (int x = 0; x < SIZE; x++)
	{
		semctl(semaphore, x, SETVAL, value);
	}
	printf("Parent: Created semaphore set: %d\n", semaphore);

	for (int x = 0; x < 5; x++)
	{
		int result = fork();
		if (result < 0)
		{
			printf("Failed to create child process");
			return -1;
		}
		else if (result == 0)
		{
			// Philosopher algorithm
			philosopher(x);
			return 0;
		}
	}
	// Parent algorithm
	while (1)
	{
	}
}

void philosopher(int id)
{
	printf("Philosopher %d: Start working\n", id);
	for (int x = 0; x < 10; x++)
	{
		printf("Philosopher %d: Thinking...\n", id);
		grab_forks(id);
		printf("Philosopher %d: Eating...\n", id);
		sleep(1);
		put_away_forks(id);
	}
}

void grab_forks(int left_fork_id)
{
	printf("Philosopher %d: Waiting for forks\n", left_fork_id);
	int rigth_fork_id = (left_fork_id + 1) % 5;
	lock_both(left_fork_id, rigth_fork_id);
	printf("Philosopher %d: Picked both forks\n", left_fork_id);
}

void put_away_forks(int left_fork_id)
{
	printf("Philosopher %d: Putting down forks\n", left_fork_id);
	int rigth_fork_id = (left_fork_id + 1) % 5;
	unlock_both(left_fork_id, rigth_fork_id);
}

void lock_both(int sem_id_1, int sem_id_2) {
	do_two_semop(sem_id_1, sem_id_2, -1);
}


void unlock_both(int sem_id_1, int sem_id_2) {
	do_two_semop(sem_id_1, sem_id_2, 1);
}

void do_two_semop(int sem_id_1, int sem_id_2, int sem_op) {
	struct sembuf sb[2];
	sb[0].sem_num = sem_id_1;
	sb[0].sem_op = sem_op;
	sb[0].sem_flg = 0;
	sb[1].sem_num = sem_id_2;
	sb[1].sem_op = sem_op;
	sb[1].sem_flg = 0;
	if (semop(semaphore, sb, 2) == -1)
	{
		printf("Two semop failed: op = %d sem_id_1: %d sem_id_2: %d\n", sem_op, sem_id_1, sem_id_2);
		exit(1);
	}
}
