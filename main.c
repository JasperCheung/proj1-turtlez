#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <fcntl.h>

#include "parse.h"

#define TRUE 1
#define S_KEY 2017
#define S_KEY2 2018

#define READ 0
#define WRITE 1

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
    
  COMPLETED
  3.) Implement simple redirection using > (redirecting stdout) and < (redirecting stdin).  

  
  4.) Implement simple pipes, that is: ls | wc would run ls and use the output from ls as the input for wc. 
  -> This will be explained in class after Thanksgiving.
  -> Check out popen() for this. 

  ALMOST
  5.) Handle errors
  -> ERRNO
*/


void print_error(){
  printf("Error: %s\n", strerror(errno));	
}

int main() {  
  char input[256];  
  char **sep_args = NULL; // semi-colon separated args
  char *trimmed_input = NULL;  
  int fork_ret;
  int num_semis;

  while(TRUE) {
    printf("<shell>");
    fgets(input, sizeof(input), stdin);
    trimmed_input = trim_string(input, strlen(input)-1); // trims out garbage in input; subtract one for \n
    
    num_semis = count_occur(trimmed_input, ";");    
    sep_args = parse_input(trimmed_input, ";"); // separates by semi-colon, double pointer

    
    int num_out_redirs, num_in_redirs;

    char **args = NULL;
    char *new_trimmed = NULL;

    int i;
    for(i=0; i<num_semis+1; i++){
      
      /* Individual trimmed command */
      new_trimmed = trim_trailing(sep_args[i], ' ');
      num_out_redirs = count_occur(new_trimmed, ">");
      num_in_redirs = count_occur(new_trimmed, "<");

      int out_cp, in_cp;
      int fd;
      
      char **first = NULL, *second = NULL;      
      
      if(num_out_redirs > 0 || num_in_redirs > 0){
	if(num_out_redirs > 0)
	  args = parse_input(new_trimmed, ">");
	else
	  args = parse_input(new_trimmed, "<");
	
	first = parse_input(trim_trailing(args[0], ' '), " ");
	second = trim_trailing(args[1], ' ');
      } else {
	args = parse_input(new_trimmed, " ");
      }
      
      int file_des[2];
      pipe(file_des);
      
      if(num_out_redirs > 0){
	fork_ret = fork();
	
	if(fork_ret == 0){
	  fd = open(second, O_CREAT | O_WRONLY, 0666);	  
	  out_cp = dup(1);
	  dup2(fd, 1);

	  /* Writes to pipe, changing value out_cp */
	  close(file_des[READ]);
	  write(file_des[WRITE], &out_cp, sizeof(out_cp));

	  close(fd);
	  
	  execvp(first[0], first);
	} else if (fork_ret > 0){
	  int status;
	  wait(&status);

	  /* Reads in from pipe */
	  close(file_des[WRITE]);
	  read(file_des[READ], &out_cp, sizeof(out_cp));
	  close(file_des[READ]);
	  
	  dup2(out_cp, 1);
	  close(out_cp);	  ;	  
	} else {
	  print_error();
	}	
      } else if(num_in_redirs > 0){
	fork_ret = fork();
	
	if(fork_ret == 0){
	  fd = open(second, O_CREAT | O_WRONLY, 0666);
	  in_cp = dup(0);
	  dup2(fd, 0);

	  /* Writes to pipe, changing value out_cp */
	  close(file_des[READ]);
	  write(file_des[WRITE], &in_cp, sizeof(in_cp));

	  close(fd);
	  
	  execvp(first[0], first);
	} else if (fork_ret > 0){
	  int status;
	  wait(&status);

	  /* Reads in from pipe */
	  close(file_des[WRITE]);
	  read(file_des[READ], &in_cp, sizeof(in_cp));
	  close(file_des[READ]);
	  
	  dup2(in_cp, 0);
	  close(in_cp);
	} else {
	  print_error();
	}	
      } else if (count_occur(new_trimmed, "|")) {
	args = parse_input(trim_trailing(sep_args[i], ' '), "|");
      } else {
	args = parse_input(trim_trailing(sep_args[i], ' '), " ");
      }
	
      if(strcmp(args[0], "cd") == 0){
	if(chdir(args[1]) < 0)
	  print_error();
      } else if (!strcmp(args[0], "exit")){	
	return 0;
      } else {	
	fork_ret = fork();

	if(fork_ret == 0) {
	  execvp(args[0], args);
	} else if (fork_ret > 0) {
	  int status, child_pid = wait(&status);
	} else {
	  print_error();
	}
      }
      
      free(first);
      free(second);
    }

    free(new_trimmed);    
    free(args);
  }

  free(trimmed_input);
  free(sep_args);
  
  return 0;
}
