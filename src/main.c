#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tabela_hash.h"


static bool ler_linha(
    const char *mensagem,
    char *buffer,
    size_t tamanho
) {

    if (buffer == NULL || tamanho == 0) {
        return false;
    }

    printf("%s", mensagem);
    fflush(stdout);

    size_t i = 0;
    bool excedeu = false;
    int caractere;

    while ((caractere = getchar()) != '\n' &&
           caractere != EOF) {

        if (i < tamanho - 1) {
            buffer[i++] = (char)caractere;
        } else {
            excedeu = true;
        }
    }

    if (caractere == EOF && i == 0 && !excedeu) {
        return false;
    }

    buffer[i] = '\0';

    
    if (i > 0 && buffer[i - 1] == '\r') {
        buffer[i - 1] = '\0';
    }

    if (excedeu) {
        printf("Entrada muito longa! Tente novamente.\n");
        return false;
    }

    return true;
}


static bool ler_texto(
    const char *mensagem,
    char *buffer,
    size_t tamanho
) {

    if (!ler_linha(mensagem, buffer, tamanho)) {
        return false;
    }

    if (buffer[0] == '\0') {
        printf("Este campo nao pode ficar vazio!\n");
        return false;
    }

    return true;
}


static bool ler_periodo(unsigned int *periodo) {

    char texto[32];

    if (periodo == NULL ||
        !ler_linha("Periodo: ", texto, sizeof(texto))) {

        return false;
    }

    errno = 0;

    char *fim;

    unsigned long valor = strtoul(texto, &fim, 10);

    if (texto[0] == '-' ||
        fim == texto ||
        errno == ERANGE ||
        valor == 0 ||
        valor > UINT_MAX) {

        printf("Periodo invalido! Digite um inteiro positivo.\n");
        return false;
    }

    while (*fim != '\0' && isspace((unsigned char)*fim)) {
        fim++;
    }

    if (*fim != '\0') {
        printf("Periodo invalido!\n");
        return false;
    }

    *periodo = (unsigned int)valor;

    return true;
}


static bool ler_estudante(Estudante *estudante) {

    if (estudante == NULL) {
        return false;
    }

    *estudante = (Estudante){0};

    printf("\n========== DADOS DO ESTUDANTE ==========\n");

    if (!ler_texto(
        "Matricula: ",
        estudante->matricula,
        sizeof(estudante->matricula)
    )) {
        return false;
    }

    if (!ler_texto(
        "Nome: ",
        estudante->nome,
        sizeof(estudante->nome)
    )) {
        return false;
    }

    if (!ler_texto(
        "Curso: ",
        estudante->curso,
        sizeof(estudante->curso)
    )) {
        return false;
    }

    if (!ler_periodo(&estudante->periodo)) {
        return false;
    }

    return true;
}


static bool ler_dados_atualizados(Estudante *estudante) {

    if (estudante == NULL) {
        return false;
    }

    printf("\n========== NOVOS DADOS ==========\n");

    if (!ler_texto(
        "Novo nome: ",
        estudante->nome,
        sizeof(estudante->nome)
    )) {
        return false;
    }

    if (!ler_texto(
        "Novo curso: ",
        estudante->curso,
        sizeof(estudante->curso)
    )) {
        return false;
    }

    if (!ler_periodo(&estudante->periodo)) {
        return false;
    }

    return true;
}


static void exibir_estudante(const Estudante *estudante) {

    if (estudante == NULL) {
        return;
    }

    printf("\n========== DADOS DO ESTUDANTE ==========\n");

    printf("Matricula: %s\n", estudante->matricula);
    printf("Nome: %s\n", estudante->nome);
    printf("Curso: %s\n", estudante->curso);
    printf("Periodo: %u\n", estudante->periodo);
}


static void cadastrar_estudante(TabelaHash *tabela) {

    Estudante estudante;

    if (!ler_estudante(&estudante)) {
        printf("Cadastro cancelado devido a uma entrada invalida.\n");
        return;
    }

    size_t capacidade_anterior = tabela->capacidade;

    ResultadoInsercao resultado = tabela_hash_inserir(
        tabela,
        &estudante
    );

    if (resultado == TABELA_INSERCAO_SUCESSO) {

        printf("\nEstudante cadastrado com sucesso!\n");

        if (tabela->capacidade != capacidade_anterior) {

            printf("A tabela foi redimensionada!\n");
            printf("Capacidade anterior: %zu\n", capacidade_anterior);
            printf("Nova capacidade: %zu\n", tabela->capacidade);
        }

    } else if (resultado == TABELA_INSERCAO_DUPLICADA) {

        printf("\nJa existe um estudante com essa matricula!\n");

    } else if (resultado == TABELA_INSERCAO_CHEIA) {

        printf("\nNao foi possivel encontrar uma posicao disponivel.\n");

    } else if (resultado == TABELA_INSERCAO_FALHA_REDIMENSIONAMENTO) {

        printf("\nFalha ao redimensionar a tabela. Nenhum novo cadastro foi realizado.\n");

    } else {

        printf("\nDados invalidos. Verifique a matricula e os campos informados.\n");
    }
}


static void consultar_estudante(TabelaHash *tabela) {

    char matricula[TAM_MATRICULA];

    if (!ler_texto(
        "\nDigite a matricula: ",
        matricula,
        sizeof(matricula)
    )) {
        return;
    }

    Estudante encontrado;

    ResultadoBusca resultado = tabela_hash_buscar(
        tabela,
        matricula,
        &encontrado
    );

    if (resultado == TABELA_BUSCA_ENCONTRADA) {

        exibir_estudante(&encontrado);

    } else if (resultado == TABELA_BUSCA_NAO_ENCONTRADA) {

        printf("\nEstudante nao encontrado!\n");

    } else {

        printf("\nMatricula invalida!\n");
    }
}


static void atualizar_estudante(TabelaHash *tabela) {

    char matricula[TAM_MATRICULA];

    if (!ler_texto(
        "\nDigite a matricula do estudante: ",
        matricula,
        sizeof(matricula)
    )) {
        return;
    }

    Estudante atualizado;

    ResultadoBusca busca = tabela_hash_buscar(
        tabela,
        matricula,
        &atualizado
    );

    if (busca != TABELA_BUSCA_ENCONTRADA) {

        printf("\nEstudante nao encontrado ou matricula invalida!\n");
        return;
    }

    
    if (!ler_dados_atualizados(&atualizado)) {

        printf("Atualizacao cancelada.\n");
        return;
    }

    ResultadoModificacao resultado = tabela_hash_atualizar(
        tabela,
        matricula,
        &atualizado
    );

    if (resultado == TABELA_MODIFICACAO_SUCESSO) {

        printf("\nEstudante atualizado com sucesso!\n");

    } else {

        printf("\nNao foi possivel atualizar o estudante.\n");
    }
}


static void remover_estudante(TabelaHash *tabela) {

    char matricula[TAM_MATRICULA];

    if (!ler_texto(
        "\nDigite a matricula do estudante: ",
        matricula,
        sizeof(matricula)
    )) {
        return;
    }

    ResultadoModificacao resultado = tabela_hash_remover(
        tabela,
        matricula
    );

    if (resultado == TABELA_MODIFICACAO_SUCESSO) {

        printf("\nEstudante removido com sucesso!\n");

    } else if (resultado == TABELA_MODIFICACAO_NAO_ENCONTRADA) {

        printf("\nEstudante nao encontrado!\n");

    } else {

        printf("\nMatricula invalida!\n");
    }
}


static void exibir_menu(void) {

    printf("\n");
    printf("============================================\n");
    printf("      SISTEMA DE CADASTRO DE ESTUDANTES\n");
    printf("              TABELA HASH 117\n");
    printf("============================================\n");

    printf("1 - Cadastrar estudante\n");
    printf("2 - Consultar estudante\n");
    printf("3 - Atualizar estudante\n");
    printf("4 - Remover estudante\n");
    printf("5 - Listar estudantes\n");
    printf("6 - Exibir estrutura da tabela\n");
    printf("0 - Sair\n");

    printf("============================================\n");
}


int main(void) {

    TabelaHash tabela = {0};

    
    if (!tabela_hash_inicializar(&tabela, 101)) {

        printf("Erro ao inicializar a tabela hash!\n");

        return EXIT_FAILURE;
    }

    printf("Tabela hash inicializada com sucesso!\n");

    bool executando = true;

    while (executando) {

        char opcao[16];

        exibir_menu();

        if (!ler_linha("Escolha uma opcao: ", opcao, sizeof(opcao))) {
            break;
        }

        if (strcmp(opcao, "1") == 0) {

            cadastrar_estudante(&tabela);

        } else if (strcmp(opcao, "2") == 0) {

            consultar_estudante(&tabela);

        } else if (strcmp(opcao, "3") == 0) {

            atualizar_estudante(&tabela);

        } else if (strcmp(opcao, "4") == 0) {

            remover_estudante(&tabela);

        } else if (strcmp(opcao, "5") == 0) {

            tabela_hash_listar(&tabela);

        } else if (strcmp(opcao, "6") == 0) {

            tabela_hash_exibir_estrutura(&tabela);

        } else if (strcmp(opcao, "0") == 0) {

            printf("\nEncerrando o sistema...\n");

            executando = false;

        } else {

            printf("\nOpcao invalida! Tente novamente.\n");
        }
    }

    tabela_hash_liberar(&tabela);

    printf("Memoria liberada. Programa encerrado!\n");

    return EXIT_SUCCESS;
}
