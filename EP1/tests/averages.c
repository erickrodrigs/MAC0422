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
    printf("Para o escalonador %d, a quantidade média de mudanças foi %f e a quantidade média de deadline descumprido foi %f\n", averageContext, averageCount);

    for (int i = 0; i < 30; i++) {
        averageCount += 

        averageContext += 
        
    }

    return 0;
}