// Jack Holkeboer
// Oregon State CS344
// August 3st, 2015
// holkeboj@onid.oregonstate.edu
// Program 3: smallsh

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
	int waitstatus;
	
	//display prompt
	do {
		printf(": ");
		fflush(stdin);
		line = readline();

		printf("Line %s\n",line);

		if (strcmp(line, "exit") == 0) {
			exit(0);
		}
		int forkid = fork();
 		if (forkid == 0) {
 			execlp(line, line, NULL);
 		}

		printf("Running command %s:\n", line);
// 		pid_t child;
// 		child = waitpid(-1,&waitstatus);
// 		printf("%d", waitstatus);
		free(line);
// 		status = 0;
	} while (status);
}

char *readline(void) {
	char *buffer = malloc(sizeof(char) * MAX_COMMAND_SIZE);
	int index = 0;
	int nextchar;
	
	if (!buffer) {
		fprintf(stderr, "readline buffer not allocated.\n");
		exit(1);
	}
	
	while (1) {
		nextchar = getchar();
		if (nextchar == EOF || nextchar == '\n' || nextchar == '#') {
			// this condition decides when to stop collecting input.
			// newlines would be the end of the command.
			// the # will ignore comments
			buffer[index] = '\0';	// terminate the string
			while ((nextchar = getchar()) != '\n' && nextchar != EOF);
			return buffer;
		} else {
			buffer[index] = nextchar;
		}
		index++;
		
		if (index > MAX_COMMAND_SIZE) {
			fprintf(stderr, "Error: command too long. Max length 2048 chars.");
			exit(1);
		}
	}

// 	char *buffer = NULL;
// 	ssize_t buffersize = MAX_COMMAND_SIZE;
// 	getline(&buffer, &buffersize, stdin);
// 	return buffer;
}



























