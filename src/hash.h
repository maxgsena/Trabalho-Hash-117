#ifndef HASH_H
#define HASH_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_DIGITOS_MATRICULA_HASH 9


bool converter_matricula(const char *matricula, uint64_t *chave);


bool hash_meio_do_quadrado(
    const char *matricula,
    size_t capacidade,
    size_t *indice
);

#endif
