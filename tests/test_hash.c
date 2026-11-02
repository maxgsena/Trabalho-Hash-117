#include <assert.h>
#include <stdio.h>

#include "hash.h"

int main(void) {

    size_t indice;

    printf("=== TESTES DA FUNCAO H3 ===\n\n");


    assert(hash_meio_do_quadrado("202600117", 101, &indice));
    assert(indice == 33);

    printf("Matricula 202600117 -> Indice: %zu\n", indice);


    assert(hash_meio_do_quadrado("12345", 101, &indice));
    assert(indice == 96);

    printf("Matricula 12345 -> Indice: %zu\n", indice);


    assert(!hash_meio_do_quadrado("12A45", 101, &indice));

  
    assert(!hash_meio_do_quadrado("1234567890", 101, &indice));

    
    assert(!hash_meio_do_quadrado("12345", 0, &indice));

   
    assert(!hash_meio_do_quadrado("12345", 101, NULL));

    printf("\nTodos os testes foram aprovados!\n");

    return 0;
}
