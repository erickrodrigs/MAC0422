#ifndef __CYCLIST_H
#define __CYCLIST_H

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

#endif
