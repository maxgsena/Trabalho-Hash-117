#ifndef TABELA_HASH_H
#define TABELA_HASH_H

#include <stdbool.h>
#include <stddef.h>

#include "estudante.h"


typedef enum {
    ENTRADA_VAZIA,
    ENTRADA_OCUPADA,
    ENTRADA_REMOVIDA
} EstadoEntrada;


typedef struct {
    Estudante estudante;
    EstadoEntrada estado;
} EntradaHash;


typedef struct {
    EntradaHash *entradas;

    size_t capacidade;
    size_t quantidade;
    size_t removidos;

    size_t colisoes;
    size_t sondagens;

} TabelaHash;


typedef enum {
    TABELA_INSERCAO_SUCESSO,
    TABELA_INSERCAO_DUPLICADA,
    TABELA_INSERCAO_CHEIA,
    TABELA_INSERCAO_PARAMETRO_INVALIDO
} ResultadoInsercao;


typedef enum {
    TABELA_BUSCA_ENCONTRADA,
    TABELA_BUSCA_NAO_ENCONTRADA,
    TABELA_BUSCA_PARAMETRO_INVALIDO
} ResultadoBusca;



typedef enum {
    TABELA_MODIFICACAO_SUCESSO,
    TABELA_MODIFICACAO_NAO_ENCONTRADA,
    TABELA_MODIFICACAO_PARAMETRO_INVALIDO
} ResultadoModificacao;


bool tabela_hash_inicializar(
    TabelaHash *tabela,
    size_t capacidade
);

void tabela_hash_liberar(TabelaHash *tabela);


ResultadoInsercao tabela_hash_inserir(
    TabelaHash *tabela,
    const Estudante *estudante
);

ResultadoBusca tabela_hash_buscar(
    TabelaHash *tabela,
    const char *matricula,
    Estudante *resultado
);


ResultadoModificacao tabela_hash_atualizar(
    TabelaHash *tabela,
    const char *matricula,
    const Estudante *atualizado
);


ResultadoModificacao tabela_hash_remover(
    TabelaHash *tabela,
    const char *matricula
);

#endif
