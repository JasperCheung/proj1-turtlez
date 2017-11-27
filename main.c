#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int cont();
char **parse_input(char *);
int get_num_phrases(char *);
char *trim_string(char *, int);

/*

  COMPLETED) Read a line at a time, parse the line to separate the command from its arguments. 
  It should then fork and exec the command. The parent process should wait until the 
  exec'd program exits and then it should read the next command.
        
  WILL BE DONE
  -> Note: exit and cd cannot be run through a forked child process, you will have to implement these commands on your own.
  -> Check out the chdir() function

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

  while(cont()) {
    printf("<shell> ");
    fgets(input, sizeof(input), stdin);

    int f = fork();

    if(f == 0){
      char **args = parse_input(trim_string(input, strlen(input)));;
      execvp(args[0], args);
    } else {
      int status, child_pid = wait(&status);
    }
  }
  
  return 0;
}

int cont(){
  return 1;
}

int get_num_phrases(char *s){
  char valid_phrase = 0;
  int i, num_phrases = 0;
  
  for(i=0; i<strlen(s); i++){
    if(s[i] == ' ' && valid_phrase) {
      num_phrases++;
      valid_phrase = 0;
    } else if (s[i] != ' '){
      valid_phrase = 1;
    }
  }

  if(s[strlen(s)-1] != ' ')
    num_phrases++; // account for last phrase

  return num_phrases;
}

char **parse_input(char *line){
  int i=0;
  char *curr_arg = NULL;
  char **ret_args = (char **)malloc(sizeof(char *) * (get_num_phrases(line)+1));
  
  while(curr_arg = strsep(&line, " ")){
    ret_args[i] = curr_arg;    
    i++;
  }

  ret_args[i] = NULL;

  return ret_args;
}

char *trim_string(char *s, int size) {
  int i;
  char *ret_str = (char *)malloc(size+1);
  
  ret_str[size-1] = '\0';
  
  for(i=0; i<size-1; i++)
    ret_str[i] = s[i];

  return ret_str;
}
