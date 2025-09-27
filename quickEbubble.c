// arquivo unico para windows mingw
// implementa bubble sort e quicksort
// atende ao enunciado com tres condicoes tamanhos fixos tres repeticoes e medias

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// metricas de execucao
typedef struct {
    unsigned long long comparacoes;
    unsigned long long trocas;
    double segundos;
} Metricas;

// relogio de alta resolucao no windows
static double agora_seg(void) {
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER ctr;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&ctr);
    return (double)ctr.QuadPart / (double)freq.QuadPart;
}

// utilitarios basicos
static void zerar_metricas(Metricas *m){
    m->comparacoes = 0ULL;
    m->trocas      = 0ULL;
    m->segundos    = 0.0;
}

// tamanhos exigidos no enunciado
static size_t tamanhos[] = {100, 1000, 5000, 30000, 50000, 100000, 150000, 200000};

// preenche vetor conforme modo de entrada
// modo 0 crescente 1 decrescente 2 aleatorio
static void preencher_vetor(int *v, size_t n, int modo) {
    if (modo == 0) {
        for (size_t i = 0; i < n; ++i) v[i] = (int)i;
    } else if (modo == 1) {
        for (size_t i = 0; i < n; ++i) v[i] = (int)(n - 1 - i);
    } else {
        for (size_t i = 0; i < n; ++i) v[i] = rand();
    }
}

// copia conteudo de vetor
static void copiar(const int *src, int *dst, size_t n) {
    for (size_t i = 0; i < n; ++i) dst[i] = src[i];
}

// checa se vetor esta ordenado nao estrito
static int vetor_ordenado(const int *v, size_t n) {
    for (size_t i = 1; i < n; ++i)
        if (v[i-1] > v[i]) return 0;
    return 1;
}

// nome legivel do modo
static const char* nome_modo(int m) {
    return (m==0) ? "crescente" : (m==1) ? "decrescente" : "aleatorio";
}

// bubble sort de complexidade n quadrado
static void bubble_sort(int *v, size_t n, Metricas *m) {
    double t0 = agora_seg();
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
    m->segundos += (agora_seg() - t0);
}

// quicksort de complexidade n log n
// usa particao de hoare e pivo no meio
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

static void quicksort(int *v, size_t n, Metricas *m) {
    if (!v || n < 2) return;
    double t0 = agora_seg();
    quick_rec(v, 0, (int)n - 1, m);
    m->segundos += (agora_seg() - t0);
}

// roda cada caso tres vezes e imprime medias
typedef void (*SortFn)(int*, size_t, Metricas*);

static void rodar_bateria(const char *algonome, SortFn sortfn, int modo) {
    printf("\n[%s] Modo: %s\n", algonome, nome_modo(modo));
    printf("n;comparacoes;trocas;tempo_s\n");

    for (size_t ti = 0; ti < sizeof(tamanhos)/sizeof(tamanhos[0]); ++ti) {
        size_t n = tamanhos[ti];
        int *base = (int*)malloc(n * sizeof(int));
        int *work = (int*)malloc(n * sizeof(int));
        if (!base || !work) {
            fprintf(stderr, "Sem memoria (n=%zu)\n", n);
            free(base); free(work);
            exit(2);
        }

        preencher_vetor(base, n, modo);

        Metricas acc; zerar_metricas(&acc);
        for (int r = 0; r < 3; ++r) {
            copiar(base, work, n);
            Metricas m; zerar_metricas(&m);
            sortfn(work, n, &m);

            if (!vetor_ordenado(work, n)) {
                fprintf(stderr, "ERRO: vetor nao ficou ordenado (n=%zu)\n", n);
                free(base); free(work);
                exit(3);
            }
            acc.comparacoes += m.comparacoes;
            acc.trocas      += m.trocas;
            acc.segundos    += m.segundos;
        }

        acc.comparacoes /= 3;
        acc.trocas      /= 3;
        acc.segundos    /= 3.0;

        printf("%zu;%llu;%llu;%.6f\n",
               n,
               (unsigned long long)acc.comparacoes,
               (unsigned long long)acc.trocas,
               acc.segundos);

        free(base); free(work);
    }
}

// menu principal para escolher algoritmo e modo
static void menu_algoritmo(void) {
    printf("=== parte 1 comparacao de algoritmos de ordenacao ===\n");
    printf("escolha o algoritmo\n");
    printf("[0] bubble sort\n");
    printf("[1] quicksort\n");
    printf("[2] rodar ambos\n");
    printf("opcao: ");
}

static void menu_modo(void) {
    printf("\nescolha o tipo de entrada\n");
    printf("[0] vetor crescente\n");
    printf("[1] vetor decrescente\n");
    printf("[2] vetor aleatorio\n");
    printf("[3] rodar os tres\n");
    printf("opcao: ");
}

int main(void) {
    srand((unsigned)time(NULL));

    int alg = -1, modo = -1;
    menu_algoritmo();
    if (scanf("%d", &alg) != 1 || alg < 0 || alg > 2) {
        fprintf(stderr, "Opcao invalida.\n");
        return 1;
    }
    menu_modo();
    if (scanf("%d", &modo) != 1 || modo < 0 || modo > 3) {
        fprintf(stderr, "Opcao invalida.\n");
        return 1;
    }

    int algs[2]; int qalg = 0;
    if (alg == 2) { algs[0] = 0; algs[1] = 1; qalg = 2; }
    else { algs[0] = alg; qalg = 1; }

    int modos[3]; int qmod = 0;
    if (modo == 3) { modos[0]=0; modos[1]=1; modos[2]=2; qmod=3; }
    else { modos[0]=modo; qmod=1; }

    for (int ai = 0; ai < qalg; ++ai) {
        const char *anome = (algs[ai]==0) ? "Bubble" : "Quick";
        SortFn fn = (algs[ai]==0) ? bubble_sort : quicksort;

        for (int mi = 0; mi < qmod; ++mi) {
            rodar_bateria(anome, fn, modos[mi]);
        }
    }
    return 0;
}
