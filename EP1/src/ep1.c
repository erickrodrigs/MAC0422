#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define MAX 10000

typedef struct schedule {
  char name[30];
  int t0;
  int dt;
  int deadline;
} Schedule;


void * execute_thread() {
  // simular execução de processo
  int i = 0;

  while (i < 400000000) {
    i++;
  }

  return NULL;
}

void create_thread() {
  pthread_t thread;

  if (pthread_create(&thread, NULL, execute_thread, NULL)) {
    printf("\n ERROR creating thread");
    exit(1);
  }

  if (pthread_join(thread, NULL))  {
    printf("\n ERROR joining thread");
    exit(1);
  }
}

int main(int argc, char **argv) {
  char * filename;
  char * output_filename;
  int scheduler;
  FILE * file;
  FILE * output_file;
  Schedule schedules[MAX];
  clock_t start, end;
  double elapsed;
  int i = 0;

  if (argc != 4) {
    printf("USO INVÁLIDO!\n");
    exit(1);
  }

  scheduler = atoi(argv[1]);
  filename = argv[2];
  output_filename = argv[3];

  file = fopen(filename, "r");

  while (!feof(file)) {
    if (fscanf(file, "%s %d %d %d", schedules[i].name, &(schedules[i].t0), &(schedules[i].dt), &(schedules[i].deadline)) != 4) {
      continue;
    }

    i++;
  }

  fclose(file);

  start = clock();
  execute_thread();
  end = clock();
  elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
  printf("\n(%g segundos)\n", elapsed);

  return 0;
}
