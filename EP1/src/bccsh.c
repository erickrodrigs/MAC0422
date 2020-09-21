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

void execute_bin(char *path, char *parameters) {
  pid_t child;
  int i = 1, j, pos = -1;
  char *args[30];

  args[0] = path;

  for (j = 0; path[j] != '\0' ; j++)
    if (path[j] == '/')
      pos = j;
  pos++;
  args[0] = &path[pos];

  while (parameters != NULL) {
    args[i] = strdup(strsep(&parameters, " "));
    i++;
  }
  args[i] = NULL;

  if ((child = fork()) == 0) {
    execvp(path, args);
  }
  else {
    waitpid(-1, NULL, 0);
  }
}

int execute_command(char *command, char *parameters) {
  char *flag, *oldname, *newname;
  int executing = 1;

  if (strcmp(command, "mkdir") == 0) {
    if (mkdir(parameters, 0755) == 0) {
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
    execute_bin(command, parameters);
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
