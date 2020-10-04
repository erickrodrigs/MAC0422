#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char **argv) {
  FILE *output_file;
  int size;
  int i;
  int t0 = 0, dt, deadline;

  size = atoi(argv[2]);

  srand(time(NULL));

  output_file = fopen(argv[1], "w");

  for (i = 0; i < size; i++) {
    int n = 1 + rand() % 10;

    if (n >= 4 && n <= 5) {
      t0 = t0 + 1;
    }
    else if (n >= 6 && n <= 7) {
      t0 = t0 + 2;
    }
    else if (n >= 8) {
      t0 = t0 + rand() % 3 + 3;
    }

    dt = 1 + rand() % 3;
    deadline = t0 + dt + rand() % 3;

    fprintf(output_file, "p%d %d %d %d\n", i, t0, dt, deadline);
  }

  fclose(output_file);

  return 0;
}
