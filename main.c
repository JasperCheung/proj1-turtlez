#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "parse.h"

#define FALSE 0
#define TRUE 1
#define S_KEY 2017
#define S_KEY2 2018
#define READ 0
#define WRITE 1

void print_error();
int handle_general_commands(char *);
void redirect(int, int, char *, char *);
void redirect_exec(int, char *);

/*
  TASKS

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

int main() {  
  char input[256];  
  char **sep_args = NULL; // semi-colon separated args
  char **args = NULL;    
  int fork_ret;
  int num_semis;

  while(TRUE) {
    printf("<shell>");
    fgets(input, sizeof(input), stdin);
    char *trimmed_input = trim_string(input, strlen(input)-1); // trims out garbage in input; subtract one for \n
    
    num_semis = count_occur(trimmed_input, ";");
    sep_args = parse_input(trimmed_input, ";"); // separates by semi-colon, double pointer
    
    int num_out_redirs, num_in_redirs, num_pipe, num_echo;    

    int i;
    for(i=0; i<num_semis+1; i++){
      
      /* Individual trimmed command */
      char *new_trimmed = trim_trailing(sep_args[i], ' ');
      
      num_out_redirs = count_occur(new_trimmed, ">");
      num_in_redirs = count_occur(new_trimmed, "<");
      num_pipe = count_occur(new_trimmed, "|");      

      int out_cp, in_cp;
      
      char *first = NULL, *second = NULL;
      
      if(num_out_redirs || num_in_redirs || num_pipe){
	if(num_out_redirs > 0)
	  args = parse_input(new_trimmed, ">");
	else if(num_in_redirs > 0)
	  args = parse_input(new_trimmed, "<");
	else if(num_pipe > 0)
	  args = parse_input(new_trimmed, "|");
	
	first = trim_trailing(args[0], ' ');
	second = trim_trailing(args[1], ' ');
      }

      /* Redirection */
      if(num_out_redirs) {
	int fd = open(second, O_CREAT | O_WRONLY, 0666);	  	
	redirect(STDOUT_FILENO, fd, first, second);
	close(fd);
      } else if(num_in_redirs) {
	int fd = open(second, O_CREAT | O_RDONLY, 0666);
	redirect(STDIN_FILENO, fd, first, second);
	close(fd);
      } else if(num_pipe) {
	FILE *f_stream = popen(second, "w");
	redirect_exec(fileno(f_stream), first);	
	pclose(f_stream);
      } else {
	handle_general_commands(new_trimmed);
      }
      
      free(first);
      free(second);
      free(new_trimmed);
    }
    
    free(trimmed_input);    
  }

  free(args);
  free(sep_args);  
  
  return 0;
}


/* 
   Prints error message sent to errno 
*/
void print_error(){
  printf("Error: %s\n", strerror(errno));
}


/* 
   Reads in a string command and executes it. If it can't be executed,
   an error message is printed out. Also handles commands that can't be
   execvp'd directly. If "parent" command or fork fails, returns -1. 
   If child fails, exits immediately.
*/
int handle_general_commands(char *command){
  
  /* Creates copy of initial command string for custom command cases */
  char cmd_cp[strlen(command)+1];
  strcpy(cmd_cp, command);

  /* Parses by space */
  char **args = parse_input(command, " ");
  
  if(strcmp(args[0], "cd") == 0){
    if(chdir(args[1]) < 0)
      print_error();
    free(args);
    return -1;
  } else if(strcmp(args[0], "exit") == 0){
    exit(0);
  } else {
    int fork_ret = fork();

    if(fork_ret == 0) {      
      if (strcmp(args[0], "echo") == 0){
	if(count_occur(cmd_cp, "\""))
	  args = parse_input(cmd_cp, "\"");
	execvp("echo", args);
      }      
      
      if(strcmp(args[0], "") != 0 && execvp(args[0], args) < 0)
	print_error();      
      exit(0);      
    } else if (fork_ret > 0) {
      int status, child_pid = wait(&status);
      free(args);
    } else {
      print_error();
      free(args);
      return -1;
    }
  }
}


/* 
   Redirects output of command first to command second. 
   File descriptor fd represents entry of command second. 
   base_fd is the placeholdered file descriptor which command
   first will output to.

   base_fd can be 0 for stdin, or 1 for stdout
*/
void redirect(int base_fd, int fd, char *first, char *second){  
  int copy, fork_ret;
  int file_des[2];
  
  pipe(file_des);
  fork_ret = fork();
  
  if(fork_ret == 0){    
    copy = dup(base_fd);
    dup2(fd, base_fd);

    /* Writes to pipe, changing value copy */
    close(file_des[READ]);
    write(file_des[WRITE], &copy, sizeof(copy));

    close(fd);
    
    if(handle_general_commands(first) < 0)
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


/*
  Executes command first, then redirects its stdout to base_fd.
  Reverts back to original file table when finished.
*/
void redirect_exec(int base_fd, char *first){
  int copy;
  
  int file_des[2];
  pipe(file_des);

  int fork_ret = fork();
	
  if(fork_ret == 0){
    copy = dup(STDOUT_FILENO);
    dup2(base_fd, STDOUT_FILENO);

    /* Writes to pipe, changing value copy */
    close(file_des[READ]);
    write(file_des[WRITE], &copy, sizeof(copy));
    close(file_des[WRITE]);    
    
    if(handle_general_commands(first) < 0)
      print_error();
    
    exit(0);
  } else if (fork_ret > 0){
    int status;
    wait(&status);

    /* Reads in from pipe */
    close(file_des[WRITE]);
    read(file_des[READ], &copy, sizeof(copy));
    close(file_des[READ]);
	  
    dup2(copy, STDOUT_FILENO);
    close(copy);
  } else {
    print_error();
  }
}
