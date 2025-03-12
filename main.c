#include <assert.h>
#include <stdio.h>
#include "helpers/vector.c"

void test_vector_create() {
    struct vector *vec = vector_create(sizeof(int));

    assert(vec != NULL);  // Verifica se a criação do vetor foi bem-sucedida
    assert(vec->esize == sizeof(int));  // Verifica o tamanho do elemento
    assert(vec->count == 0);  // O vetor deve estar vazio
    assert(vec->rindex == 0);  // O índice de leitura deve ser zero
    assert(vec->mindex == VECTOR_ELEMENT_INCREMENT);  // O vetor deve ter o tamanho inicial correto

    vector_free(vec);  // Libera a memória alocada
}

void test_vector_push() {
    struct vector *vec = vector_create(sizeof(int));
    int value1 = 10;
    int value2 = 20;

    vector_push(vec, &value1);  // Adiciona o valor 10
    vector_push(vec, &value2);  // Adiciona o valor 20

    assert(vec->count == 2);  // O vetor deve ter 2 elementos
    assert(*(int *)vector_at(vec, 0) == 10);  // O primeiro elemento deve ser 10
    assert(*(int *)vector_at(vec, 1) == 20);  // O segundo elemento deve ser 20

    vector_free(vec);  // Libera a memória alocada
}

void test_vector_pop() {
    struct vector *vec = vector_create(sizeof(int));
    int value1 = 10;
    int value2 = 20;

    vector_push(vec, &value1);  // Adiciona o valor 10
    vector_push(vec, &value2);  // Adiciona o valor 20

    vector_pop(vec);  // Remove o último valor (20)

    assert(vec->count == 1);  // O vetor deve ter 1 elemento
    assert(*(int *)vector_at(vec, 0) == 10);  // O único elemento deve ser 10

    vector_free(vec);  // Libera a memória alocada
}

void test_vector_set_peek_pointer() {
    struct vector *vec = vector_create(sizeof(int));
    int value1 = 10;
    int value2 = 20;

    vector_push(vec, &value1);  // Adiciona o valor 10
    vector_push(vec, &value2);  // Adiciona o valor 20

    vector_set_peek_pointer(vec, 0);  // Define o ponteiro de "peek" no primeiro elemento
    assert(*(int *)vector_peek(vec) == 10);  // Verifica se o valor "peek" é 10

    vector_set_peek_pointer(vec, 1);  // Define o ponteiro de "peek" no segundo elemento
    assert(*(int *)vector_peek(vec) == 20);  // Verifica se o valor "peek" é 20

    vector_free(vec);  // Libera a memória alocada
}

void test_vector_peek() {
    struct vector *vec = vector_create(sizeof(int));
    int value1 = 10;
    int value2 = 20;

    vector_push(vec, &value1);  // Adiciona o valor 10
    vector_push(vec, &value2);  // Adiciona o valor 20

    vector_set_peek_pointer(vec, 0);  // Define o ponteiro de "peek" no primeiro elemento
    assert(*(int *)vector_peek(vec) == 10);  // Verifica se o valor "peek" é 10

    vector_set_peek_pointer(vec, 1);  // Define o ponteiro de "peek" no segundo elemento
    assert(*(int *)vector_peek(vec) == 20);  // Verifica se o valor "peek" é 20

    vector_free(vec);  // Libera a memória alocada
}

int main() {
    test_vector_create();
    test_vector_push();
    test_vector_pop();
    test_vector_set_peek_pointer();
    test_vector_peek();

    printf("Todos os testes passaram!\n");

    return 0;
}