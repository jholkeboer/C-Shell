// Jack Holkeboer
// Oregon State CS344
// August 3st, 2015
// holkeboj@onid.oregonstate.edu
// Program 3: smallsh

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define MAX_COMMAND_SIZE 2048
#define MAX_ARGS 512

// function prototypes
void loopshell(void);
char *readline(void);

int main(int argc, char **argv) {
	loopshell();
	return 0;
}

// function declarations
void loopshell(void) {
	char *line = malloc(sizeof(char) * MAX_COMMAND_SIZE);
	int status = 1;
	
	//display prompt
	do {
		printf(": ");
		line = readline();
		printf("%s", line);
		free(line);
		status = 0;
	} while (status);
}

char *readline(void) {
// 	int buffersize = MAX_COMMAND_SIZE;
// 	int index = 0;
// 	int nextchar;
// 	
// 	// allocate memory for buffer
// 	char *buffer = malloc(sizeof(char) * buffersize);
// 	
// 	// allocation error check
// 	if (!buffer) {
// 		printf("Error: Buffer was not allocated.\n");
// 		exit(EXIT_FAILURE);
// 	}
// 	
// 	// read characters until EOF
// 	nextchar = getchar();
// 	if (nextchar == EOF || nextchar == '\n') {
// 		// if we've reached the end of the file or an newline,
// 		// terminate the string
// 		buffer[index] = '\0';
// 		return buffer;
// 	} else {
// 		buffer[index] = nextchar;
// 	}
// 	index++;
// 	
// 	if (index > buffersize) {
// 		printf("Error: Your command was too long.\n");
// 		exit(EXIT_FAILURE);
// 	}
	char *buffer = NULL;
	ssize_t buffersize = MAX_COMMAND_SIZE;
	getline(&buffer, &buffersize, stdin);
	return buffer;
}