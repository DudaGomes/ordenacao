#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h> // Para malloc, rand e srand
#include <time.h>   // Para clock e time

// Variável global para contar as comparações.
// Usamos long long para evitar estouro com vetores grandes.
long long comparacoes;

// Função de troca de elementos
void swap(int* xp, int* yp) {
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
}

// Bubble Sort modificado para contar comparações
void bubbleSort(int arr[], int n) {
    int i, j;
    bool swapped;
    for (i = 0; i < n - 1; i++) {
        swapped = false;
        for (j = 0; j < n - i - 1; j++) {
            // Incrementa o contador a cada comparação
            comparacoes++;
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (swapped == false)
            break;
    }
}

// --- Funções para Geração dos Vetores de Teste ---

// Gera um vetor com elementos em ordem crescente
void gerarVetorCrescente(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = i;
    }
}

// Gera um vetor com elementos em ordem decrescente
void gerarVetorDecrescente(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = n - 1 - i;
    }
}

// Gera um vetor com elementos aleatórios
void gerarVetorAleatorio(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        arr[i] = rand(); // rand() gera um número pseudoaleatório
    }
}


// --- Função Principal para Execução dos Testes ---

int main() {
    // Semente para o gerador de números aleatórios
    srand(time(NULL));

    // Comprimentos de vetores a serem testados, conforme o trabalho
    int comprimentos[] = {100, 1000, 5000, 30000, 50000, 100000, 150000, 200000};
    int num_comprimentos = sizeof(comprimentos) / sizeof(comprimentos[0]);
    int num_execucoes = 3; // Cada teste deve ser executado no mínimo 3 vezes

    printf("Iniciando analise de desempenho do Bubble Sort...\n");

    // Loop para cada tamanho de vetor
    for (int i = 0; i < num_comprimentos; i++) {
        int n = comprimentos[i];
        printf("\n--------------------------------------------------\n");
        printf("Tamanho do Vetor (N): %d\n", n);
        printf("--------------------------------------------------\n");

        // --- TESTE 1: VETOR CRESCENTE ---
        double tempo_total_crescente = 0;
        long long comp_total_crescente = 0;

        for (int j = 0; j < num_execucoes; j++) {
            int* arr = (int*)malloc(n * sizeof(int));
            if (arr == NULL) {
                printf("Falha na alocacao de memoria!\n");
                return 1;
            }
            gerarVetorCrescente(arr, n);
            
            comparacoes = 0; // Zera o contador antes de cada ordenação
            clock_t start = clock();
            bubbleSort(arr, n);
            clock_t end = clock();
            
            tempo_total_crescente += ((double)(end - start)) / CLOCKS_PER_SEC;
            comp_total_crescente += comparacoes;
            free(arr);
        }
        printf("Vetor Crescente (Melhor Caso):\n");
        printf("\tTempo medio de execucao: %f segundos\n", tempo_total_crescente / num_execucoes);
        printf("\tMedia de comparacoes: %lld\n", comp_total_crescente / num_execucoes);

        // --- TESTE 2: VETOR DECRESCENTE ---
        double tempo_total_decrescente = 0;
        long long comp_total_decrescente = 0;

        for (int j = 0; j < num_execucoes; j++) {
            int* arr = (int*)malloc(n * sizeof(int));
             if (arr == NULL) {
                printf("Falha na alocacao de memoria!\n");
                return 1;
            }
            gerarVetorDecrescente(arr, n);

            comparacoes = 0;
            clock_t start = clock();
            bubbleSort(arr, n);
            clock_t end = clock();

            tempo_total_decrescente += ((double)(end - start)) / CLOCKS_PER_SEC;
            comp_total_decrescente += comparacoes;
            free(arr);
        }
        printf("Vetor Decrescente (Pior Caso):\n");
        printf("\tTempo medio de execucao: %f segundos\n", tempo_total_decrescente / num_execucoes);
        printf("\tMedia de comparacoes: %lld\n", comp_total_decrescente / num_execucoes);

        // --- TESTE 3: VETOR ALEATÓRIO ---
        double tempo_total_aleatorio = 0;
        long long comp_total_aleatorio = 0;

        for (int j = 0; j < num_execucoes; j++) {
            int* arr = (int*)malloc(n * sizeof(int));
             if (arr == NULL) {
                printf("Falha na alocacao de memoria!\n");
                return 1;
            }
            gerarVetorAleatorio(arr, n);
            
            comparacoes = 0;
            clock_t start = clock();
            bubbleSort(arr, n);
            clock_t end = clock();

            tempo_total_aleatorio += ((double)(end - start)) / CLOCKS_PER_SEC;
            comp_total_aleatorio += comparacoes;
            free(arr);
        }
        printf("Vetor Aleatorio (Caso Medio):\n");
        printf("\tTempo medio de execucao: %f segundos\n", tempo_total_aleatorio / num_execucoes);
        printf("\tMedia de comparacoes: %lld\n", comp_total_aleatorio / num_execucoes);
    }

    return 0;
}