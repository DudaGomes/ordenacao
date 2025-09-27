#include "sorts.h"

static inline void qs_swap(int *a, int *b, Metricas *m) {
    int t = *a; *a = *b; *b = t;
    m->trocas++;
}

static int hoare_partition(int *v, int l, int r, Metricas *m) {
    int i = l - 1, j = r + 1;
    int pivo = v[(l + r) / 2];
    for (;;) {
        do { i++; m->comparacoes++; } while (v[i] < pivo);
        do { j--; m->comparacoes++; } while (v[j] > pivo);
        if (i >= j) return j;
        qs_swap(&v[i], &v[j], m);
    }
}

static void quick_rec(int *v, int l, int r, Metricas *m) {
    while (l < r) {
        int p = hoare_partition(v, l, r, m);
        if (p - l < r - (p + 1)) {
            quick_rec(v, l, p, m);
            l = p + 1;
        } else {
            quick_rec(v, p + 1, r, m);
            r = p;
        }
    }
}

void quicksort(int *v, size_t n, Metricas *m) {
    m->comparacoes = 0ULL;
    m->trocas = 0ULL;
    // m->segundos preenchido no main
    if (!v || n < 2) return;
    quick_rec(v, 0, (int)n - 1, m);
}
