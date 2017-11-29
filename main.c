#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "parse.h"

#define TRUE 1

/*

  COMPLETED) Read a line at a time, parse the line to separate the command from its arguments. 
  It should then fork and exec the command. The parent process should wait until the 
  exec'd program exits and then it should read the next command.
        
  COMPLETED
  -> Note: exit and cd cannot be run through a forked child process, you will have to implement these commands on your own.
  -> Check out the chdir() function

  COMPLETED
  2.) Read and separate multiple commands on one line with ;. That is, the line ls -l ; 
  echo hello should first run the command ls -l and then echo hello.  
    
  3.) Implement simple redirection using > (redirecting stdout) and < (redirecting stdin).
  -> This will be explained in class after Thanksgiving. 

  4.) Implement simple pipes, that is: ls | wc would run ls and use the output from ls as the input for wc. 
  -> This will be explained in class after Thanksgiving.
  -> Check out popen() for this. 

*/

int main() {  
  char input[256];
  char *trimmed_input;
  char **sep_args; // semi-colon separated args
  char **args;
  int f;

  while(TRUE) {
    printf("<shell> ");
    fgets(input, sizeof(input), stdin);
    trimmed_input = trim_string(input, strlen(input)-1); // trims out garbage in input; subtract one for \n
    
    int i;
    int num_semis = count_occur(trimmed_input, ";");
    
    
    sep_args = parse_input(trimmed_input, ";"); // separates by semi-colon, double pointer    

    for(i=0; i<num_semis+1; i++){
      int num_rights = count_occur(trimmed_input, ">");
      int num_lefts = count_occur(trimmed_input, "<");
      
      args = parse_input(trim_trailing(sep_args[i], ' '), " ");

      if(num_rights > 0){
	char *first = args[0];
	char *second = args[2];
	
	printf("First >%s< and second >%s<\n", first, second);

	f = fork();

	if(f == 0){
	  int fd = open(second, O_CREAT | O_WRONLY, 0666);
	  int fd2 = dup(fd);
	  dup2(fd,1);
	  close(1);
	  
	  /* execvp(args[0], args); run the function */
	  dup2(fd, fd2);
	  close(fd);
	  
	}else if (f > 0) {
	  int status, child_pid = wait(&status);
	}
	
      } else if(strcmp(args[0], "cd") == 0){
	int ret = chdir(args[1]);
	printf("Entered new directory >%s< !\n", args[1]);	
      } else if (strcmp(args[0], "exit") == 0){
	return 0;
      } else {	
	f = fork();

	if(f == 0){
	  execvp(args[0], args);
	} else if (f > 0) {
	  int status, child_pid = wait(&status);
	} else {
	  // ERROR CHECK!!!
	  printf("Error forking!\n");
	}
      }
    }
  }
  return 0;
}

int cont(){
  return 1;
}
