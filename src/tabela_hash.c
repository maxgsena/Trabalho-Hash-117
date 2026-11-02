#include "tabela_hash.h"

#include "hash.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


bool tabela_hash_inicializar(
    TabelaHash *tabela,
    size_t capacidade
) {

    if (tabela == NULL) {
        return false;
    }

    if (!hash_capacidade_valida(capacidade)) {
        return false;
    }

    EntradaHash *entradas = calloc(
        capacidade,
        sizeof(EntradaHash)
    );

    if (entradas == NULL) {
        return false;
    }

    tabela->entradas = entradas;
    tabela->capacidade = capacidade;
    tabela->quantidade = 0;
    tabela->removidos = 0;
    tabela->colisoes = 0;
    tabela->sondagens = 0;

    return true;
}


void tabela_hash_liberar(TabelaHash *tabela) {

    if (tabela == NULL) {
        return;
    }

    free(tabela->entradas);

    *tabela = (TabelaHash){0};
}


static size_t proxima_posicao(
    size_t atual,
    size_t passo,
    size_t capacidade
) {

    if (atual >= capacidade - passo) {
        return atual - (capacidade - passo);
    }

    return atual + passo;
}


ResultadoInsercao tabela_hash_inserir(
    TabelaHash *tabela,
    const Estudante *estudante
) {

    if (tabela == NULL ||
        tabela->entradas == NULL ||
        estudante == NULL) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

    
    if (memchr(
        estudante->matricula,
        '\0',
        sizeof(estudante->matricula)
    ) == NULL) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

    
    uint64_t chave;

    if (!converter_matricula(
        estudante->matricula,
        &chave
    )) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

    size_t indice;
    size_t passo;

  
    if (!hash_meio_do_quadrado(
        estudante->matricula,
        tabela->capacidade,
        &indice
    )) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

  
    if (!hash_duplo_passo(
        estudante->matricula,
        tabela->capacidade,
        &passo
    )) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

    size_t primeira_removida = tabela->capacidade;

    bool colisao_inicial = false;

  
    for (size_t tentativa = 0;
         tentativa < tabela->capacidade;
         tentativa++) {

        EntradaHash *entrada = &tabela->entradas[indice];

        tabela->sondagens++;

        
        if (entrada->estado == ENTRADA_OCUPADA) {

            
            if (strcmp(
                entrada->estudante.matricula,
                estudante->matricula
            ) == 0) {

                return TABELA_INSERCAO_DUPLICADA;
            }

            
            if (tentativa == 0) {
                colisao_inicial = true;
            }
        }

      
        else if (entrada->estado == ENTRADA_REMOVIDA) {

            if (primeira_removida == tabela->capacidade) {
                primeira_removida = indice;
            }
        }

        
        else {

            size_t destino =
                primeira_removida != tabela->capacidade
                ? primeira_removida
                : indice;

            if (colisao_inicial) {
                tabela->colisoes++;
            }

            if (tabela->entradas[destino].estado ==
                ENTRADA_REMOVIDA) {

                tabela->removidos--;
            }

            tabela->entradas[destino].estudante = *estudante;

            tabela->entradas[destino].estado = ENTRADA_OCUPADA;

            tabela->quantidade++;

            return TABELA_INSERCAO_SUCESSO;
        }

        
        indice = proxima_posicao(
            indice,
            passo,
            tabela->capacidade
        );
    }


    if (primeira_removida != tabela->capacidade) {

        if (colisao_inicial) {
            tabela->colisoes++;
        }

        tabela->entradas[primeira_removida].estudante = *estudante;

        tabela->entradas[primeira_removida].estado = ENTRADA_OCUPADA;

        tabela->removidos--;
        tabela->quantidade++;

        return TABELA_INSERCAO_SUCESSO;
    }

    if (colisao_inicial) {
        tabela->colisoes++;
    }

    return TABELA_INSERCAO_CHEIA;
}


ResultadoBusca tabela_hash_buscar(
    TabelaHash *tabela,
    const char *matricula,
    Estudante *resultado
) {

    if (tabela == NULL ||
        tabela->entradas == NULL ||
        matricula == NULL ||
        resultado == NULL) {

        return TABELA_BUSCA_PARAMETRO_INVALIDO;
    }

    uint64_t chave;

    if (!converter_matricula(matricula, &chave)) {
        return TABELA_BUSCA_PARAMETRO_INVALIDO;
    }

    size_t indice;
    size_t passo;


    if (!hash_meio_do_quadrado(
        matricula,
        tabela->capacidade,
        &indice
    )) {

        return TABELA_BUSCA_PARAMETRO_INVALIDO;
    }


    if (!hash_duplo_passo(
        matricula,
        tabela->capacidade,
        &passo
    )) {

        return TABELA_BUSCA_PARAMETRO_INVALIDO;
    }

    for (size_t tentativa = 0;
         tentativa < tabela->capacidade;
         tentativa++) {

        EntradaHash *entrada = &tabela->entradas[indice];

        tabela->sondagens++;

     
        if (entrada->estado == ENTRADA_VAZIA) {
            return TABELA_BUSCA_NAO_ENCONTRADA;
        }

   
        if (entrada->estado == ENTRADA_OCUPADA) {

            if (strcmp(
                entrada->estudante.matricula,
                matricula
            ) == 0) {

                *resultado = entrada->estudante;

                return TABELA_BUSCA_ENCONTRADA;
            }
        }

       

        indice = proxima_posicao(
            indice,
            passo,
            tabela->capacidade
        );
    }

    return TABELA_BUSCA_NAO_ENCONTRADA;
}
