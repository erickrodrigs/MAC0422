#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX 400

typedef struct cyclist {
  int columnPosition;
  int linePosition;
  int velocity;
} Cyclist;


int d, n;
_Atomic int *track[10];
Cyclist *cyclists;

pthread_mutex_t *sem[10];
pthread_barrier_t barrier;


int randomVelocity(int previousVelocity) {
  int probability;
  int value;

  switch (previousVelocity) {
    case 1:
      probability = 20;
      break;
    case 2:
      probability = 40;
      break;
    default:
      break;
  }

  value = (rand() % 100) + 1;

  if (value > probability)
    return 2; // 60km/h
  else
    return 1; // 30km/h
}

void changePosition(int cyclist) {
  int changedPosition = 0;

  pthread_mutex_lock(&sem[cyclists[cyclist].linePosition][(cyclists[cyclist].columnPosition + 1) % d]);

  if (track[cyclists[cyclist].linePosition][(cyclists[cyclist].columnPosition + 1) % d] == 0) {
    track[cyclists[cyclist].linePosition][cyclists[cyclist].columnPosition % d] = 0;
    track[cyclists[cyclist].linePosition][(cyclists[cyclist].columnPosition + 1) % d] = cyclist + 1;
    
    cyclists[cyclist].columnPosition = (cyclists[cyclist].columnPosition + 1) % d;
    changedPosition = 1;
  }

  if (changedPosition) {
    if (cyclists[cyclist].columnPosition == 0) {
      // volta nova, muda velocidade
      cyclists[cyclist].velocity = randomVelocity(cyclists[cyclist].velocity);
    }
    
    pthread_mutex_unlock(&sem[cyclists[cyclist].linePosition][(cyclists[cyclist].columnPosition) % d]);
  } else {
    pthread_mutex_unlock(&sem[cyclists[cyclist].linePosition][(cyclists[cyclist].columnPosition + 1) % d]);
  }
}

void * thread(void * id) {
  int cyclist = *((int *) id);
  int count = 20;

  // teste simples de mudança de posições na pista
  while (count != 0) {
    // tentativa meio meh de fazer as velocidades (nao testado perfeitamente)
    switch (cyclists[cyclist].velocity) {
      case 1:
        usleep(12000);
        changePosition(cyclist);
        break;
      case 2:
        usleep(6000);
        changePosition(cyclist);
        if (cyclists[cyclist].velocity == 2)
          usleep(6000);
        else
          usleep(12000);
        changePosition(cyclist);
        break;
      default:
        break;
    }
    // usleep(20000);
    // changePosition(cyclist);

    pthread_barrier_wait(&barrier);
    count -= 1;
  }

  return NULL;
}

void printTrack() {
  int i, j;
  
  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
      printf("%3d|  ", track[i][j]);
    }
    printf("\n");
  }
}

int main(int argc, char ** argv) {
  int i, *id, column = 0, remainingCyclists, currentCyclist, j;
  d = atoi(argv[1]);
  n = atoi(argv[2]);

  currentCyclist = n;

  pthread_t *threads = malloc(n*sizeof(pthread_t));

  id = malloc(n*sizeof(int));

  for (i = 0; i < 10; i++) {
    track[i] = malloc(d*sizeof(int));
    
    for (j = 0; j < d; j++)
      track[i][j] = 0;
  }

  cyclists = malloc((n+1)*sizeof(Cyclist));

  if (n % 5 != 0) {
    remainingCyclists = (n % 5);
    i = 0;
    while (remainingCyclists != 0) {
      track[i][column] = currentCyclist;
      cyclists[currentCyclist - 1].columnPosition = column;
      cyclists[currentCyclist - 1].linePosition = i;
      cyclists[currentCyclist - 1].velocity = 1;
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
      track[i][column] = currentCyclist;
      cyclists[currentCyclist - 1].columnPosition = column;
      cyclists[currentCyclist - 1].linePosition = i;
      cyclists[currentCyclist - 1].velocity = 1;
      i += 1;
      remainingCyclists--;
      currentCyclist--;
    }
  }

  printf("INICIOOOO\n\n");
  printTrack();

  pthread_barrier_init(&barrier, NULL, n);

  for (i = 0; i < 10; i++) {
    sem[i] = malloc(d * sizeof(pthread_mutex_t));

    for (j = 0; j < d; j++) {
      pthread_mutex_init(&sem[i][j], NULL);
    }
  }

  for (i = 0; i < n; i++) {
    id[i] = i;
    pthread_create(&threads[i], NULL, thread, &id[i]);
  }

  for (i = 0; i < n; i++) {
    pthread_join(threads[i], NULL);
  }

  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
      pthread_mutex_destroy(&sem[i][d]);
    }
  }

  printf("FIM DA PRIMEIRA ITERAÇÃO:\n");
  printTrack();

  free(cyclists);
  free(threads);
  free(id);

  for (i = 0; i < 10; i++)
    free(track[i]);

  return 0;
}
