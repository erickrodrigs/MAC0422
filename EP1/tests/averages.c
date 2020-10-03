#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main (int argc, char **argv) {
    int context_switch[30], count[30];
    int size, scheduler;
    double averageContext = 0, averageCount = 0, varianceCount = 0, varianceContext = 0;

    scheduler = atoi(argv[2]);

    for (int i = 0; i < 30; i++) {
        scanf("%d %d", &context_switch[i], &count[i]);
        averageContext += context_switch[i];
        averageCount += count[i];
        
    }
    averageContext /= 30;
    averageCount /= 30;
    printf("Para o escalonador %d, a quantidade média de mudanças foi %f e a quantidade média de deadline descumprido foi %f\n", scheduler, averageContext, averageCount);

    for (int i = 0; i < 30; i++){
        varianceCount += (count[i] - averageCount)*(count[i] - averageCount);

        varianceContext += (context_switch[i] - averageContext)*(context_switch[i] - averageContext);
    }
    varianceContext /= 30;
    varianceCount /= 30;

    printf("Mudança de contexto: Lado esquerdo: %f lado direito: %f\n", averageContext - 2.04252*sqrt(varianceContext/30), averageContext + 2.04252*sqrt(varianceContext/30));
    printf("Deadlines ultrapassadas: Lado esquerdo: %f lado direito: %f\n\n", averageCount - 2.04252*sqrt(varianceCount/30), averageCount + 2.04252*sqrt(varianceCount/30));
    return 0;
}