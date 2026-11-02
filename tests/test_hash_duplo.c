#include <assert.h>
#include <stdio.h>

#include "hash.h"

int main(void) {

    size_t h1_a;
    size_t h1_b;

    size_t passo_a;
    size_t passo_b;

    size_t indice;

    printf("=== TESTES DO HASH DUPLO ===\n\n");

    
    const char *matricula_a = "000000001";
    const char *matricula_b = "000000032";

    
    assert(hash_meio_do_quadrado(
        matricula_a, 101, &h1_a
    ));

    assert(hash_meio_do_quadrado(
        matricula_b, 101, &h1_b
    ));

    printf("Matrícula A: %s\n", matricula_a);
    printf("Matrícula B: %s\n\n", matricula_b);

    printf("H3 da matricula A: %zu\n", h1_a);
    printf("H3 da matricula B: %zu\n", h1_b);

    
    assert(h1_a == h1_b);

    printf("\nColisao confirmada!\n\n");

    
    assert(hash_duplo_passo(
        matricula_a, 101, &passo_a
    ));

    assert(hash_duplo_passo(
        matricula_b, 101, &passo_b
    ));

    printf("H2 da matricula A: %zu\n", passo_a);
    printf("H2 da matricula B: %zu\n\n", passo_b);

    
    assert(passo_a == 2);
    assert(passo_b == 33);
    assert(passo_a != passo_b);

    

    assert(hash_duplo_posicao(
        matricula_b, 101, 0, &indice
    ));
    assert(indice == 1);
    printf("Tentativa 0: posicao %zu\n", indice);

    assert(hash_duplo_posicao(
        matricula_b, 101, 1, &indice
    ));
    assert(indice == 34);
    printf("Tentativa 1: posicao %zu\n", indice);

    assert(hash_duplo_posicao(
        matricula_b, 101, 2, &indice
    ));
    assert(indice == 67);
    printf("Tentativa 2: posicao %zu\n\n", indice);

    
    int visitadas[101] = {0};

    for (size_t i = 0; i < 101; i++) {

        assert(hash_duplo_posicao(
            matricula_b, 101, i, &indice
        ));

        assert(visitadas[indice] == 0);

        visitadas[indice] = 1;
    }

    for (size_t i = 0; i < 101; i++) {
        assert(visitadas[i] == 1);
    }

    printf("Todas as 101 posicoes foram visitadas!\n");

    
    assert(!hash_duplo_passo(
        matricula_a, 100, &passo_a
    ));

    printf("Capacidade nao prima rejeitada!\n");

    printf("\nTodos os testes foram aprovados!\n");

    return 0;
}
