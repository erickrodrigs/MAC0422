#include "sort.h"

void merge(int *vector, int a, int q, int b, Cyclist *cyclists) {
  int i, j, k;
  int * aux = malloc((b - a + 1)*sizeof(int));
  
  for (i = a; i <= q; i++)
    aux[i - a] = vector[i];

  for (i = b, j = q - a + 1; i >= q+1 ; i--, j++)
    aux[j] = vector[i];

  i = 0;
  j = b - a;

  for (k = a; k <= b; k++) {
    if (cyclists[aux[i]].laps < cyclists[aux[j]].laps) {
      vector[k] = aux[i];
      i++;
    }
    else if (cyclists[aux[i]].laps == cyclists[aux[j]].laps) {
      if (cyclists[aux[j]].runningTime > cyclists[aux[i]].runningTime) {
        vector[k] = aux[j];
        j--;
      }
      else {
        vector[k] = aux[i];
        i++;
      }
    }
    else {
      vector[k] = aux[j];
      j--;
    }
  }

  free(aux);
}

void mergeSort(int *vector, int a, int b, Cyclist *cyclists) {
  if (a < b) {
    int q = (a + b)/2;
    mergeSort(vector, a, q, cyclists);
    mergeSort(vector, q + 1, b, cyclists);

    merge(vector, a, q, b, cyclists);
  }
}
