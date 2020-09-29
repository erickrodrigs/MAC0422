#include <stdio.h>
#include <stdlib.h>
#include "heap.h"

void insert(Schedule *heap, int *size, Schedule val) {
  if (*size == MAX) {
    printf("Maximum capacity!\n");
    return;
  }

  int i = *size;
  *size = *size + 1;

  heap[i] = val;

  while (i != 0 && heap[i].dt < heap[(i - 1)/2].dt) {
    Schedule x = heap[i];
    heap[i] = heap[(i - 1)/2];
    heap[(i - 1)/2] = x;

    i = (i - 1)/2;
  }
}

void remove_min(Schedule *heap, int *size) {
  if (*size == 0) {
    printf("Empty heap!\n");
    return;
  }

  *size = *size - 1;
  heap[0] = heap[*size];
  heapify(heap, size, 0);
}

void heapify(Schedule *heap, int *size, int i) {
  int smallest = i;
  
  if (2 * i + 1 < *size && heap[2 * i + 1].dt < heap[i].dt)
    smallest = 2 * i + 1;

  if (2 * i + 2 < *size && heap[2 * i + 2].dt < heap[smallest].dt)
    smallest = 2 * i + 2;

  if (smallest != i) {
    Schedule x = heap[smallest];
    heap[smallest] = heap[i];
    heap[i] = x;

    heapify(heap, size, smallest);
  }
}
