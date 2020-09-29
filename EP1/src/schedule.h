#ifndef __SCHEDULE_H
#define __SCHEDULE_H

#include <pthread.h>

#define MAX 10000

typedef struct schedule {
  char name[30];
  int t0;
  int dt;
  int tf;
  int deadline;
  int index;
  pthread_t thread;
} Schedule;

#endif
