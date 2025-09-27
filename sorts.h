#ifndef SORTS_H
#define SORTS_H

#include <stddef.h>

typedef struct {
    unsigned long long comparacoes;
    unsigned long long trocas;
    double segundos;
} Metricas;

void bubble_sort(int *v, size_t n, Metricas *m);
void quicksort(int *v, size_t n, Metricas *m);

#endif