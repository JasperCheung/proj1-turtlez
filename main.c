#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char **parse_args(char *);
int get_num_phrases(char *);



int main() {
  /*
    char *args[3] = {"ls", "-l"};
    args[2] = '\0';
  
    int f = fork();  
  
    if(f == 0) {
    execvp(args[0], args);
    }

    if(f > 0)
    printf("This is parent!\n");
  */

  char args[256];

  while(strcmp(args, "quit") != 0) {
    printf("<shell> ");
    fgets(args, 256, stdin);
    printf("Command: %s", args);
  }
  
  return 0;
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

char **parse_args(char *line){
  int i=0;
  char *curr_arg = NULL;
  char **ret_args = (char **)malloc(sizeof(char *) * (get_num_phrases(line)+1));
  
  while(curr_arg = strsep(&line, " ")){
    ret_args[i] = (char *)malloc(strlen(curr_arg)+1);
    strcpy(ret_args[i], curr_arg);
    i++;
  }

  ret_args[i] = NULL;

  return ret_args;
}
