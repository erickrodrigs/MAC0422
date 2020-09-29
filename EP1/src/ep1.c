#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "heap.h"
#include "queue.h"

pthread_t currentThread;
pthread_mutex_t sem[MAX];
pthread_mutex_t empty[MAX];
pthread_mutex_t full[MAX];
Schedule schedules[MAX];
int size;
time_t start;
int scheduler;
long int execTime;

void * execute_thread(void * current) {
  int curr = *((int *) current);
  int i = 0;

  if (scheduler != 1) {
    while (schedules[curr].dt > 0 || schedules[curr].udt > 0) {
      pthread_mutex_lock(&sem[curr]);
      pthread_mutex_lock(&empty[curr]);
      usleep(execTime);
      if (execTime > schedules[curr].udt) {
        schedules[curr].dt--;
        schedules[curr].udt += 1e6 - execTime;
      }
      else {
        schedules[curr].udt -= execTime;
      }
      i = 2 * 2;
      
      printf("DT de %s AGORA VALE %d\n", schedules[curr].name, schedules[curr].dt);
      pthread_mutex_unlock(&sem[curr]);
      pthread_mutex_unlock(&full[curr]);
    }
  }
  else {
    pthread_mutex_lock(&sem[curr]);
    while (schedules[curr].dt > 0) {
      sleep(1);
      schedules[curr].dt--;
      i = 2 * 2;
    }
  }
  return NULL;
}

void roundRobin() {
  int i, j;
  int current;
  int executing = 0;
  int size_queue = 0;
  long int t = 0;
  Queue queue;

  for (i = 0; i < size; i++) {
    pthread_mutex_init(&sem[i], NULL);
    pthread_mutex_lock(&sem[i]);

    pthread_mutex_init(&empty[i], NULL);
    pthread_mutex_init(&full[i], NULL);
    pthread_mutex_lock(&full[i]);
  }

  for (i = 0; i < size; i++) {
    if (pthread_create(&schedules[i].thread, NULL, execute_thread, (void *) &schedules[i].index)) {
      printf("\n ERROR creating thread");
      exit(1);
    }
  }

  start = time(NULL);
  i = 0;
  queue_init(&queue);

  while (1) {
    if ((int) difftime(time(NULL), start) == t /*Se for igual nosso próximo quantum*/) {
      for (j = i; j < size && schedules[j].t0 == t; j++) {
        //insere na nossa estrutura de dado
        printf("%s ENTROU NA FILA\n", schedules[j].name);
      }
    
      if (executing) {
        pthread_mutex_lock(&full[current]);

        if (schedules[current].dt == 0) {
          printf("PROCESSO %s terminou\n", schedules[current].name);
          executing = 0;
          pthread_cancel(schedules[current].thread);
          schedules[i].tf = difftime(time(NULL),start);

          if (!is_empty(&queue)) {
            current = remove_queue(&queue).index;
            printf("%s VAI EXECUTAR...\n", schedules[current].name);
            pthread_mutex_unlock(&sem[current]);
          }
        }
        else if (!is_empty(&queue)) {
          int previous = current;
          current = remove_queue(&queue).index;

          printf("%s VAI SER SUBSTITUÍDO POR %s\n", schedules[previous].name, schedules[current].name);
          insert_queue(&queue, schedules[previous]);

          printf("%s VAI EXECUTAR...\n", schedules[current].name);
          pthread_mutex_unlock(&sem[current]);
        }

        pthread_mutex_unlock(&empty[current]);
      }
      else {
        if (!is_empty(&queue)) {
          current = remove_queue(&queue).index;
          printf("%s VAI EXECUTAR E NAO TINHA NADA EXECUTANDO...\n", schedules[current].name);
          pthread_mutex_unlock(&sem[current]);
          executing = 1;
        }
        else if (i == size) break;
      }
    }
  }
}

void srtn() {
  int i, j;
  Schedule heap[MAX];
  int size_heap = 0;
  int current;
  int executing = 0;
  int t = 0;

  for (i = 0; i < size; i++) {
    pthread_mutex_init(&sem[i], NULL);
    pthread_mutex_lock(&sem[i]);

    pthread_mutex_init(&empty[i], NULL);
    pthread_mutex_init(&full[i], NULL);
    pthread_mutex_lock(&full[i]);
  }

  for (i = 0; i < size; i++) {
    if (pthread_create(&schedules[i].thread, NULL, execute_thread, (void *) &schedules[i].index)) {
      printf("\n ERROR creating thread");
      exit(1);
    }
  }

  start = time(NULL);
  i = 0;
  execTime = 1e6;

  while (1) {
    if ((int) difftime(time(NULL), start) == t) {
      for (j = i; j < size && schedules[j].t0 == t; j++) {
        insert(heap, &size_heap, schedules[j]);
        printf("%s ENTROU NO HEAP\n", schedules[j].name);
      }

      i = j;

      if (executing) {
        pthread_mutex_lock(&full[current]);

        if (schedules[current].dt == 0) {
          printf("PROCESSO %s terminou\n", schedules[current].name);
          executing = 0;
          pthread_cancel(schedules[current].thread);
          schedules[i].tf = difftime(time(NULL),start);

          if (size_heap > 0) {
            current = heap[0].index;
            remove_min(heap, &size_heap);

            printf("%s VAI EXECUTAR...\n", schedules[current].name);
            pthread_mutex_unlock(&sem[current]);

            executing = 1;
          }
        }
        else if (size_heap > 0 && heap[0].dt < schedules[current].dt) {
          int previous = current;
          current = heap[0].index;

          printf("%s VAI SER SUBSTITUÍDO POR %s\n", schedules[previous].name, schedules[current].name);
          remove_min(heap, &size_heap);
          insert(heap, &size_heap, schedules[previous]);

          printf("%s VAI EXECUTAR...\n", schedules[current].name);
          pthread_mutex_unlock(&sem[current]);
        }

        pthread_mutex_unlock(&empty[current]);
      }
      else {
        if (size_heap > 0) {
          current = heap[0].index;
          remove_min(heap, &size_heap);
          printf("%s VAI EXECUTAR E NAO TINHA NADA EXECUTANDO...\n", schedules[current].name);
          pthread_mutex_unlock(&sem[current]);
          executing = 1;
        }
      }

      t++;
    }

    if (size_heap == 0 && !executing && i == size)
      break;
  }

  for (i = 0; i < size; i++) {
    pthread_join(schedules[i].thread, NULL);
    pthread_mutex_destroy(&sem[i]);
    pthread_mutex_destroy(&empty[i]);
    pthread_mutex_destroy(&full[i]);
  }
}

void fcfs() {
  int i = 0;
  start = time(NULL);

  for (i = 0; i < size; i++) {
    pthread_mutex_init(&sem[i], NULL);
    pthread_mutex_lock(&sem[i]);
  }

  for (i = 0; i < size; i++) {
    if (pthread_create(&schedules[i].thread, NULL, execute_thread, (void *) &schedules[i].index)) {
      printf("\n ERROR creating thread");
      exit(1);
    }
  }


  for (i = 0; i < size; i++) {
    while (difftime(time(NULL), start) < schedules[i].t0);
    printf("%s vai rodar por %d segundos!!\n", schedules[i].name, schedules[i].dt);
    
    pthread_mutex_unlock(&sem[i]);
    pthread_join(schedules[i].thread, NULL);

    schedules[i].tf = difftime(time(NULL),start);
    printf("tf = %d\n", schedules[i].tf);
  }

  for (i = 0; i < size; i++) {
    pthread_mutex_destroy(&sem[i]);
  }
}

int main(int argc, char **argv) {
  char * filename;
  char * output_filename;
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
    schedules[size].udt = 0;
    schedules[size].index = size;

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
