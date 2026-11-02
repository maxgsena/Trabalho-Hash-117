#include <stdio.h>
#include "tabela_hash.h"

int main(void) {

    
    Estudante aluno = {
        "202600117",
        "Ana Souza",
        "Engenharia de Computacao",
        2
    };

    
    EntradaHash entrada = {
        .estudante = aluno,
        .estado = ENTRADA_OCUPADA
    };

    
    TabelaHash tabela = {0};

    printf("=== TESTE DAS ESTRUTURAS ===\n\n");

    printf("Matricula: %s\n", aluno.matricula);
    printf("Nome: %s\n", aluno.nome);
    printf("Curso: %s\n", aluno.curso);
    printf("Periodo: %u\n", aluno.periodo);

    printf("\nEstado da entrada: %d\n", entrada.estado);

    printf("\nCapacidade da tabela: %zu\n", tabela.capacidade);
    printf("Quantidade de estudantes: %zu\n", tabela.quantidade);

    return 0;
}
