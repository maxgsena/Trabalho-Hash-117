#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "tabela_hash.h"


static Estudante criar_estudante(unsigned int numero) {

    Estudante estudante = {0};

    snprintf(
        estudante.matricula,
        sizeof(estudante.matricula),
        "%09u",
        100000000u + numero
    );

    snprintf(
        estudante.nome,
        sizeof(estudante.nome),
        "Estudante %u",
        numero
    );

    snprintf(
        estudante.curso,
        sizeof(estudante.curso),
        "Engenharia de Computacao"
    );

    estudante.periodo = (numero % 10) + 1;

    return estudante;
}

int main(void) {

    printf("=== TESTE DE REDIMENSIONAMENTO ===\n\n");

    TabelaHash tabela = {0};

    
    assert(tabela_hash_inicializar(&tabela, 101));

    printf("Capacidade inicial: %zu\n", tabela.capacidade);

    

    for (unsigned int i = 0; i < 70; i++) {

        Estudante estudante = criar_estudante(i);

        assert(
            tabela_hash_inserir(&tabela, &estudante)
            == TABELA_INSERCAO_SUCESSO
        );
    }

    assert(tabela.capacidade == 101);
    assert(tabela.quantidade == 70);

    printf("70 estudantes cadastrados!\n");
    printf("Capacidade mantida: %zu\n\n", tabela.capacidade);

    

    assert(
        tabela_hash_remover(
            &tabela,
            "100000000"
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    assert(tabela.quantidade == 69);
    assert(tabela.removidos == 1);

    printf("Um estudante removido!\n");
    printf("Posicoes removidas: %zu\n\n", tabela.removidos);

    

    Estudante estudante70 = criar_estudante(70);

    assert(
        tabela_hash_inserir(&tabela, &estudante70)
        == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.capacidade == 101);
    assert(tabela.quantidade == 70);

    printf("70 registros ativos: sem redimensionamento!\n\n");

  

    Estudante estudante71 = criar_estudante(71);

    assert(
        tabela_hash_inserir(&tabela, &estudante71)
        == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.capacidade == 211);
    assert(tabela.quantidade == 71);

    printf("Limite de carga atingido!\n");
    printf("Redimensionamento realizado!\n");

    printf("Nova capacidade: %zu\n", tabela.capacidade);
    printf("Estudantes ativos: %zu\n\n", tabela.quantidade);


    Estudante encontrado;

    for (unsigned int i = 1; i <= 71; i++) {

        Estudante esperado = criar_estudante(i);

        assert(
            tabela_hash_buscar(
                &tabela,
                esperado.matricula,
                &encontrado
            ) == TABELA_BUSCA_ENCONTRADA
        );

        assert(strcmp(
            encontrado.nome,
            esperado.nome
        ) == 0);
    }

    printf("Todos os 71 estudantes foram preservados!\n");

   

    assert(
        tabela_hash_buscar(
            &tabela,
            "100000000",
            &encontrado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

    printf("Estudante removido continua inexistente!\n");

    

    assert(tabela.removidos == 0);

    printf("Marcadores de remocao eliminados!\n");

    

    double fator_carga =
        (double)tabela.quantidade / tabela.capacidade;

    printf("\n=== RESULTADO FINAL ===\n");

    printf("Capacidade: %zu\n", tabela.capacidade);
    printf("Estudantes: %zu\n", tabela.quantidade);
    printf("Removidos: %zu\n", tabela.removidos);

    printf("Fator de carga: %.2f%%\n", fator_carga * 100);

    printf("Colisoes: %zu\n", tabela.colisoes);
    printf("Sondagens: %zu\n", tabela.sondagens);

    tabela_hash_liberar(&tabela);

    printf("\nTodos os testes foram aprovados!\n");

    return 0;
}
