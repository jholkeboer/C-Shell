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
#include <signal.h>

#define MAX_COMMAND_SIZE 2048
#define MAX_ARGS 512

// function prototypes
void loopshell(void);
char *readline(void);
char **splitline(char *line);

// signal handlers
void catchSIGTERM(int sig) {
	printf("Caught sigterm\n");
}




int exitstatus;
int termstatus;
int bgflag;
int firstrun = 1;

pid_t deadpid;


// main function
int main(int argc, char **argv) {	
	struct sigaction act;
	act.sa_handler = SIG_IGN;
	act.sa_flags = 0;
	sigfillset(&(act.sa_mask));

	sigaction(SIGINT, &act, NULL);	
	
	
	loopshell();
	return 0;
}

// function declarations
void loopshell(void) {
	int status = 1;

	// this will carry the status of the waitpid command
	int waitstatus;

	//display prompt
	do {
		int bgstatus;
		pid_t bgwait;
		
		// check for completed background process
		bgwait = waitpid(-1,&bgstatus,WNOHANG);
		if (WIFEXITED(bgstatus) && bgwait > 0) {
			printf("Background process %d exited with status %d\n",bgwait,WEXITSTATUS(bgstatus));
		}
		
		// allocate to store input
		char *line = malloc(sizeof(char) * MAX_COMMAND_SIZE);
		char **args;
		
		// begin prompt
		printf(": ");
		line = readline();
		
		// detect blank line
		if (line[0] == '\n' || line[0] == '#') {
			// if line is blank, give a fresh shell prompt
			free(line);
			continue;
		}
		
		// detect comments
		int i;
		for (i=0; i < MAX_COMMAND_SIZE - 1; i++) {
			if (line[i] == '#') {
				line[i] = '\n';
				// terminate string after comment is detected
				line[i+1] = '\0';
			}
		}
		
		// parse input into arguments
		args = splitline(line);
		
		// these flags are set if the '>' or '<' or '&' characters are present
		int pointleftflag = 0;
		int pointrightflag = 0;
		int backgroundflag = 0;
		
		// look for '<' or '>' characters
		int argindex = 0;
		while (args[argindex] != NULL) {
			if (strcmp(args[argindex], "<") == 0) {
				pointleftflag = 1;
			}
			else if (strcmp(args[argindex], ">") == 0) {
				pointrightflag = 1;
			}
			argindex++;
		}
		
		// check for background command
		if (strcmp(args[argindex-1], "&") == 0) {
			backgroundflag = 1;
			args[argindex-1] = NULL;
		}
		
		// arrays to hold split arguments
		char **leftargs = malloc(MAX_ARGS * sizeof(char*));
		char **rightargs = malloc(MAX_ARGS * sizeof(char*));
		
		// file pointers for redirection
		FILE *outfile;
		FILE *infile;
		
		// open files if necessary
		if (pointleftflag == 1) {
			if (!(infile = fopen(args[2],"r"))) {
				perror("Error: could not open that file");
			};		
		}
		else if (pointrightflag == 1) {
			if (!(outfile = fopen(args[2],"w"))) {
				perror("Error: could not open that file");
			};		
		}		

		// detect exit command
		if (strcmp(args[0], "exit") == 0) {
			exit(0);
		}
		
		// detect cd command
		if (strcmp(args[0], "cd") == 0) {
			if (args[1] == NULL) {
				// change to home directory
				char *homedir = getenv("HOME");
				chdir(homedir);
				continue;
			}
			else {
				// change directory to argument
				chdir(args[1]);
				continue;
			}
		}
		
		// detect status command
		if (strcmp(args[0], "status") == 0) {
			if (&exitstatus != NULL) {
				printf("Last command had exit value %d\n",exitstatus);		
			} else {
				printf("No foreground processes run yet.\n");
			}
			free(args);
			free(line);
			continue;
		}
		
		// if no built in commands are recognized,
		// fork and execute the command
		
		// these variables will hold process id's returned from fork()
		pid_t childpid, wait;

		// fork the program
		childpid = fork();
		
		
		// conditional structure for child process
		if (childpid == 0) {
			struct sigaction act2;
			act2.sa_handler = SIG_DFL;
			act2.sa_flags = 0;
			sigfillset(&(act2.sa_mask));
			sigaction(SIGINT,&act2,NULL);
			
			struct sigaction act3;
			act3.sa_handler = SIG_DFL;
			act3.sa_flags = 0;
			sigfillset(&(act3.sa_mask));
			sigaction(SIGTERM,&act3,NULL);
			

			// in this case, we are in the child process
			if (backgroundflag == 1) {
				FILE *nullfile;
				nullfile = fopen("/dev/null", "w");
				dup2(fileno(nullfile),1);
				fclose(nullfile);
				if (execvp(args[0], args) == -1) {
					printf("Error: %s did not run successfully.\n", args[0]);					
				}
			}
			if (pointleftflag == 1) {
				dup2(fileno(infile),0);
				fclose(infile);
				if (execlp(args[0], args[0], NULL) == -1) {
					printf("Error: %s did not run successfully.\n", args[0]);
				};
			}
			else if (pointrightflag == 1) {
				dup2(fileno(outfile),1);
				fclose(outfile);
				if (execlp(args[0], args[0], NULL) == -1) {
					printf("Error: %s did not run successfully.\n", args[0]);
				};		
			}
			else {
				if (execvp(args[0], args) == -1 ) {
				printf("Error: %s did not run successfully.\n", args[0]);
				exit(1);
				}
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
				if (backgroundflag == 0) {
					do {
						wait = waitpid(childpid, &waitstatus, WUNTRACED);
					} while (!WIFEXITED(waitstatus) && !WIFSIGNALED(waitstatus));
					if (WIFEXITED(waitstatus)) {
						exitstatus = WEXITSTATUS(waitstatus);
					}

				}
				else if (backgroundflag == 1) {
					printf("Background pid %d\n",childpid);
				}
		}
		
		// free memory
		free(args);
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























