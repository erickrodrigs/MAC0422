#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#define MAX 400

int d, n;
int *track[10];

void * thread(void * id) {
  int cyclist = *((int *) id);
  
  return NULL;
}

void printTrack() {
  int i, j;
  
  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
      printf("%3d|", track[i][j]);
    }
    printf("\n");
  }
}


int main(int argc, char ** argv) {
  int i, *id, column = 0, remainingCyclists, actualCyclist = 1, j;
  d = atoi(argv[1]);
  n = atoi(argv[2]);

  pthread_t *threads = malloc(n*sizeof(pthread_t));

  id = malloc(n*sizeof(int));

  for (i = 0; i < 10; i++) {
    track[i] = malloc(d*sizeof(int));
    
    for (j = 0; j < d; j++)
      track[i][j] = 0;
  }
  
  for (i = 0; i < n; i++) {
    id[i] = i; 
    pthread_create(&threads[i], NULL, thread, &id[i]);
  }

  if (n%5 != 0) {
    remainingCyclists = n%5;
    i = 0;
    while (remainingCyclists != 0) {
        track[i][column] = actualCyclist;
        i += 2;
        remainingCyclists--;
        actualCyclist++;
    }
    column++;
  }

  remainingCyclists = n - n%5;
  i = 0;
  while (remainingCyclists != 0) {
    if (i == 10) {
      column++;
      i = 0;
    }
    else {
      track[i][column] = actualCyclist;
      i += 2;
      remainingCyclists--;
      actualCyclist++;
    }
  }

  printTrack();

  free(threads);
  free(id);

  for (i = 0; i < 10; i++)
    free(track[i]);

  return 0;
}