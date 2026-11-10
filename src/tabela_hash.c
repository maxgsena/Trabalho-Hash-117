#include "tabela_hash.h"

#include "hash.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define FATOR_CARGA_MAXIMO 0.70


static ResultadoBusca localizar_indice(
    TabelaHash *tabela,
    const char *matricula,
    size_t *indice_encontrado
);

static bool estudante_valido(const Estudante *estudante) {

    if (estudante == NULL) {
        return false;
    }

    if (memchr(
        estudante->matricula,
        '\0',
        sizeof(estudante->matricula)
    ) == NULL) {
        return false;
    }

    if (memchr(
        estudante->nome,
        '\0',
        sizeof(estudante->nome)
    ) == NULL) {
        return false;
    }

    if (memchr(
        estudante->curso,
        '\0',
        sizeof(estudante->curso)
    ) == NULL) {
        return false;
    }

    uint64_t chave;

    return converter_matricula(
        estudante->matricula,
        &chave
    );
}

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


static bool calcular_proxima_capacidade(
    size_t atual,
    size_t *proxima
) {

    if (proxima == NULL) {
        return false;
    }

    
    if (atual > (SIZE_MAX - 1) / 2) {
        return false;
    }

    size_t candidato = atual * 2 + 1;

    
    while (!hash_capacidade_valida(candidato)) {

        if (candidato > SIZE_MAX - 2) {
            return false;
        }

        candidato += 2;
    }

    *proxima = candidato;

    return true;
}


static bool tabela_hash_rehash(
    TabelaHash *tabela,
    size_t nova_capacidade
) {

    if (tabela == NULL ||
        tabela->entradas == NULL ||
        nova_capacidade <= tabela->capacidade ||
        nova_capacidade <= tabela->quantidade ||
        !hash_capacidade_valida(nova_capacidade)) {

        return false;
    }

    
    EntradaHash *novas_entradas = calloc(
        nova_capacidade,
        sizeof(EntradaHash)
    );

    if (novas_entradas == NULL) {
        return false;
    }

    size_t colisoes_rehash = 0;
    size_t sondagens_rehash = 0;

    
    for (size_t i = 0; i < tabela->capacidade; i++) {

        
        if (tabela->entradas[i].estado != ENTRADA_OCUPADA) {
            continue;
        }

        Estudante estudante = tabela->entradas[i].estudante;

        size_t indice;
        size_t passo;

        
        if (!hash_meio_do_quadrado(
            estudante.matricula,
            nova_capacidade,
            &indice
        )) {

            free(novas_entradas);
            return false;
        }

        
        if (!hash_duplo_passo(
            estudante.matricula,
            nova_capacidade,
            &passo
        )) {

            free(novas_entradas);
            return false;
        }

        
        bool colisao_inicial =
            novas_entradas[indice].estado == ENTRADA_OCUPADA;

        bool inserido = false;

        
        for (size_t tentativa = 0;
             tentativa < nova_capacidade;
             tentativa++) {

            sondagens_rehash++;

            if (novas_entradas[indice].estado != ENTRADA_OCUPADA) {

                novas_entradas[indice].estudante = estudante;

                novas_entradas[indice].estado = ENTRADA_OCUPADA;

                if (colisao_inicial) {
                    colisoes_rehash++;
                }

                inserido = true;

                break;
            }

            indice = proxima_posicao(
                indice,
                passo,
                nova_capacidade
            );
        }

        
        if (!inserido) {
            free(novas_entradas);
            return false;
        }
    }

  

    EntradaHash *vetor_antigo = tabela->entradas;

    tabela->entradas = novas_entradas;

    tabela->capacidade = nova_capacidade;

    
    tabela->removidos = 0;

    
    tabela->colisoes += colisoes_rehash;

    tabela->sondagens += sondagens_rehash;

    
    free(vetor_antigo);

    return true;
}


ResultadoInsercao tabela_hash_inserir(
    TabelaHash *tabela,
    const Estudante *estudante
) {

    if (tabela == NULL ||
        tabela->entradas == NULL ||
        !estudante_valido(estudante)) {

        return TABELA_INSERCAO_PARAMETRO_INVALIDO;
    }

   
    double fator_previsto =
        (double)(tabela->quantidade + 1) /
        (double)tabela->capacidade;

    if (fator_previsto >= FATOR_CARGA_MAXIMO) {

       
        size_t indice_existente;

        ResultadoBusca situacao = localizar_indice(
            tabela,
            estudante->matricula,
            &indice_existente
        );

        if (situacao == TABELA_BUSCA_ENCONTRADA) {
            return TABELA_INSERCAO_DUPLICADA;
        }

        if (situacao == TABELA_BUSCA_PARAMETRO_INVALIDO) {
            return TABELA_INSERCAO_PARAMETRO_INVALIDO;
        }

        
        size_t nova_capacidade;

        if (!calcular_proxima_capacidade(
            tabela->capacidade,
            &nova_capacidade
        )) {

            return TABELA_INSERCAO_FALHA_REDIMENSIONAMENTO;
        }

       
        if (!tabela_hash_rehash(
            tabela,
            nova_capacidade
        )) {

            return TABELA_INSERCAO_FALHA_REDIMENSIONAMENTO;
        }
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



static ResultadoBusca localizar_indice(
    TabelaHash *tabela,
    const char *matricula,
    size_t *indice_encontrado
) {

    if (tabela == NULL ||
        tabela->entradas == NULL ||
        matricula == NULL ||
        indice_encontrado == NULL) {

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

                *indice_encontrado = indice;

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

ResultadoBusca tabela_hash_buscar(
    TabelaHash *tabela,
    const char *matricula,
    Estudante *resultado
) {

    if (resultado == NULL) {
        return TABELA_BUSCA_PARAMETRO_INVALIDO;
    }

    size_t indice;

    ResultadoBusca situacao = localizar_indice(
        tabela,
        matricula,
        &indice
    );

    if (situacao != TABELA_BUSCA_ENCONTRADA) {
        return situacao;
    }

    *resultado = tabela->entradas[indice].estudante;

    return TABELA_BUSCA_ENCONTRADA;
}


ResultadoModificacao tabela_hash_atualizar(
    TabelaHash *tabela,
    const char *matricula,
    const Estudante *atualizado
) {

    if (matricula == NULL ||
        !estudante_valido(atualizado)) {

        return TABELA_MODIFICACAO_PARAMETRO_INVALIDO;
    }

   
    if (strcmp(
        matricula,
        atualizado->matricula
    ) != 0) {

        return TABELA_MODIFICACAO_PARAMETRO_INVALIDO;
    }

    size_t indice;

    ResultadoBusca situacao = localizar_indice(
        tabela,
        matricula,
        &indice
    );

    if (situacao == TABELA_BUSCA_PARAMETRO_INVALIDO) {
        return TABELA_MODIFICACAO_PARAMETRO_INVALIDO;
    }

    if (situacao == TABELA_BUSCA_NAO_ENCONTRADA) {
        return TABELA_MODIFICACAO_NAO_ENCONTRADA;
    }

  
    tabela->entradas[indice].estudante = *atualizado;

    return TABELA_MODIFICACAO_SUCESSO;
}


ResultadoModificacao tabela_hash_remover(
    TabelaHash *tabela,
    const char *matricula
) {

    size_t indice;

    ResultadoBusca situacao = localizar_indice(
        tabela,
        matricula,
        &indice
    );

    if (situacao == TABELA_BUSCA_PARAMETRO_INVALIDO) {
        return TABELA_MODIFICACAO_PARAMETRO_INVALIDO;
    }

    if (situacao == TABELA_BUSCA_NAO_ENCONTRADA) {
        return TABELA_MODIFICACAO_NAO_ENCONTRADA;
    }

  
    EntradaHash *entrada = &tabela->entradas[indice];

    entrada->estado = ENTRADA_REMOVIDA;

   
    memset(
        &entrada->estudante,
        0,
        sizeof(Estudante)
    );

  
    tabela->quantidade--;
    tabela->removidos++;

    return TABELA_MODIFICACAO_SUCESSO;
}