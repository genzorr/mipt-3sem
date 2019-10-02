#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"

#define KILL_TIME	5
#define MAX_ARG_NUM	20


const char* filepath = "../input.txt";
const char* timeout_s = "timeout";
const char* timeout_t = "5";


int get_fileSize(const char* filepath, int* size);
char* read_plaintext(const char* filepath, int* filesize, int* nLines, int* response);
char** create_pointersToLinesArray(char* plaintext, int nlines, int* response);
int text_fillArray_removeNewLines(char* plaintext, char** text);
int get_linesNumber(char* str, int* nLines);

int Split(char* str, char* delim, char** words, int* count);


int main(int argc, char **argv, char** envp)
{
	int filesize = 0, nLines = 0, response = 0;

	//	Read data from file to one string.
	char* plaintext = read_plaintext(filepath, &filesize, &nLines, &response);
	if (response)
		return OPEN_FAIL;

	//	Create and fill pointers to lines array.
	char** text = create_pointersToLinesArray(plaintext, nLines, &response);
	if (response)
		return response;

	//	Get number of commands to be runned.
	int cmd_number = 0;
	cmd_number = atoi(text[0]);

	//	Array for keeping pointers to arguments of current command.
	char** args = (char**)calloc(MAX_ARG_NUM+1, sizeof(*args));

	for (int i = 1; i <= cmd_number; i++)
	{
		int timeout = 0;

		pid_t pid = fork();
		if (pid == 0)
		{
			memset(args, 0, MAX_ARG_NUM+1);

			//	Get timeout value
			char* space_pos = strchr(text[i], ' ');
			*space_pos = '\0';
			timeout = atoi(text[i]);

			//	Fill in args array, set 0 and 1 elements to special timeout values.
			int args_num = 0;
			args[0] = timeout_s;
			args[1] = timeout_t;
			Split(space_pos+1, " ", args+2, &args_num);

			//	Wait needed time and run command.
			sleep(timeout);
			execvp("timeout", args);
		}
	}

	int killed_number = 0, status = 0;
	for (;;)
	{
		status = 0;
		pid_t pid = waitpid(-1, &status, 0);

		//	Idea is that status of process that was killed by timeout will be not zero.
		if ((status >> 8) & 255)
		{
			red; printf("Process %d was killed by timeout\n", pid); reset_color;
		}
		else
		{
			green; printf("Process %d ended succesfully\n", pid); reset_color;
		}

		killed_number++;

		if (killed_number == cmd_number)
			break;
	}

	return 0;
}


int get_fileSize(const char* filepath, int* size)
{
	if (MY_assert(filepath) || MY_assert(size))
		return ASSERT_FAIL;

	struct stat st = {};
	stat(filepath, &st);
	*size = st.st_size;

	return OK;
}


int get_linesNumber(char* str, int* nLines)
{
	if (MY_assert(str) || MY_assert(nLines))
		return ASSERT_FAIL;

	int nChar = 0;
	char* ch = str;

	for (ch = strchr(str, '\n'); ch; ch = strchr(ch + 1, '\n'))
		nChar++;

	*nLines = (str[-1] == '\n') ? nChar : (nChar+1);

	return OK;
}



char* read_plaintext(const char* filepath, int* filesize, int* nLines, int* response)
{
	if (MY_assert(filepath) || MY_assert(filesize))	return NULL;
	if (MY_assert(nLines)	|| MY_assert(response))	return NULL;

	int error = 0;

	error = get_fileSize(filepath, filesize);
	if ((*response == ASSERT_FAIL) || error)
	{
		message(red, "# Can't find file in filesystem");
		return NULL;
	}

	FILE* file = fopen(filepath, "r");
	if (MY_assert(file != NULL))
	{
		message(red, "# Error opening file");
		return NULL;
	}

	char* raw_text = (char*)calloc(*filesize+1, sizeof(*raw_text));
	if (MY_assert(raw_text))
	{
		*response = ALLOC_FAIL;
		message(red, "# Can't allocate memory");
		return NULL;
	}

	int readed_size = fread(raw_text, sizeof(*raw_text), *filesize, file);
	if (readed_size < *filesize)
	{
		red; printf("# Only %d bytes from %d were read\n", readed_size, *filesize); reset_color;
	}

	error = fclose(file);
	if (error)
	{
		message(red, "# Error occured while file closing\n");
		return NULL;
	}

	error = get_linesNumber(raw_text, nLines);
	if (error)
		return NULL;

//	rawtext_removeGarbage(raw_text, *filesize, nlines);

	return raw_text;
}


char** create_pointersToLinesArray(char* plaintext, int nlines, int* response)
{
	if (MY_assert(plaintext) || MY_assert(response))
	{
		*response = ASSERT_FAIL;
		return NULL;
	}

	*response = 0;

	char** text = (char**)calloc(nlines+1, sizeof(*text));
	if (MY_assert(text))
	{
		*response = ASSERT_FAIL;
		message(red, "# Can't allocate memory");
		return NULL;
	}

	text[0] = plaintext;
	text[nlines] = NULL;
	if (text_fillArray_removeNewLines(plaintext, text))
		return NULL;

	return text;
}


int text_fillArray_removeNewLines(char* plaintext, char** text)
{
	if (MY_assert(plaintext) || MY_assert(text))
		return ASSERT_FAIL;

	int lines_counter = 1;
	for (char* ch = strchr(plaintext, '\n'); ch; ch = strchr(ch + 1, '\n')) {
		*ch = '\0';
		text[lines_counter++] = ch+1;
	}

	return OK;
}


//	Splits str by delim using strtok
int Split(char* str, char* delim, char** words, int* count)
{
	if (MY_assert(str) || MY_assert(delim) || MY_assert(words) || MY_assert(count))
		return ASSERT_FAIL;
	*count = 0;

	words[*count] = strtok(str, delim);

	while (words[*count] != NULL)
	{
		if (*count > MAX_ARG_NUM - 1)
		{
			puts("# Memory for words ended\n");
			return FUN_ERROR;
		}
		words[++(*count)] = strtok(NULL, delim);
	}

	return OK;
}
