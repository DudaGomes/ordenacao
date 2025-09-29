#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include "sorts.h"

// menu consolidado (inclui aoh)
static void menu_alg(void) {
    printf("=== parte 1/2 comparacao de algoritmos de ordenacao ===\n");
    printf("escolha o algoritmo\n");
    printf("[0] bubble sort\n");
    printf("[1] quicksort\n");
    printf("[2] aoh (hibrido)\n");
    printf("[3] rodar os tres\n");
    printf("opcao: ");
}

// relogio de alta resolucao
static double agora_seg(void) {
    static LARGE_INTEGER freq = {0};
    LARGE_INTEGER ctr;
    if (freq.QuadPart == 0) QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&ctr);
    return (double)ctr.QuadPart / (double)freq.QuadPart;
}

// tamanhos completos do enunciado
static size_t tamanhos_full[] = {100, 1000, 5000, 30000, 50000, 100000, 150000, 200000};
static size_t qt_full = sizeof(tamanhos_full)/sizeof(tamanhos_full[0]);

// tamanhos pequenos para apresentacao
static size_t tamanhos_demo[] = {100, 200, 300, 400, 500};
static size_t qt_demo = sizeof(tamanhos_demo)/sizeof(tamanhos_demo[0]);

// preenche vetor conforme modo 0 crescente 1 decrescente 2 aleatorio
static void preencher_vetor(int *v, size_t n, int modo) {
    if (modo == 0) {
        for (size_t i = 0; i < n; ++i) v[i] = (int)i;
    } else if (modo == 1) {
        for (size_t i = 0; i < n; ++i) v[i] = (int)(n - 1 - i);
    } else {
        for (size_t i = 0; i < n; ++i) v[i] = rand();
    }
}

// copia vetor
static void copiar(const int *src, int *dst, size_t n) {
    for (size_t i = 0; i < n; ++i) dst[i] = src[i];
}

// checa ordenacao
static int vetor_ordenado(const int *v, size_t n) {
    for (size_t i = 1; i < n; ++i)
        if (v[i-1] > v[i]) return 0;
    return 1;
}

// nome do modo
static const char* nome_modo(int m) {
    return (m==0) ? "crescente" : (m==1) ? "decrescente" : "aleatorio";
}

// executa bateria de testes com 3 repeticoes e medias
typedef void (*SortFn)(int*, size_t, Metricas*);

static void rodar_bateria(const char *algonome, SortFn sortfn, int modo, const size_t *tams, size_t qt) {
    printf("\n[%s] modo %s\n", algonome, nome_modo(modo));
    printf("n;comparacoes;trocas;tempo_s\n");

    for (size_t ti = 0; ti < qt; ++ti) {
        size_t n = tams[ti];
        int *base = (int*)malloc(n * sizeof(int));
        int *work = (int*)malloc(n * sizeof(int));
        if (!base || !work) {
            fprintf(stderr, "sem memoria n=%zu\n", n);
            free(base); free(work);
            exit(2);
        }

        preencher_vetor(base, n, modo);

        unsigned long long somac = 0ULL;
        unsigned long long somat = 0ULL;
        double somatempo = 0.0;

        for (int r = 0; r < 3; ++r) {
            copiar(base, work, n);

            double t0 = agora_seg();
            Metricas m;
            sortfn(work, n, &m);
            double t1 = agora_seg();

            if (!vetor_ordenado(work, n)) {
                fprintf(stderr, "erro vetor nao ordenado n=%zu\n", n);
                free(base); free(work);
                exit(3);
            }

            somac += m.comparacoes;
            somat += m.trocas;
            somatempo += (t1 - t0);
        }

        unsigned long long comp_med = somac / 3ULL;
        unsigned long long troca_med = somat / 3ULL;
        double tempo_med = somatempo / 3.0;

        printf("%zu;%llu;%llu;%.6f\n",
               n,
               (unsigned long long)comp_med,
               (unsigned long long)troca_med,
               tempo_med);

        free(base); free(work);
    }
}

// menu de modo
static void menu_modo(void) {
    printf("\nescolha o tipo de entrada\n");
    printf("[0] vetor crescente\n");
    printf("[1] vetor decrescente\n");
    printf("[2] vetor aleatorio\n");
    printf("[3] rodar os tres\n");
    printf("opcao: ");
}

// menu de conjunto de tamanhos
static void menu_tamanho(void) {
    printf("\nescolha o conjunto de tamanhos\n");
    printf("[0] apresentacao ate 500\n");
    printf("[1] todos do enunciado\n");
    printf("opcao: ");
}

int main(void) {
    srand((unsigned)time(NULL));

    int alg = -1, modo = -1, conj = -1;
    menu_alg();
    if (scanf("%d", &alg) != 1 || alg < 0 || alg > 3) { fprintf(stderr, "opcao invalida\n"); return 1; }
    menu_modo();
    if (scanf("%d", &modo) != 1 || modo < 0 || modo > 3) { fprintf(stderr, "opcao invalida\n"); return 1; }
    menu_tamanho();
    if (scanf("%d", &conj) != 1 || conj < 0 || conj > 1) { fprintf(stderr, "opcao invalida\n"); return 1; }

    const size_t *tams = (conj == 0) ? tamanhos_demo : tamanhos_full;
    size_t qt = (conj == 0) ? qt_demo : qt_full;

    int algs[3]; int qalg = (alg == 3) ? 3 : 1;
    if (qalg == 1) {
        algs[0] = alg;         // 0 bubble, 1 quick, 2 aoh
    } else {
        algs[0] = 0; algs[1] = 1; algs[2] = 2;
    }

    int modos[3]; int qmod = (modo == 3) ? 3 : 1;
    modos[0] = (modo == 3) ? 0 : modo;
    if (qmod == 3) { modos[1] = 1; modos[2] = 2; }

    for (int ai = 0; ai < qalg; ++ai) {
        const char *anome;
        SortFn fn;
        if (algs[ai] == 0) { anome = "bubble"; fn = bubble_sort; }
        else if (algs[ai] == 1) { anome = "quick"; fn = quicksort; }
        else { anome = "aoh"; fn = aoh_sort; }

        for (int mi = 0; mi < qmod; ++mi) {
            rodar_bateria(anome, fn, modos[mi], tams, qt);
        }
    }
    return 0;
}
