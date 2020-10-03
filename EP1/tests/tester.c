#include <stdio.h>

int main(int argc, char **argv) {
  FILE *input_file, *output_file;
  int t0, dt, tf, tr, deadline, context_switch;
  char * name;
  int count = 0;

  input_file = fopen(argv[1], "r");
  output_file = fopen(argv[2], "r");

  while (!feof(input_file)) {
    if (fscanf(input_file, "%s %d %d %d", name, &t0, &dt, &deadline) != 4)
      continue;

    if (fscanf(output_file, "%s %d %d", name, &tf, &tr) != 3)
      continue;

    if (tf > deadline) {
      fprintf(stderr, "%s terminou depois do deadline! tf = %d | deadline = %d\n", name, tf, deadline);
      count += 1;
    }
  }

  fscanf(output_file, "%d", &context_switch);
  printf("%d\n", context_switch);
  printf("%d\n\n", count);
  
  fclose(input_file);
  fclose(output_file);

  return 0;
}
