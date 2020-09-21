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

void type_prompt(char * fullprompt) {
  char *username, cwd[1024];
  username = getenv("USER");
  getcwd(cwd, sizeof(cwd));

  strcat(fullprompt, username);
  strcat(fullprompt, "@");
  strcat(fullprompt, cwd);
  strcat(fullprompt, "} ");
}

void read_command(char **command, char **parameters) {
  char fullprompt[1024] = "{";
  char *line;

  type_prompt(fullprompt);
  line = readline(fullprompt);

  add_history(line);

  *command = strsep(&line, " ");
  *parameters = line;
}

int execute_command(char *command, char *parameters) {
  char *flag, *oldname, *newname;
  int executing = 1;

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
    int signal;
    pid_t pid;

    flag = strsep(&parameters, " ");
    signal = flag[1] - '0';
    pid = atoi(parameters);

    printf("Flag: %s\n", flag);
    printf("Signal: %d\n", signal);
    printf("PID: %d\n", pid);
    
    if (kill(pid, signal) == 0) {
      printf("Processo %d finalizado!\n", pid);
    }
    else {
      printf("Deu ruim!\n");
    }
  }
  else if (strcmp(command, "ln") == 0) {
    flag = strsep(&parameters, " ");
    oldname = strsep(&parameters, " ");
    newname = parameters;

    printf("Flag: %s\n", flag);
    printf("Oldname: %s\n", oldname);
    printf("Newname: %s\n", newname);
    
    if (symlink(oldname, newname) == 0) {
      printf("Link para %s criado com sucesso!\n", oldname);
    }
    else {
      printf("Deu ruim!\n");
    }
  }
  else if (strcmp(command, "exit") == 0) {
    executing = 0;
    printf("Bye bye!\n");
  }
  else {
    printf("Comando não suportado!\n");
  }

  return executing;
}

int main(int argc, char **argv) {
  char *command, *parameters;
  int executing = 1;

  while (executing) {
    command = NULL;
    parameters = NULL;

    read_command(&command, &parameters);
    executing = execute_command(command, parameters);

    if (command != NULL)
      free(command);
  }

  exit(0);
}
