#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

int cont();

char **parse_input(char *, char *);
char *trim_string(char *, int);
int count_occurrences(char *, char);
char *trim_trailing(char *, char);

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
  char *trimmed_input;
  char **sep_args; // semi-colon separated args
  char **args;
  int f;

  while(cont()) {
    printf("<shell> ");
    fgets(input, sizeof(input), stdin);
    trimmed_input = trim_string(input, strlen(input)-1); // trims out garbage in input; subtract one for \n
    
    int i, num_semis = count_occur(trimmed_input, ";");
    
    sep_args = parse_input(trimmed_input, ";"); // separates by semi-colon, double pointer    

    for(i=0; i<num_semis+1; i++){
      f = fork();

      if(f == 0){	
	args = parse_input(trim_trailing(sep_args[i], ' '), " ");	
	execvp(args[0], args);
      } else if (f > 0) {
	int status, child_pid = wait(&status);
      } else {
	// ERROR CHECK!!!
	printf("Error forking!\n");
      }
    }

  }
  
  return 0;
}

int cont(){
  return 1;
}

int get_num_phrases(char *s, char delimiter){
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

char **parse_input(char *line, char *delimiter){
  int i=0;
  char *curr_arg = NULL;
  char **ret_args = (char **)malloc(sizeof(char *) * (count_occur(line, delimiter)+2));
  
  while(curr_arg = strsep(&line, delimiter)){
    ret_args[i] = curr_arg;
    i++;
  }

  ret_args[i] = NULL;

  return ret_args;
}

/* Leaves only size characters, not including terminating null */
char *trim_string(char *s, int size) {
  int i;
  char *ret_str = (char *)malloc(size+1);
  
  ret_str[size] = '\0';
  
  for(i=0; i<size; i++)
    ret_str[i] = s[i];

  return ret_str;
}

/* Trims characters c from beginning and end of string */
char *trim_trailing(char *s, char c){
  int i, count=0, count2=0;  

  for(i=0; i<strlen(s); i++){
    if(s[i] == c)
      count++;
    else
      break;
  }

  for(i=strlen(s)-1; i>=0; i--){
    if(s[i] == c)
      count2++;
    else
      break;
  }

  char *ret_str = malloc(strlen(s)-count-count2+1);

  ret_str[strlen(ret_str)-count-count2] = '\0';

  for(i=count; i<strlen(s)-count2; i++)
    ret_str[i-count] = s[i];

  return ret_str;
}

/* Counts occurrences of substring occ in s */
int count_occur(char *s, char *occ){
  int count = 0;
  char *rem = s;

  while(rem = strstr(rem, occ)){
    count++;
    rem++;
  }

  return count;
}
