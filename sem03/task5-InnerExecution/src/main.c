#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

#include "tools.h"

const char* filepath = "../input.txt";

int get_fileSize(const char* filepath, int* size);
char* read_plaintext(const char* filepath, int* filesize, int* nLines, int* response);
char** create_pointersToLinesArray(char* plaintext, int nlines, int* response);
int text_fillArray_removeNewLines(char* plaintext, char** text);
int get_linesNumber(char* str, int* nLines);


int main(int argc, char **argv, char** envp)
{
	int filesize = 0, nLines = 0, response = 0;
	char* plaintext = read_plaintext(filepath, &filesize, &nLines, &response);
	if (response)
		return OPEN_FAIL;

	char** text = create_pointersToLinesArray(plaintext, nLines, &response);
	if (response)
		return response;

	int cmd_number = 0;
	cmd_number = atoi(text[0]);

	for (int i = 1; i < cmd_number+1; i++)
	{
		//	fork
		int cmd_len = 0, params_len = 0;
		char* space_pos = strchr(text[i], ' ');

		if (space_pos != NULL)
		{
			*space_pos = '\0';
			printf("cmd: %s", text[i]);
			printf(", params: %s", space_pos+1);
			cmd_len = (int)(space_pos - text[i]);
			params_len = (int)(text[i+1] - text[i]) - cmd_len - 2;
		}
		else
		{
			printf("cmd: %s", text[i]);
			cmd_len = (int)(text[i+1] - text[i]) - 1;
		}

		printf("\ncmd len: %d, params len: %d\n\n", cmd_len, params_len);
	}


//	int n = 5;
//
//	pid_t PPID = getpid();
//	blue; printf("# Parent process id: %d\n", PPID); reset_color;
//
//	for (int i = 0; i < n; i++)
//	{
//		pid_t new_pid = fork();		//	Fork in parent
//
//		if (new_pid == 0)			//	We are IN child
//		{
//			green; printf("# Parent %d created child %d\n", getppid(), getpid()); reset_color;
//		}
//		else
//		{
//			int status = 0;
//			pid_t termin_child_pid = wait(&status);
//			pid_t ppid = getpid();
//
//			if (ppid == PPID)
//				blue;
//			else
//				yellow;
//
//			printf("# Parent %d terminated child %d status: %d\n",
//					ppid, termin_child_pid, (status >> 8) & 255);
//			reset_color;
//			exit(42);
//		}
//	}

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
