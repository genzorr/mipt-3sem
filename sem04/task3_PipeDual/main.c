#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "tools.h"

const char* filepath = "../input.txt";

//	fd[0] for read, [1] for write


int main(int argc, char **argv, char** envp)
{
	int pip1[2], pip2[2];

	if ((pipe(pip1) < 0) || (pipe(pip2) < 0))
	{
		printf("Can't open pipe\n");
		return -1;
	}

	pid_t pid = fork();
	if (pid < 0)
	{
		printf("Can't fork\n");
		return -1;
	}

	if (pid > 0)
	{
		int a = 0, b = 0;
		puts("Enter a and b");
		scanf("%d%d", &a, &b);

		close(pip1[0]);
		close(pip2[1]);

		write(pip1[1], &a, sizeof(int));
		write(pip1[1], &b, sizeof(int));

		int res = 0;
		read(pip2[0], &res, sizeof(res));
		printf("res: %d\n", res);

		close(pip1[1]);
		close(pip2[0]);
	}
	else
	{
		int a = 0, b = 0;

		close(pip1[1]);
		close(pip2[0]);

		read(pip1[0], &a, sizeof(int));
		read(pip1[0], &b, sizeof(int));

		int res = a + b;
		write(pip2[1], &res, sizeof(int));

		close(pip1[0]);
		close(pip2[1]);
	}

	return 0;
}
