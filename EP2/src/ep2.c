#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

typedef struct cyclist {
  int id;
  int columnPosition;
  int linePosition;
  int velocity;
  int laps;
  int broken;
  int eliminated;
  int finished;
  double runningTime;
} Cyclist;

typedef struct node {
  int id;
  int size;
  struct node *next;
} Node;

int d, n;
int debug = 0;
int *track[10];
int *canContinue;
int countBroken = 0;
int maximumVelocity = 0;
struct timeval start;
Cyclist *cyclists;

//stack.h
int empty(Node *stack) {
  return stack == NULL;
}

int stackSize(Node *stack) {
  if (empty(stack))
    return 0;
  return stack->size;
}

Node *push(Node *stack, int id) {
  Node *node = malloc(sizeof(Node));
  node->size = stackSize(stack) + 1;
  node->id = id;
  node->next = stack;
  return node;
}

Node *pop(Node *stack) {
  Node *next = NULL;

  if (!empty(stack)) {
    next = stack->next;
    free(stack);
  }
  return next;
}

int top(Node *stack) {
  if (!empty(stack))
    return stack->id;
  return -1;
}

void printStack(Node *stack) {
  Node *current = stack;
  int size = stackSize(stack);
  int *ranking = malloc(size*sizeof(int));

  for (int i = 0; i < size; i++) {
    ranking[i] = current->id;
    current = current->next;
  }

  for (int i = size - 1, j = 1; i >= 0; i--, j++) {
    printf("Ciclista %d está %dº lugar\n", ranking[i] + 1, j);
  }

  free(ranking);
}

//stack.h 
pthread_mutex_t *sem[10], *availableCyclists, *lapsSem, randMutex;
pthread_barrier_t barrier;
pthread_t *threads;
Node **stacks;

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
// sort.h
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
    else if (cyclists[aux[i]].laps == cyclists[aux[j]].laps) {
      if (cyclists[aux[j]].runningTime > cyclists[aux[i]].runningTime) {
        vector[k] = aux[j];
        j--;
      }
      else {
        vector[k] = aux[i];
        i++;
      }
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
// sort.h

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
    return 2; // 60km/h
  else
    return 1; // 30km/h
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
      //processamento do juiz
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
  int i, j;

  printf("\n\n");

  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
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
  //int maximumVelocity;
  int lapCompleted;

  while (remainingCyclists > 1) {
    maximumVelocity = 0;
    for (int i = 0; i < n; i++) {
      if (cyclists[i].velocity == 3)
        maximumVelocity = 1;
    }//é possível checar lá em cima também
    
    // printf("%d\n", maximumVelocity);

    if (maximumVelocity)
      usleep(20000);
    else
      usleep(60000);
    

    pthread_barrier_wait(&barrier);
      //change position, canContinue= 0;
    pthread_barrier_wait(&barrier);

    if (debug)
      printTrack();

    //mergeSort(sortedCyclists, 0, n - 1);
    //printRank(sortedCyclists, n - 1);

    // for (int i = 0; i < n; i++) {
    //   if (!cyclists[i].broken && !cyclists[i].eliminated)
    //     printf("Ciclista %d fez %d voltas\n", cyclists[i].id, cyclists[i].laps);
    // }

    someoneHasLeft = 0;

    // verifica se o ciclista quebrou
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

    /* 

Para armazenar a posição em que ele completou a volta, durante o change position, inserimos na pilha na volta equivalente.

Vai ter uma variável local que corresponde ao lap que precisa ser elimanado. Começa valendo 2, depois 4, depois 6 ...

Lap completa:
  É uma lap que contém todos os ciclistas possíveis.

  Uma lap é completa sse todos os ciclistas i tais que ciclistas[i].broken = 0 têm ciclistas[i].lap >= lapLocal 

Último: 
  é um ciclistas[i].broken = 0 numa lap completa com maior posição na pilha.

Eliminação:
Acontece quando  o último passa na coluna 0.
*/
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
        while (cyclists[top(stacks[lap])].broken || cyclists[top(stacks[lap])].eliminated) //será que tem algum problema?
          stacks[lap] = pop(stacks[lap]);

        if (cyclists[top(stacks[lap])].columnPosition == 0) { //será que tem algum problema?
          currentCyclist = top(stacks[lap]);
          stacks[lap] = pop(stacks[lap]);
          cyclists[currentCyclist].eliminated = 1;
          line = cyclists[currentCyclist].linePosition;
          column = cyclists[currentCyclist].columnPosition;

          printf("Ciclista %d foi embora!! ", currentCyclist + 1);
          printf("Tempo final ciclista %d: %lf\n", currentCyclist + 1, cyclists[currentCyclist].runningTime);         
          remainingCyclists--;

          track[line][column] = 0;
          //elimina o topo da pilha da pista = cancela, printf, zerar a posição, diminuir remaining cyclists, broken = 1
          
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

    // if (remainingCyclists > 1) {
    //   if (someoneHasLeft) {
    //     pthread_barrier_destroy(&barrier);
    //     pthread_barrier_init(&barrier, NULL, remainingCyclists + 1);
    //   }

    //   for (int i = 0; i < n; i++)
    //     if (!cyclists[i].broken && !cyclists[i].eliminated)
    //       canContinue[i] = 1;
    // }
    // else {
    //   printf("CICLISTA VENCEDOR: %d\n", top(stacks[lap]) + 1);
    //   pthread_cancel(threads[top(stacks[lap])]);
    // }
  }
  pthread_barrier_destroy(&barrier);
  printf("QUEBRADOS: %d\n", countBroken);
}

int main(int argc, char ** argv) {
  int i, *id, column = 0, remainingCyclists, currentCyclist, j;
  int *sortedCyclists;

  debug = (argc == 4) ? 1 : 0;

  d = atoi(argv[1]);
  n = atoi(argv[2]);

  gettimeofday(&start, NULL);

  stacks = malloc(2*(n + 1)*sizeof(Node *));

  for (i = 0; i < 2*(n + 1); i++)
    stacks[i] = NULL;

  currentCyclist = n;

  threads = malloc(n*sizeof(pthread_t));
  availableCyclists = malloc(n*sizeof(pthread_mutex_t));
  lapsSem = malloc(2*(n + 1)*sizeof(pthread_mutex_t));

  for (i = 0; i < n; i++) {
    pthread_mutex_init(&availableCyclists[i], NULL);
    pthread_mutex_lock(&availableCyclists[i]);
  }

  for (i = 0; i < 2*(n + 1); i++)
    pthread_mutex_init(&lapsSem[i], NULL);

  pthread_mutex_init(&randMutex, NULL);

  id = malloc(n*sizeof(int));
  canContinue = malloc(n*sizeof(int));

  for (i = 0; i < n; i++)
    canContinue[i] = 0;

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
      cyclists[currentCyclist - 1].eliminated = 0;
      cyclists[currentCyclist - 1].id = currentCyclist;
      cyclists[currentCyclist - 1].finished = 0;
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
      cyclists[currentCyclist - 1].eliminated = 0;
      cyclists[currentCyclist - 1].id = currentCyclist;
      cyclists[currentCyclist - 1].finished = 0;
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

  for (i = 0; i < n; i++) {
    id[i] = i;
    pthread_create(&threads[i], NULL, thread, &id[i]);
  }

  remainingCyclists = n;

  judge(remainingCyclists);

  printf("\nFIM DA CORRIDA!!!\n");

  if (debug)
    printTrack();

  printf("\nCLASSIFICAÇÃO FINAL:\n");
  mergeSort(sortedCyclists, 0, n - 1);
  printRank(sortedCyclists, n - 1);

  for (i = 0; i < 10; i++) {
    for (j = 0; j < d; j++) {
      pthread_mutex_destroy(&sem[i][j]);
    }
    free(sem[i]);
    free(track[i]);
  }

  for (i = 0; i < n; i++) {
    pthread_join(threads[i], NULL);
    pthread_mutex_destroy(&availableCyclists[i]);
  }

  for (i = 0; i < 2*(n + 1); i++) {
    pthread_mutex_destroy(&lapsSem[i]);

    while (!empty(stacks[i]))
      stacks[i] = pop(stacks[i]);
  }

  pthread_mutex_destroy(&randMutex);

  free(availableCyclists);
  free(lapsSem);
  free(sortedCyclists);
  free(cyclists);
  free(threads);
  free(id);
  free(canContinue);
  free(stacks);

  return 0;
}
