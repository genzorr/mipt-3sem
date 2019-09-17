/*
 * main.c
 *
 *  Created on: Sep 12, 2019
 *      Author: michael
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "tools.h"

#define MAX_LEN		1000
#define DELIM_LEN	10
#define MAX_WORDS	100


char* getString_alloc(int len);
int Split(char* str, char* delim, char** words, int* count);
int PrintWords(char** words, int count);
int FreeWordsMemory(char** words, int count);

//	askdjh ;aasd;;asd;asd dfffewd;dsf sdf; df;sdf

int main()
{
	char* str			= getString_alloc(MAX_LEN+1);
	char* delimeters	= getString_alloc(DELIM_LEN+1);

	if (MY_assert(str) || MY_assert(delimeters))
	{
		message(red, "# Cannot allocate memory");
		return ASSERT_FAIL;
	}

	int words_count = 0;
	char** words = (char**)calloc(MAX_WORDS, sizeof(*words));
	Split(str, delimeters, words, &words_count);

	PrintWords(words, words_count);
	FreeWordsMemory(words, words_count);

	free(str);
	free(delimeters);
	return 0;
}

//	Allocates memory for string and gets it from stdin, don't forget to free(s)
char* getString_alloc(int len)
{
	char* s = (char*)calloc(len, sizeof(*s));
	fgets(s, MAX_LEN, stdin);

	return s;
}

//	Splits str by delim using strtok, allocates memory inside
int Split(char* str, char* delim, char** words, int* count)
{
	if (MY_assert(str) || MY_assert(delim) || MY_assert(words) || MY_assert(count))
		return ASSERT_FAIL;
	*count = 0;

	words[*count] = strtok(str, delim);

	while (words[*count] != NULL)
		words[++(*count)] = strtok(NULL, delim);

	return OK;
}


int PrintWords(char** words, int count)
{
	if (MY_assert(words))
		return ASSERT_FAIL;

	for (int i = 0; i < count; i++)
		printf("%s\n", words[i]);

	return OK;
}


int FreeWordsMemory(char** words, int count)
{
	if (MY_assert(words))
		return ASSERT_FAIL;

	free(words);
	words = NULL;

	return OK;
}
