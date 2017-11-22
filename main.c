#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

  printf("bash >");
  
  return 0;
}
