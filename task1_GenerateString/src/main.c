#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tools.h"

#define MAX_LEN	1000


void GenerateString(int n, char* string);


int main()
{
	int n = 0;
	char* string = (char*)calloc(MAX_LEN, sizeof(*string));
	if (MY_assert(string))
	{
		printf("# Can't allocate memory\n");
		return ASSERT_FAIL;
	}

	int scanned = scanf("%d", &n);
	if (scanned != 1)
	{
		printf("# Can't scan string length, try again\n");
		return -1;
	}

	GenerateString(n, string);

	printf("%s\n", string);

	return 0;
}


void GenerateString(int n, char* string)
{
	int new_size = 0;
	char current_char = 'a';
	for (int i = 1; i < n+1; i++)
	{
		string[(1 << (i-1)) - 1] = current_char++;
		strncpy((char*)&string[new_size+1], string, new_size);

		new_size = (1 << i) - 1;
	}
	return;
}
