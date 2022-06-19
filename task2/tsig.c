#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

#define WITH_SIGNALS

const int NUM_CHILD = 5;
const int CHILD_SLEEP = 10; // should be 10
const int PARENT_SLEEP = 1;

int sigIngReceived = 0;

void childAlgorithm();
void parentAlgorithm();

void modifyParentSignals();
void modifyChildSignals();
void restoreParentSignals();
void sigIntHandler(int);
void sigTermHandler(int);

int main()
{
	pid_t pids[NUM_CHILD];

#ifdef WITH_SIGNALS
	modifyParentSignals();
#endif

	for (int x = 0; x < NUM_CHILD; x++)
	{
#ifdef WITH_SIGNALS
		if (sigIngReceived == 1)
		{
			printf("parent[%d]: Creation process has been stopped.\n", getpid());
			for (int y = 0; y < x; y++)
			{
				kill(pids[y], SIGTERM);
			}
			parentAlgorithm();
			restoreParentSignals();
			return 0;
		}
#endif

		int result = fork();

		if (result == 0)
		{
#ifdef WITH_SIGNALS
			modifyChildSignals();
#endif
			childAlgorithm();
			return 0;
		}
		else if (result < 0)
		{
			printf("parent[%d]: Failed to create subprocess...\n", getpid());
			// kill already created subporcesses
			for (int y = 0; y < x; y++)
			{
				kill(pids[y], SIGTERM);
			}
#ifdef WITH_SIGNALS
			restoreParentSignals();
#endif
			return 1;
		}

		// save pid of currenlty created child
		pids[x] = result;

		sleep(PARENT_SLEEP);
	}

	printf("parent[%d]: All child processes have been created.\n", getpid());
	parentAlgorithm();
#ifdef WITH_SIGNALS
	restoreParentSignals();
#endif
	return 0;
}

void childAlgorithm()
{
	printf("child[%d]: Pid of my parent is: %d\n", getpid(), getppid());
	sleep(CHILD_SLEEP);
	printf("child[%d]: Child process completed execution.\n", getpid());
}

void parentAlgorithm()
{
	int counter = 0;
	while (1)
	{
		int result = wait(NULL);
		if (result == -1)
			break;

		counter++;
	}

	printf("parent[%d]: There are no more child processes.\n", getpid());
	printf("parent[%d]: Received exit codes from the %d child processes\n", getpid(), counter);
}

void sigIntHandler(int signum)
{
	printf("parent[%d]: SIGINT signal was received.\n", getpid());
	sigIngReceived = 1;
}

void sigTermHandler(int signum)
{
	printf("child[%d]: Received SIGTERM signal. Terminating...\n", getpid());
}

void modifyParentSignals()
{
	for (int x = 1; x < NSIG; x++)
	{
		signal(x, SIG_IGN);
	}

	signal(SIGCHLD, SIG_DFL);
	signal(SIGINT, sigIntHandler);
}

void restoreParentSignals()
{
	for (int x = 1; x < NSIG; x++)
	{
		signal(x, SIG_DFL);
	}
}

void modifyChildSignals()
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, sigTermHandler);
}