#ifndef __HEAP_H
#define __HEAP_H

#include "schedule.h"

void insert(Schedule *heap, int *size, Schedule val);
void remove_min(Schedule *heap, int *size);
void heapify(Schedule *heap, int *size, int i);

#endif
