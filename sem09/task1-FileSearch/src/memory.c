#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

#include "tools.h"

#define LEN 300


void searchFile(char* file, char* currentdir, int depth)
{
	char actualdir[LEN] = {};

	DIR* dir = opendir(currentdir);
	if (dir == NULL)
	{
		printf("# Can't open dir %s, exit.\n", currentdir);
		return;
	}

	struct dirent* dir_struct = NULL;
	while((dir_struct = readdir(dir)) != NULL)
	{
		if ((strcmp(".", dir_struct->d_name) == 0) || ((strcmp("..", dir_struct->d_name)) == 0))
			continue;

		struct stat buf = {};
		stat(dir_struct->d_name, &buf);

		if (dir_struct->d_type == DT_DIR)
		{
			depth--;
			sprintf(actualdir, "%s/%s", currentdir, dir_struct->d_name);
			if (depth > 0)
				searchFile(file, actualdir, depth);
			depth++;
		}

		if (dir_struct->d_type == DT_REG)
		{
			if (strcmp(file, dir_struct->d_name) == 0)
				printf("-> %s\n", currentdir);
		}
	}

	closedir(dir);
	return;
}


int main(int argc, char **argv)
{
	if (argc != 4)
	{
		printf("# Enter valid (3) arguments number.\n");
		return -1;
	}

	char* file 		= argv[1];
	char* startdir 	= argv[2];
	int depth		= atoi(argv[3]);

	searchFile(file, startdir, depth);

	return 0;
}
