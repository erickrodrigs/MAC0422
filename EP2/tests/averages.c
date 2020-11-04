#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main (int argc, char **argv) {
    int memory[30];
    int size;
    double execution_time[30];
    double averageMemory = 0, averageTime = 0;
    double varianceMemory = 0, varianceTime = 0;
    char *filename, *output_filename;
    FILE *input, *output;

    filename = argv[1];
    output_filename = argv[2];

    input = fopen(filename, "r");

    size = 0;

    while (!feof(input)) {
      if (fscanf(input, "%d %lf", &memory[size], &execution_time[size]) != 2) {
        continue;
      }

      averageMemory += memory[size];
      averageTime += execution_time[size];

      size += 1;
    }

    fclose(input);

    averageMemory /= 30;
    averageTime /= 30;

    output = fopen(output_filename, "w");

    fprintf(output, "Quantidade média de uso de memória foi %lf\n", averageMemory);
    fprintf(output, "Quantidade média de tempo de execução foi %lf\n\n", averageTime);

    for (int i = 0; i < size; i++){
      varianceMemory += (memory[i] - averageMemory) * (memory[i] - averageMemory);
      varianceTime += (execution_time[i] - averageTime) * (execution_time[i] - averageTime);
    }

    varianceMemory /= 30;
    varianceTime /= 30;

    fprintf(output, "Uso de memória: Lado esquerdo: %lf lado direito: %lf\n", averageMemory - 2.04252*sqrt(varianceMemory/30), averageMemory + 2.04252*sqrt(varianceMemory/30));
    fprintf(output, "Tempo de execução: Lado esquerdo: %lf lado direito: %lf\n\n", averageTime - 2.04252*sqrt(varianceTime/30), averageTime + 2.04252*sqrt(varianceTime/30));

    fclose(output);

    return 0;
}
