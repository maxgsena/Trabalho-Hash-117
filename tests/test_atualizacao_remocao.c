#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tabela_hash.h"

int main(void) {

    printf("=== TESTES DE ATUALIZACAO E REMOCAO ===\n\n");

    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

   
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

    assert(
        tabela_hash_inserir(&tabela, &bruno)
        == TABELA_INSERCAO_SUCESSO
    );

    printf("Dois estudantes cadastrados!\n");
    printf("Colisoes: %zu\n\n", tabela.colisoes);

    

    Estudante bruno_atualizado = {
        "000000032",
        "Bruno Lima Atualizado",
        "Engenharia de Computacao",
        4
    };

    assert(
        tabela_hash_atualizar(
            &tabela,
            "000000032",
            &bruno_atualizado
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    printf("Teste 1: Atualizacao realizada!\n");

    
    assert(
        tabela_hash_buscar(
            &tabela,
            "000000032",
            &encontrado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(
        encontrado.nome,
        "Bruno Lima Atualizado"
    ) == 0);

    assert(strcmp(
        encontrado.curso,
        "Engenharia de Computacao"
    ) == 0);

    assert(encontrado.periodo == 4);

    printf("Dados atualizados conferidos!\n\n");

   

    Estudante chave_alterada = {
        "000000099",
        "Bruno Lima",
        "Engenharia de Computacao",
        4
    };

    assert(
        tabela_hash_atualizar(
            &tabela,
            "000000032",
            &chave_alterada
        ) == TABELA_MODIFICACAO_PARAMETRO_INVALIDO
    );

    printf("Teste 2: Alteracao de matricula rejeitada!\n\n");

   

    assert(
        tabela_hash_remover(
            &tabela,
            "000000001"
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    printf("Teste 3: Ana removida!\n");

    assert(tabela.quantidade == 1);
    assert(tabela.removidos == 1);

    
    assert(
        tabela.entradas[1].estado == ENTRADA_REMOVIDA
    );

    printf("Marcador de remocao confirmado!\n\n");

    
    assert(
        tabela_hash_buscar(
            &tabela,
            "000000032",
            &encontrado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(
        encontrado.nome,
        "Bruno Lima Atualizado"
    ) == 0);

    printf("Teste 4: Bruno continua acessivel apos a remocao!\n\n");

   

    assert(
        tabela_hash_buscar(
            &tabela,
            "000000001",
            &encontrado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

    printf("Teste 5: Ana nao foi encontrada!\n\n");

   

    assert(
        tabela_hash_remover(
            &tabela,
            "999999999"
        ) == TABELA_MODIFICACAO_NAO_ENCONTRADA
    );

    printf("Teste 6: Remocao inexistente tratada!\n\n");

   

    assert(
        tabela_hash_inserir(
            &tabela,
            &ana
        ) == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.quantidade == 2);
    assert(tabela.removidos == 0);

    assert(
        tabela.entradas[1].estado == ENTRADA_OCUPADA
    );

    printf("Teste 7: Posicao removida reutilizada!\n\n");

  
    printf("=== RESULTADO FINAL ===\n");

    printf("Capacidade: %zu\n", tabela.capacidade);
    printf("Estudantes: %zu\n", tabela.quantidade);
    printf("Posicoes removidas: %zu\n", tabela.removidos);
    printf("Colisoes: %zu\n", tabela.colisoes);
    printf("Sondagens: %zu\n", tabela.sondagens);

    tabela_hash_liberar(&tabela);

    printf("\nTodos os testes foram aprovados!\n");

    return 0;
}
