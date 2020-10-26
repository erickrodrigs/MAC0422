#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#define MAX 400

typedef struct cyclist {
  int id;
  int columnPosition;
  int linePosition;
  int velocity;
  int laps;
  int broken;
} Cyclist;

int d, n;
int debug = 1;
int *track[10];
Cyclist *cyclists;

pthread_mutex_t *sem[10], *availableCyclists;
pthread_barrier_t barrier;
pthread_t *threads;

void printRank(int *vector, int b) {
  int rank = 1, i;

  printf("Ciclista %d está em %dº lugar \n", cyclists[vector[b]].id, rank);

  for (i = b - 1; i >= 0; i--) {
    if (cyclists[vector[i]].laps != cyclists[vector[i+1]].laps 
    || cyclists[vector[i]].columnPosition != cyclists[vector[i+1]].columnPosition) {
      rank++;
    }
    printf("Ciclista %d está em %dº lugar \n", cyclists[vector[i]].id, rank);
  }

}

void merge(int *vector, int a, int q, int b) {
  int i, j, k;
  int * aux = malloc((b - a + 1)*sizeof(int));
  
  for (i = a; i <= q; i++)
    aux[i - a] = vector[i];

  for (i = b, j = q - a + 1; i >= q+1 ; i--, j++)
    aux[j] = vector[i];

  i = 0;
  j = b - a;

  for (k = a; k <= b; k++) {
    if (cyclists[aux[i]].laps < cyclists[aux[j]].laps) {
      vector[k] = aux[i];
      i++;
    }
    else if (cyclists[aux[i]].laps == cyclists[aux[j]].laps && 
            cyclists[aux[i]].columnPosition < cyclists[aux[j]].columnPosition) {
      vector[k] = aux[i];
      i++;
    }
    else {
      vector[k] = aux[j];
      j--;
    }
  
  }

  free(aux);
}

void mergeSort(int *vector, int a, int b) {
  if (a < b) {
    int q = (a + b)/2;
    mergeSort(vector, a, q);
    mergeSort(vector, q + 1, b);

    merge(vector, a, q, b);
  }
}

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
  int line, column, changed;
  line = cyclists[cyclist].linePosition;
  column = cyclists[cyclist].columnPosition;

  pthread_mutex_lock(&sem[line][column]);

  pthread_mutex_lock(&sem[line][(column + 1) % d]);
  if (track[line][(column + 1) % d] == 0) {
    cyclists[cyclist].columnPosition = (column + 1)% d;
    track[line][(column + 1)% d] = cyclist + 1;
    changed = 1;
    pthread_mutex_unlock(&sem[line][(column + 1) % d]);
  }
  else {
    pthread_mutex_unlock(&sem[line][(column + 1) % d]);

    for (int i = line + 1; i < 10 && !changed; i++) {
      pthread_mutex_lock(&sem[i][column]);
      pthread_mutex_lock(&sem[i][(column + 1) % d]);

      if (track[i][column] == 0 && track[i][(column + 1)% d] == 0) {
        cyclists[cyclist].linePosition = i;
        cyclists[cyclist].columnPosition = (column + 1)% d;
        track[i][(column + 1)% d] = cyclist + 1;
        changed = 1;
      }

      pthread_mutex_unlock(&sem[i][(column + 1) % d]);
      pthread_mutex_unlock(&sem[i][column]);
    }
  }

  if (changed) {
    if ((column + 1) % d == 0) {
      cyclists[cyclist].velocity = randomVelocity(cyclists[cyclist].velocity);
      cyclists[cyclist].laps++;
    }
    track[line][column] = 0;
  }

  pthread_mutex_unlock(&sem[line][column]);
}

void * thread(void * id) {
  int cyclist = *((int *) id);
  int count = 10;
  int timeRemaining = 0;

  while (1) {
    switch (cyclists[cyclist].velocity) {
      case 1:
        timeRemaining = 2;
        break;
      case 2:
        timeRemaining = 1;
        break;
      default:
        break;
    }

    while (timeRemaining != 0) {
      usleep(60000);
      timeRemaining--;

      if (timeRemaining == 0)
        changePosition(cyclist);
      
      pthread_barrier_wait(&barrier);
      //processamento do juiz
      pthread_mutex_lock(&availableCyclists[cyclist]);
    }

    
    count--;
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

void judge(int remainingCyclists, int *sortedCyclists) {
  int previousLap = 0;
  int begin = 0, currentCyclist;
  int brokenProbability;
  int someoneHasBroken;
  Cyclist current;

  while (remainingCyclists != 1) {
    pthread_barrier_wait(&barrier);

    if (debug)
      printTrack();

    mergeSort(sortedCyclists, begin, n - 1);
    printRank(sortedCyclists, n - 1);

    someoneHasBroken = 0;

    // verifica se o ciclista quebrou
    for (int i = 0; i < n; i++) {
      brokenProbability = (rand() % 100) + 1;

      if (cyclists[i].laps == 0 || cyclists[i].broken)
        continue;
      
      if (brokenProbability <= 5 && cyclists[i].laps % 6 == 0) {
        cyclists[i].broken = 1;

        printf("Ciclista %d quebrou!!\n", cyclists[i].id + 1);
        pthread_cancel(threads[i]);
        remainingCyclists--;

        track[cyclists[i].linePosition][cyclists[i].columnPosition] = 0;

        someoneHasBroken = 1;
      }
    }

    currentCyclist = sortedCyclists[begin];
    current = cyclists[currentCyclist];

    /* alguém pode ter quebrado com os 5% nas primeiras posições,
      assim que esse cara que quebrou ficar na primeira posição do sortedCyclists,
      ele não é o último (pois já quebrou). Precisamos andar pra frente com o begin
    */
    while (current.broken) {
      begin += 1;
      currentCyclist = sortedCyclists[begin];
      current = cyclists[currentCyclist];
    }

    if (current.laps > previousLap && current.laps % 2 == 0) {
      previousLap = current.laps;

      cyclists[currentCyclist].broken = 1;

      printf("Ciclista %d foi embora!!\n", currentCyclist + 1);
      pthread_cancel(threads[currentCyclist]);
      remainingCyclists--;

      begin += 1;

      track[current.linePosition][current.columnPosition] = 0;

      someoneHasBroken = 1;
    }

    if (someoneHasBroken) {
      pthread_barrier_destroy(&barrier);
      pthread_barrier_init(&barrier, NULL, remainingCyclists + 1);
    }

    for (int i = 0; i < n; i++) {
      pthread_mutex_unlock(&availableCyclists[i]);
    }
  }
}

int main(int argc, char ** argv) {
  int i, *id, column = 0, remainingCyclists, currentCyclist, j;
  int *sortedCyclists;
  d = atoi(argv[1]);
  n = atoi(argv[2]);

  currentCyclist = n;

  threads = malloc(n*sizeof(pthread_t));
  availableCyclists = malloc(n*sizeof(pthread_mutex_t));

  for (i = 0; i < n; i++) {
    pthread_mutex_init(&availableCyclists[i], NULL);
    pthread_mutex_lock(&availableCyclists[i]);
  }

  id = malloc(n*sizeof(int));

  for (i = 0; i < 10; i++) {
    track[i] = malloc(d*sizeof(int));
    
    for (j = 0; j < d; j++)
      track[i][j] = 0;
  }

  cyclists = malloc(n*sizeof(Cyclist));
  sortedCyclists = malloc(n*sizeof(int));

  if (n % 5 != 0) {
    remainingCyclists = (n % 5);
    i = 0;
    while (remainingCyclists != 0) {
      track[i][column] = currentCyclist;
      cyclists[currentCyclist - 1].columnPosition = column;
      cyclists[currentCyclist - 1].linePosition = i;
      cyclists[currentCyclist - 1].velocity = 1;
      cyclists[currentCyclist - 1].laps = 0;
      cyclists[currentCyclist - 1].broken = 0;
      cyclists[currentCyclist - 1].id = currentCyclist;
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
      cyclists[currentCyclist - 1].laps = 0;
      cyclists[currentCyclist - 1].broken = 0;
      cyclists[currentCyclist - 1].id = currentCyclist;
      i += 1;
      remainingCyclists--;
      currentCyclist--;
    }
  }

  srand(time(NULL));

  if (debug)
    printTrack();

  pthread_barrier_init(&barrier, NULL, n + 1);

  for (i = 0; i < 10; i++) {
    sem[i] = malloc(d * sizeof(pthread_mutex_t));

    for (j = 0; j < d; j++) {
      pthread_mutex_init(&sem[i][j], NULL);
    }
  }
  
  for (i = 0; i < n; i++)
    sortedCyclists[i] = i;

  mergeSort(sortedCyclists, 0, n - 1);
  printRank(sortedCyclists, n - 1);

  for (i = 0; i < n; i++) {
    id[i] = i;
    pthread_create(&threads[i], NULL, thread, &id[i]);
  }

  remainingCyclists = n;

  judge(remainingCyclists, sortedCyclists);

  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
      pthread_mutex_destroy(&sem[i][d]);
    }
  }

  printf("FIM DA CORRIDA:\n");
  printTrack();

  free(cyclists);
  free(threads);
  free(id);

  for (i = 0; i < 10; i++)
    free(track[i]);

  return 0;
}
