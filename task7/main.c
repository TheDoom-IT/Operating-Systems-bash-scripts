#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define N	5
#define LEFT	( i + N - 1 ) % N
#define RIGHT	( i + 1 ) % N

#define THINKING
#define HUNGRY
#define EATING

//mutex	m 		initialized to 1
//int	state[N]	initiated to THINKING's
//mutex	s[N]		initialized to 0's

void grab_forks(int i);
void put_away_forks(int i);
void test(int i);
void* thread_task(void*);

int main() {
	pthread_t thread_ids[N];
	int arguments[N];
	pthread_attr_t attr;
	int s;

	s = pthread_attr_init(&attr);
	if(s != 0)
	{
		printf("Unable to init pthread attributes\n");
		exit(1);
	}
	for(int x = 0; x < N; x++) {
		thread_ids[x] = x;
		arguments[x] = x;
		
		int s = pthread_create(&thread_ids[x],&attr, 
				&thread_task,&arguments[x]); 

		if(s != 0) {
			printf("Unable to create thread %d", x);
			exit(1);
		}
	}

	s = pthread_attr_destroy(&attr);
	if(s != 0)
	{
		printf("Unable to destroy pthread attributes\n");
		exit(1);
	}

	while(1){ }
	return 0;
}

void * thread_task(void* args) {
	int* index = args;
//	while (1) {
	//	think();
		printf("Philosopher %d: Thinking\n", *index);	
		grab_forks(*index);
	//	eat();
		printf("Philosopher %d: Eating\n", *index);
		put_away_forks(*index);
//	}	
	return NULL;
}

void grab_forks( int i )
{
//	down( &m );
//		state[i] = HUNGRY;
//		test( i );
//	up( &m );
//	down( &s[i] );
}

void put_away_forks( int i )
{
//	down( &m );
//		state[i] = THINKING;
//		test( LEFT );
//		test( RIGHT );
//	up( &m );
}

void test( int i )
{
//	if( state[i] == HUNGRY
//		&& state[LEFT] != EATING
//		&& state[RIGHT] != EATING )
//	{
//		state[i] = EATING;
//		up( &s[i] );
//	}
}
