#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <sys/wait.h>

void type_prompt() {
  char *username, cwd[616];
  username = getenv("USER");
  getcwd(cwd, sizeof(cwd));
  printf("{%s@%s}", username, cwd);
}

void read_command(char **command) {
  char *line = readline(" ");
  *command = strsep(&line, " ");
  printf("linha: %s\n", line);
  printf("comando: %s\n", *command);
}


int main (int argc, char **argv) {
  pid_t childpid;
  char *command;

  while (1) {
    type_prompt();
    read_command(&command);
    printf("%s\n", command);

    /*if ( (childpid = fork()) == 0) {
      printf("[Sou o processo filho]\n");
      while (1) {
        sleep(1);
        printf("Primeiro processo filho...\n");
      }
    }
    else {
      printf("[Sou o pai. Criei o %d]\n",childpid);
      //sleep(3);
      waitpid(-1, NULL, 0);
    }
    */
  }
  exit(0);
}