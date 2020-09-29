#include "queue.h"
#include <stdlib.h>

void queue_init(Queue *queue) {
  queue->begin = queue->end = NULL;
}


void insert_queue(Queue *queue, Schedule data) {
  Cell *new;
  new = malloc(sizeof(Cell));
  new->previous = NULL;
  new->data = data;
  
  if (queue->end != NULL) {
    new->next = queue->end;
    queue->end->previous = new;
  }
  else {
    new->next = NULL;
    queue->begin = new;
  }

  queue->end = new;
}

int is_empty(Queue *queue) {
  return queue->begin == NULL;
}

Schedule remove_queue(Queue *queue) {
  Schedule data = queue->begin->data;

  if (queue->end != queue->begin) {
    queue->begin = queue->begin->previous;
    free(queue->begin->next);
    queue->begin->next = NULL;
  }
  else {
    free(queue->end);
    queue->end = queue->begin = NULL;
  }

  return data;
}