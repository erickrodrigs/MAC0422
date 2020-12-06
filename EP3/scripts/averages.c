#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main (int argc, char **argv) {
    int size;
    double execution_time[30];
    double averageTime = 0;
    double varianceTime = 0;
    char *filename, *output_filename;
    FILE *input, *output;

    filename = argv[1];
    output_filename = argv[2];

    input = fopen(filename, "r");

    size = 0;

    while (!feof(input)) {
      if (fscanf(input, "%lf", &execution_time[size]) != 1) {
        continue;
      }
      averageTime += execution_time[size];

      size += 1;
    }

    fclose(input);

    averageTime /= 30;

    //output = fopen(output_filename, "w");

    fprintf(stdout, "Quantidade média de tempo de execução foi %lf\n\n", averageTime);

    for (int i = 0; i < size; i++){
      varianceTime += (execution_time[i] - averageTime) * (execution_time[i] - averageTime);
    }

    varianceTime /= 30;

    fprintf(stdout, "Tempo de execução: Lado esquerdo: %lf lado direito: %lf\n\n", averageTime - 2.04252*sqrt(varianceTime/30), averageTime + 2.04252*sqrt(varianceTime/30));

    //fclose(output);

    return 0;
}
