#include "sorts.h"

void bubble_sort(int *v, size_t n, Metricas *m) {
    m->comparacoes = 0ULL;
    m->trocas = 0ULL;
    // m->segundos preenchido no main

    int trocou;
    for (size_t pass = 0; pass + 1 < n; ++pass) {
        trocou = 0;
        for (size_t i = 0; i + 1 < n - pass; ++i) {
            m->comparacoes++;
            if (v[i] > v[i+1]) {
                int t = v[i]; v[i] = v[i+1]; v[i+1] = t;
                m->trocas++;
                trocou = 1;
            }
        }
        if (!trocou) break;
    }
}
