#ifndef __STACK_H
#define __STACK_H

typedef struct stackNode {
  int id;
  int size;
  struct stackNode *next;
} StackNode;

int empty(StackNode *stack);
int stackSize(StackNode *stack);
int top(StackNode *stack);
StackNode *push(StackNode *stack, int id);
StackNode *pop(StackNode *stack);
void printStack(StackNode *stack);

#endif
