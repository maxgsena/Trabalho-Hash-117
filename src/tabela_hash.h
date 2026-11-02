#ifndef TABELA_HASH_H
#define TABELA_HASH_H

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

#endif
