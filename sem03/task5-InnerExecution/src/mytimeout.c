#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"

int main(int argc, char** argv)
{
	int time = atoi(argv[1]);

	int status = 0;
	pid_t pid = fork();

	if (pid == 0)
		execvp(argv[2], &argv[2]);
	else
	{
		struct timeval tval_before, tval_after, tval_result;
		gettimeofday(&tval_before, NULL);
		for (;;)
		{
			status = 0;
			pid_t new_pid = waitpid(pid, &status, WNOHANG);
			if (new_pid == pid)
				break;

			gettimeofday(&tval_after, NULL);
			timersub(&tval_after, &tval_before, &tval_result);

			int delta_time = ((long int)tval_result.tv_sec * 1000000 + (long int)tval_result.tv_usec);

			if (delta_time > time*1000000)
			{
				kill(pid, SIGKILL);
				return -1;
			}
		}
	}

	return 0;
}
