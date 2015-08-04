// Jack Holkeboer
// Oregon State CS344
// August 3rd, 2015
// holkeboj@onid.oregonstate.edu
// Program 3: smallsh

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_COMMAND_SIZE 2048
#define MAX_ARGS 512

// function prototypes
void loopshell(void);
char *readline(void);
char **splitline(char *line);

// main function
int main(int argc, char **argv) {
	loopshell();
	return 0;
}

// function declarations
void loopshell(void) {
	int status = 1;
	
	//display prompt
	do {
		char *line = malloc(sizeof(char) * MAX_COMMAND_SIZE);
		char **args;
		printf(": ");

		line = readline();
		printf("Line = %s",line);
		int i;
		
		// detect comments
		for (i=0; i < MAX_COMMAND_SIZE - 1; i++) {
			if (line[i] == '#') {
				line[i] = '\n';
				// terminate string after comment is detected
				line[i+1] = '\0';
			}
		}
		printf("Parsed line = %s",line);
		args = splitline(line);

		// detect exit command
		if (strcmp(args[0], "exit") == 0) {
			exit(0);
		}
		
		// detect cd command
		
		
		// detect status command
		
		
		// if no built in commands are recognized,
		// fork and execute the command
		
		// these variables will hold process id's returned from fork()
		pid_t childpid, wait;
		
		// this will carry the status of the wait command
		int waitstatus;
		
		// fork the program
		childpid = fork();
		
		// conditional structure for child process
		if (childpid == 0) {
			// in this case, we are in the child process
			if (execvp(args[0], args) == -1 ) {
				printf("Error: %s did not run successfully.\n", args[0]);
			}
			// this will only exit if child process did not run
			exit(0);
		}
		// condition for unsuccessful fork
		else if (childpid < 0) {
			perror("Error: unable to fork.");
			exit(0);
		}
		// if we reach this condition, we are in the parent process.
		else {
			do {
				wait = waitpid(childpid, &waitstatus, WUNTRACED);
			} while (!WIFEXITED(waitstatus) && !WIFSIGNALED(waitstatus));
		}
		
// 		int forkid = fork();
//  		if (forkid == 0) {
//  			execlp(line, line, NULL);
//  		}

		// free memory for strings
		free(line);
	} while (status);
}

char *readline(void) {
	char *buffer = NULL;
	ssize_t buffersize = MAX_COMMAND_SIZE;
	getline(&buffer, &buffersize, stdin);
	return buffer;
}

char **splitline(char *line) {
	int index = 0;
	char* nexttoken;
	char **tokenargs = malloc(MAX_ARGS * sizeof(char*));
	
	// Tokenize the input line using the strtok() function
	// delimit by space or newline
	nexttoken = strtok(line, " \n");
	while (nexttoken != NULL) {
		tokenargs[index] = nexttoken;
		index++;
		nexttoken = strtok(NULL, " \n");
	}
	
	// terminate the argument with a NULL value
	// so it can be passed to exec()
	tokenargs[index] = NULL;
	
	// return array of arguments
	return tokenargs;
}























