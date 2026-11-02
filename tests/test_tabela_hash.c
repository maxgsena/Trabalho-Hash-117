#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tabela_hash.h"

int main(void) {

    printf("=== TESTES DA TABELA HASH ===\n\n");

    
    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

    printf("Tabela inicializada!\n");


    Estudante ana = {
        "000000001",
        "Ana Souza",
        "Engenharia de Computacao",
        2
    };

    Estudante bruno = {
        "000000032",
        "Bruno Lima",
        "Ciencia da Computacao",
        3
    };

    Estudante encontrado;

    assert(
        tabela_hash_inserir(&tabela, &ana)
        == TABELA_INSERCAO_SUCESSO
    );

    printf("Ana cadastrada!\n");

  
    assert(
        tabela_hash_inserir(&tabela, &bruno)
        == TABELA_INSERCAO_SUCESSO
    );

    printf("Bruno cadastrado!\n");


    assert(tabela.quantidade == 2);

   
    assert(tabela.colisoes == 1);

    printf("Colisoes registradas: %zu\n", tabela.colisoes);

  
    assert(
        tabela_hash_inserir(&tabela, &ana)
        == TABELA_INSERCAO_DUPLICADA
    );

    printf("Matricula duplicada rejeitada!\n");

   
    assert(
        tabela_hash_buscar(
            &tabela,
            "000000001",
            &encontrado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(encontrado.nome, "Ana Souza") == 0);

    printf("Busca de Ana realizada!\n");

    
    assert(
        tabela_hash_buscar(
            &tabela,
            "000000032",
            &encontrado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(encontrado.nome, "Bruno Lima") == 0);

    printf("Busca de Bruno realizada!\n");

   
    assert(
        tabela_hash_buscar(
            &tabela,
            "999999999",
            &encontrado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

    printf("Busca inexistente testada!\n");

    
    assert(strcmp(
        tabela.entradas[1].estudante.nome,
        "Ana Souza"
    ) == 0);

    assert(strcmp(
        tabela.entradas[34].estudante.nome,
        "Bruno Lima"
    ) == 0);

    printf("Posicoes da tabela verificadas!\n");


    printf("\n=== METRICAS ===\n");

    printf("Capacidade: %zu\n", tabela.capacidade);
    printf("Quantidade: %zu\n", tabela.quantidade);
    printf("Colisoes: %zu\n", tabela.colisoes);
    printf("Sondagens: %zu\n", tabela.sondagens);

    
    tabela_hash_liberar(&tabela);

    assert(tabela.entradas == NULL);

    printf("\nMemoria liberada!\n");
    printf("Todos os testes foram aprovados!\n");

    return 0;
}
