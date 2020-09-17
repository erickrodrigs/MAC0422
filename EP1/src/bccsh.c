#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <readline/readline.h> 
#include <readline/history.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void type_prompt() {
  char *username, cwd[616];
  username = getenv("USER");
  getcwd(cwd, sizeof(cwd));
  printf("{%s@%s}", username, cwd);
}

void read_command(char **command, char **parameters) {
  *parameters = readline(" ");
  *command = strsep(parameters, " ");
}

int main (int argc, char **argv) {
  pid_t childpid;
  char *command, *parameters;

  while (1) {
    type_prompt();
    read_command(&command, &parameters);

    if (strcmp(command, "/usr/bin/du") == 0) {
      if (strcmp(command, "-hs .") == 0) {
        // invoca aqui, não sei fazer ainda
      }
    }
    else if (strcmp(command, "/usr/bin/traceroute") == 0) {
      if (strcmp(command, "www.google.com.br") == 0) {
        // invoca aqui, não sei fazer ainda
      }
    }
    else if (strcmp(command, "./ep1") == 0) {
      // invoca aqui, não sei fazer ainda
    }
    else if (strcmp(command, "mkdir") == 0) {
      if (mkdir(parameters, 0777) == 0) {
        printf("Criado diretório %s\n", parameters);
      }
      else {
        printf("Deu ruim!\n");
      }
    }
    else if (strcmp(command, "kill") == 0) {
      char *flag = strsep(&parameters, " ");
      int signal = flag[1] - '0';
      pid_t pid = atoi(parameters);
      
      if (kill(pid, signal) == 0) {
        printf("Processo %d finalizado!\n", pid);
      }
      else {
        printf("Deu ruim!\n");
      }
    }
    else if (strcmp(command, "ln") == 0) {
      char *flag = strsep(&parameters, " ");
      char *oldname = strsep(&parameters, " ");
      char *newname = parameters;
      
      if (symlink(oldname, parameters) == 0) {
        printf("Link para %s criado com sucesso!\n", oldname);
      }
      else {
        printf("Deu ruim!\n");
      }
    }
    else {
      printf("Comando não suportado!\n");
    }

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
