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

#define FALSE 0
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


/* 
   Prints error message sent to errno 
*/
void print_error(){
  printf("Error: %s\n", strerror(errno));
}


/* 
   Reads in commands args and executes them. If they can't be executed,
   an error message is printed out 
*/
void handle_general_commands(char **args){
  if(strcmp(args[0], "cd") == 0){
    if(chdir(args[1]) < 0)
      print_error();
  } else if(strcmp(args[0], "exit") == 0){
    exit(0);	      
  } else {	
    int fork_ret = fork();

    if(fork_ret == 0) {
      execvp(args[0], args);
      exit(0);
    } else if (fork_ret > 0) {
      int status, child_pid = wait(&status);
    } else {
      print_error();
    }
  }
}


/* 
   Replaces base_fd with fd, then runs execution code of first (>\<) second, then 
   adds back base_fd and removes fd from file table. 

   base_fd can be 0 for stdin, or 1 for stdout
*/
void redirect(int *file_des, int base_fd, char **first, char *second){
  
  int fork_ret = fork();
  int fd, copy;
	
  if(fork_ret == 0){
    fd = open(second, O_CREAT | O_RDWR, 0666);	  
    copy = dup(base_fd);
    dup2(fd, base_fd);

    /* Writes to pipe, changing value copy */
    close(file_des[READ]);
    write(file_des[WRITE], &copy, sizeof(copy));

    close(fd);
    
    if(execvp(first[0], first) < 0)
      print_error();
    
    exit(0);
  } else if (fork_ret > 0){
    int status;
    wait(&status);

    /* Reads in from pipe */
    close(file_des[WRITE]);
    read(file_des[READ], &copy, sizeof(copy));
    close(file_des[READ]);
	  
    dup2(copy, base_fd);
    close(copy);
  } else {
    print_error();
  }
}


int main() {  
  char input[256];  
  char **sep_args = NULL; // semi-colon separated args
  char **args = NULL;
  char *trimmed_input = NULL;
  char *new_trimmed = NULL;
  int fork_ret;
  int num_semis;

  while(TRUE) {
    printf("<shell>");
    fgets(input, sizeof(input), stdin);
    trimmed_input = trim_string(input, strlen(input)-1); // trims out garbage in input; subtract one for \n
    
    num_semis = count_occur(trimmed_input, ";");    
    sep_args = parse_input(trimmed_input, ";"); // separates by semi-colon, double pointer

    
    int num_out_redirs, num_in_redirs, num_pipe;

    
    char *new_trimmed = NULL;

    int i;
    for(i=0; i<num_semis+1; i++){
      
      /* Individual trimmed command */
      new_trimmed = trim_trailing(sep_args[i], ' ');
      
      num_out_redirs = count_occur(new_trimmed, ">");
      num_in_redirs = count_occur(new_trimmed, "<");
      num_pipe = count_occur(new_trimmed, "|");

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

      /* Redirection */
      if(num_out_redirs > 0)
	redirect(file_des, STDOUT_FILENO, first, second);
      else if(num_in_redirs > 0)
	redirect(file_des, STDIN_FILENO, first, second);	
      else if (count_occur(new_trimmed, "|"))
	args = parse_input(trim_trailing(sep_args[i], ' '), "|");
      else
	args = parse_input(trim_trailing(sep_args[i], ' '), " ");

      if(num_out_redirs == 0 && num_in_redirs == 0 && num_pipe == 0)
	handle_general_commands(args);
      
      free(first);
      free(second);
    }
  }

  free(trimmed_input);
  free(args);
  free(sep_args);
  free(new_trimmed);
  
  return 0;
}
