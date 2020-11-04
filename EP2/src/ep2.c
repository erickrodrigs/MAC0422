#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "stack.h"
#include "cyclist.h"
#include "sort.h"

int d, n;
int debug = 0;
int *track[10];
int *canContinue;
int countBroken = 0;
int maximumVelocity = 0;
struct timeval start;
pthread_mutex_t *sem[10], *availableCyclists, *lapsSem, randMutex;
pthread_barrier_t barrier;
pthread_t *threads;
StackNode **stacks;
Cyclist *cyclists;

void printRank(int *vector, int b) {
  int rank = 1, i;

  printf("Ciclista %d está em %dº lugar ", cyclists[vector[b]].id, rank);
  printf("com tempo %lf segundos\n", cyclists[vector[b]].runningTime);

  for (i = b - 1; i >= 0; i--) {
    if (!cyclists[vector[i]].broken) {
      rank++;
      printf("Ciclista %d está em %dº lugar ", cyclists[vector[i]].id, rank);
      printf("com tempo %lf segundos\n", cyclists[vector[i]].runningTime);
    }
  }

  for (i = b - 1; i >= 0; i--) {
    if (cyclists[vector[i]].broken) {
      printf("Ciclista %d quebrou na volta %d ", cyclists[vector[i]].id, cyclists[vector[i]].laps);
      printf("com tempo %lf segundos\n", cyclists[vector[i]].runningTime);
    }
  }
}

int randomVelocity(int id) {
  int probability;
  int value;

  pthread_mutex_lock(&randMutex);
  value = (rand() % 100) + 1;
  pthread_mutex_unlock(&randMutex);

  if (cyclists[id].velocity == 3)
    return 3;

  if (cyclists[id].laps >= 2 * (n - countBroken - 1)) {
    if (value <= 10)
      return 3;
  }

  switch (cyclists[id].velocity) {
    case 1:
      probability = 20;
      break;
    case 2:
      probability = 40;
      break;
    default:
      break;
  }

  if (value > probability)
    return 2;
  else
    return 1;
}

void changePosition(int cyclist) {
  int line, column, changed = 0, laps;
  struct timeval currentTime;

  line = cyclists[cyclist].linePosition;
  column = cyclists[cyclist].columnPosition;

  pthread_mutex_lock(&sem[line][column]);

  if (cyclists[cyclist].laps >= 2 * n) {
    cyclists[cyclist].finished = 1;
    pthread_mutex_unlock(&sem[line][column]);
    return;
  }

  pthread_mutex_lock(&sem[line][(column + 1) % d]);
  if (track[line][(column + 1) % d] == 0) {
    cyclists[cyclist].columnPosition = (column + 1) % d;
    track[line][(column + 1)% d] = cyclist + 1;
    changed = 1;
    pthread_mutex_unlock(&sem[line][(column + 1) % d]);
  }
  else {
    pthread_mutex_unlock(&sem[line][(column + 1) % d]);

    for (int i = line + 1; i < 10 && !changed; i++) {
      pthread_mutex_lock(&sem[i][column]);
      pthread_mutex_lock(&sem[i][(column + 1) % d]);

      if (track[i][column] == 0 && track[i][(column + 1) % d] == 0) {
        cyclists[cyclist].linePosition = i;
        cyclists[cyclist].columnPosition = (column + 1) % d;
        track[i][(column + 1) % d] = cyclist + 1;
        changed = 1;
      }

      pthread_mutex_unlock(&sem[i][(column + 1) % d]);
      pthread_mutex_unlock(&sem[i][column]);
    }
  }

  if (changed) {
    if ((column + 1) % d == 0) {
      cyclists[cyclist].laps++;
      laps = cyclists[cyclist].laps;
      cyclists[cyclist].velocity = randomVelocity(cyclist);

      gettimeofday(&currentTime, NULL);

      cyclists[cyclist].runningTime = currentTime.tv_sec - start.tv_sec;
      cyclists[cyclist].runningTime += (currentTime.tv_usec - start.tv_usec) / 1e6;
      
      pthread_mutex_lock(&lapsSem[laps]);
      stacks[laps] = push(stacks[laps], cyclist);
      pthread_mutex_unlock(&lapsSem[laps]);
    }
    track[line][column] = 0;
  }

  pthread_mutex_unlock(&sem[line][column]);
}

void * thread(void * id) {
  int cyclist = *((int *) id);
  int count = 10;
  int timeRemaining = 0;
  int executionTime = 0;

  while (1) {
    switch (cyclists[cyclist].velocity) {
      case 1:
        timeRemaining = (maximumVelocity ? 6 : 2);
        break;
      case 2:
        timeRemaining = (maximumVelocity ? 3 : 1);
        break;
      default:
        timeRemaining = 1;
        break;
    }

    while (timeRemaining != 0) {
      
      timeRemaining--;
      pthread_barrier_wait(&barrier);
      if (timeRemaining == 0)
        changePosition(cyclist);
      canContinue[cyclist] = 0;

      pthread_barrier_wait(&barrier);

      while (!canContinue[cyclist])
        usleep(100);

      if (cyclists[cyclist].broken || cyclists[cyclist].eliminated)
        pthread_exit(NULL);

      if (!executionTime && maximumVelocity) {
        executionTime = 1;
        timeRemaining *= 3;
      }
    }

    
    count--;
  }

  return NULL;
}

void printTrack() {
  printf("\n\n");

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < d; j++) {
      printf("%3d|  ", track[i][j]);
    }
    printf("\n");
  }

  printf("\n\n");
}

void judge(int remainingCyclists) {
  int lap = 1, line, column;
  int currentCyclist;
  int brokenProbability;
  int someoneHasLeft;
  int lapCompleted;

  while (remainingCyclists > 1) {
    maximumVelocity = 0;
    for (int i = 0; i < n; i++) {
      if (cyclists[i].velocity == 3)
        maximumVelocity = 1;
    }

    if (maximumVelocity)
      usleep(20000);
    else
      usleep(60000);

    pthread_barrier_wait(&barrier);
    pthread_barrier_wait(&barrier);

    if (debug)
      printTrack();

    someoneHasLeft = 0;

    if (remainingCyclists > 5) {
      for (int i = 0; i < n; i++) {
        brokenProbability = (rand() % 100) + 1;

        if (cyclists[i].laps == 0 || cyclists[i].broken || cyclists[i].eliminated || cyclists[i].finished)
          continue;
        
        if (brokenProbability <= 5 && cyclists[i].laps % 6 == 0 && cyclists[i].columnPosition == 0) {
          cyclists[i].broken = 1;

          printf("Ciclista %d quebrou!! ", cyclists[i].id);
          printf("Ele estava na volta %d\n", cyclists[i].laps);

          countBroken += 1;
          remainingCyclists--;

          track[cyclists[i].linePosition][cyclists[i].columnPosition] = 0;

          someoneHasLeft = 1;

          if (remainingCyclists == 5)
            break;
        }
      }
    }

    lapCompleted = 1;

    for (int i = 0; i < n; i++)
      if (!cyclists[i].broken && !cyclists[i].eliminated && cyclists[i].laps < lap) {
        lapCompleted = 0;
        break;
      }
    

    if (lapCompleted) {
      printf("\n\nVOLTA %d\n", lap);
      printStack(stacks[lap]);

      if (lap % 2 == 0){
        while (cyclists[top(stacks[lap])].broken || cyclists[top(stacks[lap])].eliminated)
          stacks[lap] = pop(stacks[lap]);

        if (cyclists[top(stacks[lap])].columnPosition == 0) {
          currentCyclist = top(stacks[lap]);
          stacks[lap] = pop(stacks[lap]);
          cyclists[currentCyclist].eliminated = 1;
          line = cyclists[currentCyclist].linePosition;
          column = cyclists[currentCyclist].columnPosition;

          printf("Ciclista %d foi embora!! ", currentCyclist + 1);
          printf("Tempo final ciclista %d: %lf\n", currentCyclist + 1, cyclists[currentCyclist].runningTime);         
          remainingCyclists--;

          track[line][column] = 0;
    
          someoneHasLeft = 1;
        }
      }
      lap += 1;
    }

    if (someoneHasLeft) {
      if (remainingCyclists > 1) {
        pthread_barrier_destroy(&barrier);
        pthread_barrier_init(&barrier, NULL, remainingCyclists + 1);
      }
      else {
        lap -= 1;
        printf("CICLISTA VENCEDOR: %d\n", top(stacks[lap]) + 1);
        cyclists[top(stacks[lap])].eliminated = 1;
      }
    }

    for (int i = 0; i < n; i++)
      canContinue[i] = 1;
  }

  pthread_barrier_destroy(&barrier);
  printf("CICLISTAS QUEBRADOS: %d\n", countBroken);
}

void putCyclistInTrack(int cyclistNumber, int line, int column) {
  int id = cyclistNumber - 1;

  track[line][column] = cyclistNumber;
  cyclists[id].columnPosition = column;
  cyclists[id].linePosition = line;
  cyclists[id].velocity = 1;
  cyclists[id].laps = 0;
  cyclists[id].broken = 0;
  cyclists[id].eliminated = 0;
  cyclists[id].id = cyclistNumber;
  cyclists[id].finished = 0;
}

void initTrack() {
  int remainingCyclists;
  int column = 0;
  int i;
  int currentCyclist = n;

  for (int i = 0; i < 10; i++) {
    track[i] = malloc(d*sizeof(int));
    
    for (int j = 0; j < d; j++)
      track[i][j] = 0;
  }

  if (n % 5 != 0) {
    remainingCyclists = (n % 5);
    i = 0;
    while (remainingCyclists != 0) {
      putCyclistInTrack(currentCyclist, i, column);
      i += 1;
      remainingCyclists--;
      currentCyclist--;
    }
    column++;
  }

  remainingCyclists = n - (n % 5);
  i = 0;
  while (remainingCyclists != 0) {
    if (i == 5) {
      column++;
      i = 0;
    }
    else {
      putCyclistInTrack(currentCyclist, i, column);
      i += 1;
      remainingCyclists--;
      currentCyclist--;
    }
  }
}

void initMutex() {
  availableCyclists = malloc(n * sizeof(pthread_mutex_t));
  lapsSem = malloc(2 * (n + 1) * sizeof(pthread_mutex_t));

  for (int i = 0; i < n; i++) {
    pthread_mutex_init(&availableCyclists[i], NULL);
    pthread_mutex_lock(&availableCyclists[i]);
  }

  for (int i = 0; i < 2 * (n + 1); i++)
    pthread_mutex_init(&lapsSem[i], NULL);
  
  pthread_mutex_init(&randMutex, NULL);

  for (int i = 0; i < 10; i++) {
    sem[i] = malloc(d * sizeof(pthread_mutex_t));

    for (int j = 0; j < d; j++) {
      pthread_mutex_init(&sem[i][j], NULL);
    }
  }
}

void generateOutput(char *name) {
  int i, j;
  size_t size;
  long int memoryUsage = 0;
  struct timeval currentTime;
  double finishTime;
  char *line;

  
  FILE * memoryUsageFile = fopen("/proc/self/status", "r");
  FILE * programLogFile = fopen(name, "a");

  for (i = 0; i < 23; i++)
    getline(&line, &size, memoryUsageFile);

  for (i = 0; line[i] > '9' || line[i] < '0'; i++);

  for (j = i + 1; line[j] <= '9' && line[j] >= '0'; j++);
  line[j] = '\0';
  line = line + i;
  
  memoryUsage = atol(line);

  gettimeofday(&currentTime, NULL);

  finishTime = currentTime.tv_sec - start.tv_sec;
  finishTime += (currentTime.tv_usec - start.tv_usec) / 1e6;
 
  fprintf(programLogFile, "%ld %lf\n", memoryUsage, finishTime);

  fclose(programLogFile);
  fclose(memoryUsageFile);
}

void freeAllocatedMemory(int *ids) {
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < d; j++) {
      pthread_mutex_destroy(&sem[i][j]);
    }
    
    free(sem[i]);
    free(track[i]);
  }

  for (int i = 0; i < n; i++) {
    pthread_join(threads[i], NULL);
    pthread_mutex_destroy(&availableCyclists[i]);
  }

  for (int i = 0; i < 2 * (n + 1); i++) {
    pthread_mutex_destroy(&lapsSem[i]);

    while (!empty(stacks[i]))
      stacks[i] = pop(stacks[i]);
  }

  pthread_mutex_destroy(&randMutex);

  free(availableCyclists);
  free(lapsSem);
  free(cyclists);
  free(threads);
  free(ids);
  free(canContinue);
  free(stacks);
}

int main(int argc, char ** argv) {
  int *ids;
  char *path = NULL;

  d = atoi(argv[1]);
  n = atoi(argv[2]);

  for (int i = 3; i < argc; i++) {
    if (argv[i][0] == '-') 
      debug = 1;
    else 
      path = argv[i];
  }

  srand(time(NULL));
  gettimeofday(&start, NULL);

  cyclists = malloc(n * sizeof(Cyclist));
  stacks = malloc(2 * (n + 1) * sizeof(StackNode *));
  threads = malloc(n * sizeof(pthread_t));
  ids = malloc(n * sizeof(int));
  canContinue = malloc(n * sizeof(int));

  pthread_barrier_init(&barrier, NULL, n + 1);

  for (int i = 0; i < 2 * (n + 1); i++)
    stacks[i] = NULL;

  for (int i = 0; i < n; i++)
    canContinue[i] = 0;

  initMutex();
  initTrack();

  if (debug)
    printTrack();

  for (int i = 0; i < n; i++) {
    ids[i] = i;
    pthread_create(&threads[i], NULL, thread, &ids[i]);
  }

  judge(n);

  printf("\nFIM DA CORRIDA!!!\n");

  if (debug)
    printTrack();

  printf("\nCLASSIFICAÇÃO FINAL:\n");
  mergeSort(ids, 0, n - 1, cyclists);
  printRank(ids, n - 1);

  if (path)
    generateOutput(path);

  freeAllocatedMemory(ids);

  return 0;
}
