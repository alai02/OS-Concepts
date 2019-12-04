/* ************************************************************************** */
/*   my_shell.c                                                               */
/*   Description: This program is a shell implementation, 					  */
/* 		it supports many system calls as well as some extra commands  		  */
/*   Author: alex lai                                                         */
/*   Created: 2017/1/30                                                       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

/*Get data from lex*/

extern char **getln();

/*  Function Declarations */

int countCommand(char **args);

int cdCommand(char **args);

int myCommand(char **args);

int exitCommand(char **args);

int addCommand(char **args);



/*This function prints out the amount of arguments*/
int countCommand(char ** args) {

	char* token;
	int i = 0;
	
	while(args[i]){
	
	  token = strtok(args[i], " ");
	
	  while(token != NULL){
	
		token = strtok(NULL, " ");
		i++;
	
	  }
	
	}


	int j;
	
	printf("Output: argc = %d, args = ", i-1);
	
	for(j = 1; j < i; j++) {
	
		if(j == (i -1) ){
			printf("%s ", args[j]);
		} else {
			printf("%s, ", args[j]);
		}
	}
	
	printf("\n");
	
	return 1;
}


/* This function changes the working directory */
int cdCommand(char **args) {

  if (args[1] == NULL) {

	  printf("Enter a directory after cd command\n");
  } else {

	  if (chdir(args[1]) != 0) { /*change into working directory*/
		  printf("ERROR changing into the directory\n");
	  }
  }

  return 1;
}


/* Write a fun function*/
int myCommand(char **args){
	printf("       88888888\n");
	printf("     88        88\n");
	printf("    88          88\n");
	printf("  88              88\n");
	printf(" 88  ----+  +----   88\n");
	printf("888   +-+ || +-+    888\n");
	printf("888  + O ||||  O +  888\n");
	printf(" 88  +   +||+    +  88\n");
	printf(" 88      (..)       88\n");
	printf("  88      ||       88\n");
	printf("    88  (----)   88\n");
	printf("     88   --   88\n");
	printf("       88888888\n");
	return 1;
}


/* This command exits my shell program*/
int exitCommand(char **args){
  return 0;
}


/* This function adds the arguments that are either hex or decimal*/
int addCommand(char **args) {

	char* token;
	int i = 0, j = 0, k = 0;
	int sum = 0;
	int isANum = 0;
	long int hexNum;


	while(args[i]){
	  token = strtok(args[i], " ");
	  while(token != NULL){
		token = strtok(NULL, " ");
		i++;
	  }
	}


	for(j = 0; j < i; j++) {

		for(k = 0; k < strlen(args[j]); k++) {

			isANum = 0;
			if(isdigit(args[j][k]) ){
				isANum = 1;
			}
		}

		if ( isANum == 1) {

			if(args[j][1] == 'x'){
				hexNum = strtol(args[j], NULL, 16);
				sum += hexNum;
			} else {
				sum += atoi(args[j]);
			}
		}
	}
	printf("Output: ");
	for(j = 1; j < i; j++) {

		if(j == (i -1) ){
			printf("%s ", args[j]);
		} else {
			printf("%s + ", args[j]);
		}
	}

	printf("= %d\n", sum);
	return 1;
}
/* This function uses the command & to  */
int runAndContinue(char ** args){

	int checker;
	pid_t processID;
	processID = fork();
	
	if(processID == 0) {
		if (execvp(args[0], args) == -1) {
			printf("An error occured in the child process\n");
		}
	} else {
		//Parent does work here
		if(!strcmp(args[1],"&") == 0){
			waitpid(processID, &checker, 0);
		}
	}
	return 1;
}
/*constructor for Function pointers that link the commands*/
int ( * myFunctions[]) ( char ** ) = {
  &cdCommand,
  &exitCommand,
  &addCommand,
  &countCommand,
  &myCommand,
  &runAndContinue
};


/*Starts a process and waits for it to finish, return 1 to continue */
int startChildProcess(char **args) {

	int checker;
	pid_t processID;
	pid_t waitProcessID;
	processID = fork();
	
	switch(processID){
		case 0:
			if (execvp(args[0], args) == -1) {
				printf("An error occured in the child process\n");
			}
			exit(1);
			break;
		case -1:
			printf("ERROR: there was an problem forking\n");
			break;
		default:
			do { /*wait for child process to exit*/
				waitProcessID = waitpid(processID, &checker, 2);
			} while (!WIFEXITED(checker) && !WIFSIGNALED(checker)); /*make sure exit success*/
	}

	waitProcessID++;
	return 1;
}

int executeCommand(char **args) {

	int i, c = 0, argc = 0;
	char** newArgs = malloc(1000);

	if (args[0] == NULL) {
		return 1;
	}

	for(i = 0; args[i] != NULL; i++){
		argc++;
	}

	if (strcmp(args[0], "cd") == 0) {
		return (*cdCommand)(args);
	} else if(strcmp(args[0], "exit") == 0) {
		return (*exitCommand)(args);
	} else if (strcmp(args[0], "add") == 0) {
		return (*addCommand)(args);
	} else if(strcmp(args[0], "arg") == 0) {
		return (*countCommand)(args);
	} else if (strcmp(args[0], "face") == 0){
		return (*myCommand)(args);
	} else if (argc > 1) {

		if (strcmp(args[1], "&") == 0){
			return (*runAndContinue)(args);
		} else if(strcmp(args[1], "<") == 0 || strcmp(args[1], ">") == 0) {
		
			for(i = 0; i < argc; i++){
				if(strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0){
					/*Do nothing */
				} else {
					newArgs[c] = malloc(1000);
					newArgs[c] =  args[i];
					c++;
				}
			}
			printf("after\n" );
			for(i = 0; newArgs[i] != NULL; i++){
				printf("Data> %s\n", newArgs[i]);
			}
			return startChildProcess(newArgs);
		}
	}

	return startChildProcess(args); /*Not one of my commands so system calls*/
}

/* Shell command loop*/

void myShellLoop() {

	int checker;
	char **args;

	do {
		
		printf("> ");
		args = getln();
		checker = executeCommand(args);

	} while (checker == 1);

}

int main() {

	myShellLoop();
	
	return 0;

}
