#ifndef __QUEUE_H
#define __QUEUE_H

#include "schedule.h"

typedef struct cell {
  Schedule data;
  struct cell *next;
  struct cell *previous;
} Cell;

typedef struct queue {
  Cell *begin;
  Cell *end;
} Queue;

void queue_init(Queue *queue);
void insert_queue(Queue *queue, Schedule data);
int is_empty(Queue *queue);
Schedule remove_queue(Queue *queue);


#endif