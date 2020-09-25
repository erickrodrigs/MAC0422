#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define MAX 10000

pthread_t currentThread;
int current;

typedef struct schedule {
  char name[30];
  int t0;
  int dt;
  int tf;
  int deadline;
} Schedule;

Schedule schedules[MAX];
int size;
time_t start;

void * execute_thread() {
  while (schedules[current].dt > 0){
    sleep(1);
    schedules[current].dt--;
  }
  return NULL;
}

void roundRobin() {
  current = 0;
}

void srtn() {
  int i, j;
  current = 0;

  for (i = 0; i < size; i++) {
    if (current < schedules[i].t0) {
      sleep(schedules[i].t0 - current);
      current = schedules[i].t0;
    }
    for (j = i; j < size && schedules[j].t0 == current; j++)
      /*insertheap*/;

  }

}

void fcfs() {
  int i = 0;
  start = time(NULL);// "Nosso '0'"

  for (i = 0; i < size; i++) {
    current = i;
    while (difftime(time(NULL), start) < schedules[i].t0);
    printf("%s vai rodar por %d segundos!!\n", schedules[i].name, schedules[i].dt);
    if (pthread_create(&currentThread, NULL, execute_thread, NULL)) {
      printf("\n ERROR creating thread");
      exit(1);
    }

    if (pthread_join(currentThread, NULL))  {
      printf("\n ERROR joining thread");
      exit(1);
    }
    schedules[i].tf = difftime(time(NULL),start);
    printf("tf = %d\n", schedules[i].tf);
  }
}

int main(int argc, char **argv) {
  char * filename;
  char * output_filename;
  int scheduler;
  FILE * file;
  FILE * output_file;  
  size = 0;

  if (argc != 4) {
    printf("USO INVÁLIDO!\n");
    exit(1);
  }

  scheduler = atoi(argv[1]);
  filename = argv[2];
  output_filename = argv[3];

  file = fopen(filename, "r");

  while (!feof(file)) {
    if (fscanf(file, "%s %d %d %d", schedules[size].name, &(schedules[size].t0), &(schedules[size].dt), &(schedules[size].deadline)) != 4) {
      continue;
    }

    size++;
  }

  fclose(file);
 
  switch(scheduler) {
    case 1:
      fcfs();
      break;
    case 2:
      srtn();
      break;
    case 3:
      roundRobin();
    default:
      printf("Escalonador inválido!\n");
      exit(1);
      break;
  } 

  return 0;
}
