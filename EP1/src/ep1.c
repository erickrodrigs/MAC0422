#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "heap.h"
#include "queue.h"

#define RB_EXEC_TIME (long int) 1e6/2

pthread_t currentThread;
pthread_mutex_t sem[MAX];
pthread_mutex_t empty[MAX];
pthread_mutex_t full[MAX];
Schedule schedules[MAX];
int size;
int scheduler;
int context_switch;
long int execTime;
int show_info;

void processEntering(int processOrder) {
  fprintf(stderr,"%s entrou no sistema, com t0 = %d, dt = %d, deadline = %d \n", schedules[processOrder].name, schedules[processOrder].t0, schedules[processOrder].dt, schedules[processOrder].deadline);
}

int greaterEq(suseconds_t ubegin, suseconds_t uend, double begin, double end) {
  if (begin < end) return 1;
  
  return (begin == end && ubegin < uend);
}

void * execute_thread(void * current) {
  int curr = *((int *) current);
  int i = 0;
  
  if (scheduler != 1) {
    while (schedules[curr].dt > 0 || schedules[curr].udt > 0) {
      //pthread_mutex_lock(&sem[curr]);
      pthread_mutex_lock(&empty[curr]);

      if (show_info)
        fprintf(stderr, "CPU processo %s: %d\n", schedules[curr].name, sched_getcpu());
      
      usleep(execTime);
      if (execTime > schedules[curr].udt) {
        schedules[curr].dt--;
        schedules[curr].udt += 1e6 - execTime;
      }
      else {
        schedules[curr].udt -= execTime;
      }
      i = 2 * 2;
      
      printf("DT de %s AGORA VALE %d e o udt vale %ld \n", schedules[curr].name, schedules[curr].dt, schedules[curr].udt);
      //pthread_mutex_unlock(&sem[curr]);
      if (show_info)
        fprintf(stderr, "Processo %s vai liberar a CPU %d\n", schedules[curr].name, sched_getcpu());
      
      pthread_mutex_unlock(&full[curr]);
    }
  }
  else {
    pthread_mutex_lock(&sem[curr]);
    if (show_info)
      fprintf(stderr, "CPU processo %s: %d\n", schedules[curr].name, sched_getcpu());
    while (schedules[curr].dt > 0) {
      sleep(1);
      schedules[curr].dt--;
      i = 2 * 2;
    }
    if (show_info)
      fprintf(stderr, "Processo %s vai liberar a CPU %d\n", schedules[curr].name, sched_getcpu());
  }
  return NULL;
}

void roundRobin() {
  int i, j;
  int current;
  int executing = 0;
  int t = 0;
  long int ut = 0;
  Queue queue;
  struct timeval start, actual;

  for (i = 0; i < size; i++) {
    //pthread_mutex_init(&sem[i], NULL);
    

    pthread_mutex_init(&empty[i], NULL);
    pthread_mutex_init(&full[i], NULL);
    pthread_mutex_lock(&full[i]);
    pthread_mutex_lock(&empty[i]);
  }

  for (i = 0; i < size; i++) {
    if (pthread_create(&schedules[i].thread, NULL, execute_thread, (void *) &schedules[i].index)) {
      printf("\n ERROR creating thread");
      exit(1);
    }
  }

  i = 0;
  queue_init(&queue);
  execTime = RB_EXEC_TIME;
  gettimeofday(&start, NULL);
  while (1) {
    gettimeofday(&actual, NULL);
    //printf("%d && %d\n", (int)  == t,  >= ut));
    //printf("%ld\n", (actual.tv_usec - start.tv_usec));
    //printf("AAAAAAAA\n");
    if (greaterEq(ut, actual.tv_usec - start.tv_usec, t, difftime(actual.tv_sec, start.tv_sec))) {
      for (j = i; j < size && schedules[j].t0 == t; j++) {
        insert_queue(&queue, schedules[j]);
        if (show_info)
          processEntering(j);
      }
      i = j;
    
      if (executing) {
        pthread_mutex_lock(&full[current]);

        if (schedules[current].dt == 0 && schedules[current].udt == 0) {
          printf("PROCESSO %s terminou\n", schedules[current].name);
          executing = 0;
          pthread_cancel(schedules[current].thread);
          schedules[current].tf = difftime(time(NULL),start.tv_sec);
          fprintf(stderr, "SAÍDA: %s %d %d\n", schedules[current].name, schedules[current].tf, schedules[current].tf - schedules[current].t0);

          if (!is_empty(&queue)) {
            current = remove_queue(&queue).index;
            executing = 1;
            context_switch += 1;
            printf("%s VAI EXECUTAR...\n", schedules[current].name);
            //pthread_mutex_unlock(&sem[current]);
          }
        }
        else if (!is_empty(&queue)) {
          int previous = current;
          current = remove_queue(&queue).index;

          printf("%s VAI SER SUBSTITUÍDO POR %s\n", schedules[previous].name, schedules[current].name);
          insert_queue(&queue, schedules[previous]);

          context_switch += 1;

          printf("%s VAI EXECUTAR...\n", schedules[current].name);
          //pthread_mutex_unlock(&sem[current]);
        }

        pthread_mutex_unlock(&empty[current]);
      }
      else {
        if (!is_empty(&queue)) {
          current = remove_queue(&queue).index;
          printf("%s VAI EXECUTAR E NAO TINHA NADA EXECUTANDO...\n", schedules[current].name);
          pthread_mutex_unlock(&empty[current]);
          executing = 1;
        }
        else if (i == size) 
          break;
      }
      if (execTime + ut >= 1e6) {
        t++;
        ut += execTime - 1e6;
      }
      else ut += execTime;
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
  time_t start;

  for (i = 0; i < size; i++) {
    //pthread_mutex_init(&sem[i], NULL);
    //pthread_mutex_lock(&sem[i]);

    pthread_mutex_init(&empty[i], NULL);
    pthread_mutex_init(&full[i], NULL);
    pthread_mutex_lock(&full[i]);
    pthread_mutex_lock(&empty[i]);
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
        if (show_info)
          processEntering(j);
      }

      i = j;

      if (executing) {
        pthread_mutex_lock(&full[current]);

        if (schedules[current].dt == 0) {
          printf("PROCESSO %s terminou\n", schedules[current].name);
          executing = 0;
          pthread_cancel(schedules[current].thread);
          schedules[current].tf = difftime(time(NULL),start);
          fprintf(stderr, "SAÍDA: %s %d %d\n", schedules[current].name, schedules[current].tf, schedules[current].tf - schedules[current].t0);

          if (size_heap > 0) {
            current = heap[0].index;
            remove_min(heap, &size_heap);

            context_switch += 1;

            printf("%s VAI EXECUTAR...\n", schedules[current].name);
            //pthread_mutex_unlock(&sem[current]);

            executing = 1;
          }
        }
        else if (size_heap > 0 && heap[0].dt < schedules[current].dt) {
          int previous = current;
          current = heap[0].index;

          printf("%s VAI SER SUBSTITUÍDO POR %s\n", schedules[previous].name, schedules[current].name);
          remove_min(heap, &size_heap);
          insert(heap, &size_heap, schedules[previous]);

          context_switch += 1;

          printf("%s VAI EXECUTAR...\n", schedules[current].name);
          //pthread_mutex_unlock(&sem[current]);
        }

        pthread_mutex_unlock(&empty[current]);
      }
      else {
        if (size_heap > 0) {
          current = heap[0].index;
          remove_min(heap, &size_heap);
          printf("%s VAI EXECUTAR E NAO TINHA NADA EXECUTANDO...\n", schedules[current].name);
          pthread_mutex_unlock(&empty[current]);
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
  int i;
  int executing = 0;
  int t = 0;
  int curr;
  time_t start;

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

  curr = -1;
  i = -1;
  start = time(NULL);

  while (1) {
    if (difftime(time(NULL), start) == t) {
      while (schedules[i+1].t0 == t) {
        i++;
        if (show_info) processEntering(i);    
      }
      t++;
    }
    if (executing && schedules[curr].dt == 0) {
      schedules[curr].tf = difftime(time(NULL),start);
      fprintf(stderr, "SAÍDA: %s %d %d\n", schedules[curr].name, schedules[curr].tf, schedules[curr].tf - schedules[curr].t0);
      executing = 0;
      if (curr + 1 <= i){
        curr++;
        context_switch += 1;
        pthread_mutex_unlock(&sem[curr]);
        executing = 1;
      }
    }
    if (!executing) {
      if (curr + 1 <= i){
        curr++;
        pthread_mutex_unlock(&sem[curr]);
        executing = 1;
      }
    }
    if (!executing && curr + 1 == size) break;
  }

  for (i = 0; i < size; i++) {
    pthread_join(schedules[i].thread, NULL);
    pthread_mutex_destroy(&sem[i]);
  }
}

int main(int argc, char **argv) {
  char * filename;
  char * output_filename;
  FILE * file;
  FILE * output_file;
  int i; 
  size = 0;
  show_info = 0;

  if (argc < 4 || argc > 5) {
    printf("USO INVÁLIDO!\n");
    exit(1);
  }

  if (argc == 5)
    show_info = 1;

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

  context_switch = 0;
 
  switch(scheduler) {
    case 1:
      fcfs();
      break;
    case 2:
      srtn();
      break;
    case 3:
      roundRobin();
      break;
    default:
      printf("Escalonador inválido!\n");
      exit(1);
      break;
  }

  fprintf(stderr, "Número de mudanças de contexto %d\n", context_switch);

  output_file = fopen(output_filename, "w");
  
  for (i = 0; i < size; i++)
    fprintf(output_file, "%s %d %d\n", schedules[i].name, schedules[i].tf, schedules[i].tf - schedules[i].t0);

  fprintf(output_file, "%d\n", context_switch);

  fclose(output_file);

  return 0;
}
