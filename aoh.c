// aoh.c
// autores: joao gabriel miranda queiroz, maria eduarda gomes
// descricao: aoh = quicksort 3-way + mediana de tres, fallback para heapsort por limite de profundidade,

#include "sorts.h"
#include <stddef.h>

// incrementa comparacoes ao comparar chaves
static inline int cmp_inc(int x, int y, Metricas *m) {
    if (m) m->comparacoes++;
    return (x > y) - (x < y);
}

// troca e incrementa trocas
static inline void swap_inc(int *a, int *b, Metricas *m) {
    int t = *a; *a = *b; *b = t;
    if (m) m->trocas++;
}

// floor(log2(n)) sem math.h
static int floor_log2_int(int n) {
    int k = -1;
    unsigned int u = (n <= 0) ? 1u : (unsigned int)n;
    while (u) { u >>= 1; k++; }
    if (k < 0) k = 0;
    return k;
}

/* =============== insertion sort (intervalo) ==================== */

// aqui nao contamos trocas nos shifts nem na escrita final para manter
// a metrica "trocas" apenas para swaps reais (compatibilidade com bubble/quick)
static void insertion_sort_range(int *a, int lo, int hi, Metricas *m) {
    for (int i = lo + 1; i <= hi; ++i) {
        int key = a[i];
        int j = i - 1;
        for (;;) {
            if (j < lo) break;
            if (m) m->comparacoes++;           // a[j] > key
            if (a[j] > key) {
                a[j + 1] = a[j];               // movimento simples, nao conta trocas
                j--;
            } else {
                break;
            }
        }
        a[j + 1] = key;                         // escrita final, nao conta trocas
    }
}

/* =================== heapsort no intervalo ===================== */

static void heapify_range(int *a, int lo, int size, int i, Metricas *m) {
    for (;;) {
        int largest = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < size) {
            if (m) m->comparacoes++;          // a[lo+l] > a[lo+largest]
            if (a[lo + l] > a[lo + largest]) largest = l;
        }
        if (r < size) {
            if (m) m->comparacoes++;          // a[lo+r] > a[lo+largest]
            if (a[lo + r] > a[lo + largest]) largest = r;
        }
        if (largest != i) {
            swap_inc(&a[lo + i], &a[lo + largest], m);
            i = largest;
        } else {
            break;
        }
    }
}

static void heap_sort_range(int *a, int lo, int hi, Metricas *m) {
    int size = hi - lo + 1;
    if (size <= 1) return;
    for (int i = size / 2 - 1; i >= 0; --i) heapify_range(a, lo, size, i, m);
    for (int i = size - 1; i > 0; --i) {
        swap_inc(&a[lo], &a[lo + i], m);
        heapify_range(a, lo, i, 0, m);
    }
}

/* =============== deteccao de quase ordenado ==================== */

// amostra ate 256 pares adjacentes; se >= 85% em ordem, considera quase ordenado
static int is_nearly_sorted(const int *a, int n, Metricas *m) {
    if (n < 2) return 1;
    const int sample = 256;
    const int thr_num = 85; // porcentagem
    if (n - 1 <= sample) {
        int ok = 0;
        for (int i = 0; i < n - 1; ++i) {
            if (m) m->comparacoes++;          // a[i] <= a[i+1]
            if (a[i] <= a[i + 1]) ok++;
        }
        return ok * 100 >= thr_num * (n - 1);
    } else {
        int step = (n - 1) / sample;
        if (step < 1) step = 1;
        int ok = 0, tot = 0;
        for (int i = 0; i < n - 1 && tot < sample; i += step) {
            if (m) m->comparacoes++;          // a[i] <= a[i+1]
            if (a[i] <= a[i + 1]) ok++;
            tot++;
        }
        return ok * 100 >= thr_num * tot;
    }
}

/* =================== quicksort 3-way =========================== */

static int median3_idx(const int *a, int i, int j, int k, Metricas *m) {
    if (cmp_inc(a[i], a[j], m) < 0) {
        if (cmp_inc(a[j], a[k], m) < 0) return j;
        else if (cmp_inc(a[i], a[k], m) < 0) return k;
        else return i;
    } else {
        if (cmp_inc(a[i], a[k], m) < 0) return i;
        else if (cmp_inc(a[j], a[k], m) < 0) return k;
        else return j;
    }
}

static void quicksort_3way(int *a, int lo, int hi, int small_cutoff, int depth_limit, Metricas *m) {
    while (lo < hi) {
        int n = hi - lo + 1;

        // subvetor pequeno: insertion
        if (n <= small_cutoff) {
            insertion_sort_range(a, lo, hi, m);
            return;
        }

        // limite de profundidade: heapsort no intervalo
        if (depth_limit == 0) {
            heap_sort_range(a, lo, hi, m);
            return;
        }

        // pivo pela mediana de tres
        int mid = lo + n / 2;
        int p = median3_idx(a, lo, mid, hi, m);
        swap_inc(&a[lo], &a[p], m);
        int pivot = a[lo];

        // particao 3-way (dutch national flag)
        int lt = lo, i = lo + 1, gt = hi;
        while (i <= gt) {
            if (m) m->comparacoes++;          // a[i] <, > ou == pivot
            if (a[i] < pivot) {
                swap_inc(&a[lt], &a[i], m);
                lt++; i++;
            } else if (a[i] > pivot) {
                swap_inc(&a[i], &a[gt], m);
                gt--;
            } else {
                i++;
            }
        }

        // recursao no lado menor primeiro (tail recursion)
        int left_size = lt - lo;
        int right_size = hi - gt;
        depth_limit--;

        if (left_size < right_size) {
            if (lo < lt - 1) quicksort_3way(a, lo, lt - 1, small_cutoff, depth_limit, m);
            lo = gt + 1;  // tail call no lado maior
        } else {
            if (gt + 1 < hi) quicksort_3way(a, gt + 1, hi, small_cutoff, depth_limit, m);
            hi = lt - 1;
        }
    }
}

/* ======================= aoh sort ============================== */

void aoh_sort(int *v, size_t n, Metricas *m) {
    if (!v || n < 2) { if (m) { m->comparacoes = 0; m->trocas = 0; } return; }

    // zera metricas aqui, tempo sera medido no main
    m->comparacoes = 0ULL;
    m->trocas = 0ULL;

    // se quase ordenado, insertion direta
    if (is_nearly_sorted(v, (int)n, m)) {
        insertion_sort_range(v, 0, (int)n - 1, m);
        return;
    }

    // parametros do hibrido
    const int small_cutoff = 32;
    int depth_limit = 2 * floor_log2_int((int)n);

    quicksort_3way(v, 0, (int)n - 1, small_cutoff, depth_limit, m);
}
