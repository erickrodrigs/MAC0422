#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int empty(StackNode *stack) {
  return stack == NULL;
}

int stackSize(StackNode *stack) {
  if (empty(stack))
    return 0;
  return stack->size;
}

StackNode *push(StackNode *stack, int id) {
  StackNode *node = malloc(sizeof(StackNode));
  node->size = stackSize(stack) + 1;
  node->id = id;
  node->next = stack;
  return node;
}

StackNode *pop(StackNode *stack) {
  StackNode *next = NULL;

  if (!empty(stack)) {
    next = stack->next;
    free(stack);
  }
  return next;
}

int top(StackNode *stack) {
  if (!empty(stack))
    return stack->id;
  return -1;
}

void printStack(StackNode *stack) {
  StackNode *current = stack;
  int size = stackSize(stack);
  int *ranking = malloc(size * sizeof(int));

  for (int i = 0; i < size; i++) {
    ranking[i] = current->id;
    current = current->next;
  }

  for (int i = size - 1, j = 1; i >= 0; i--, j++) {
    printf("Ciclista %d está %dº lugar\n", ranking[i] + 1, j);
  }

  free(ranking);
}
