#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"


int main(int argc, char **argv, char** envp)
{
	int n = 5;

	pid_t PPID = getpid();
	blue; printf("# Parent process id: %d\n", PPID); reset_color;

	for (int i = 0; i < n; i++)
	{
		pid_t new_pid = fork();		//	Fork in parent

		if (new_pid == 0)			//	We are IN child
		{
			green; printf("# Parent %d created child %d\n", getppid(), getpid()); reset_color;
		}
		else
		{
			int status = 0;
			pid_t termin_child_pid = wait(&status);
			pid_t ppid = getpid();

			if (ppid == PPID)
				blue;
			else
				yellow;

			printf("# Parent %d terminated child %d status: %d\n",
					ppid, termin_child_pid, (status >> 8) & 255);
			reset_color;
			exit(42);
		}
	}

	return 0;
}
