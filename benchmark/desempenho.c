#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "tabela_hash.h"

#define REPETICOES 5
#define CAPACIDADE_INICIAL 101


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


static double tempo_ms(clock_t inicio, clock_t fim) {

    return ((double)(fim - inicio) * 1000.0)
           / (double)CLOCKS_PER_SEC;
}


static bool executar_cenario(
    size_t quantidade,
    FILE *arquivo_csv
) {

    double soma_insercao = 0.0;
    double soma_busca_ok = 0.0;
    double soma_busca_ausente = 0.0;

    double soma_colisoes = 0.0;
    double soma_capacidade = 0.0;
    double soma_fator_carga = 0.0;

    double soma_sondagens_insercao = 0.0;
    double soma_sondagens_busca_ok = 0.0;
    double soma_sondagens_busca_ausente = 0.0;

    
    Estudante *alunos = malloc(quantidade * sizeof(Estudante));

    char (*matriculas_ausentes)[TAM_MATRICULA] = malloc(
        quantidade * sizeof(*matriculas_ausentes)
    );

    if (alunos == NULL || matriculas_ausentes == NULL) {

        fprintf(stderr, "Erro ao alocar os dados do benchmark.\n");
        free(alunos);
        free(matriculas_ausentes);
        return false;
    }

    for (size_t i = 0; i < quantidade; i++) {

        alunos[i] = criar_estudante((unsigned int)i);

        
        snprintf(
            matriculas_ausentes[i],
            TAM_MATRICULA,
            "%09u",
            300000000u + (unsigned int)i
        );
    }

    printf("\n========================================\n");
    printf("CENARIO: %zu ESTUDANTES\n", quantidade);
    printf("========================================\n");

    
    for (size_t repeticao = 0;
         repeticao < REPETICOES;
         repeticao++) {

        TabelaHash tabela = {0};

        if (!tabela_hash_inicializar(&tabela, CAPACIDADE_INICIAL)) {

            fprintf(stderr, "Erro ao inicializar tabela.\n");
            free(alunos);
            free(matriculas_ausentes);
            return false;
        }

        clock_t inicio;
        clock_t fim;

        
        inicio = clock();

        if (inicio == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            fprintf(stderr, "Erro ao iniciar temporizador.\n");
            return false;
        }

        for (size_t i = 0; i < quantidade; i++) {

            ResultadoInsercao resultado =
                tabela_hash_inserir(&tabela, &alunos[i]);

            if (resultado != TABELA_INSERCAO_SUCESSO) {

                fprintf(stderr, "Falha na insercao do estudante.\n");
                tabela_hash_liberar(&tabela);
                free(alunos);
                free(matriculas_ausentes);
                return false;
            }
        }

        fim = clock();

        if (fim == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            fprintf(stderr, "Erro ao finalizar temporizador.\n");
            return false;
        }

        double tempo_insercao = tempo_ms(inicio, fim);
        size_t sondagens_insercao = tabela.sondagens;
        size_t colisoes = tabela.colisoes;

        
        inicio = clock();

        if (inicio == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            return false;
        }

        for (size_t i = 0; i < quantidade; i++) {

            Estudante resultado;

            ResultadoBusca busca = tabela_hash_buscar(
                &tabela,
                alunos[i].matricula,
                &resultado
            );

            if (busca != TABELA_BUSCA_ENCONTRADA ||
                resultado.periodo != alunos[i].periodo ||
                strcmp(resultado.nome, alunos[i].nome) != 0) {

                fprintf(stderr, "Falha em busca bem-sucedida.\n");
                tabela_hash_liberar(&tabela);
                free(alunos);
                free(matriculas_ausentes);
                return false;
            }
        }

        fim = clock();

        if (fim == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            return false;
        }

        double tempo_busca_ok = tempo_ms(inicio, fim);
        size_t sondagens_apos_busca_ok = tabela.sondagens;

        
        inicio = clock();

        if (inicio == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            return false;
        }

        for (size_t i = 0; i < quantidade; i++) {

            Estudante resultado;

            ResultadoBusca busca = tabela_hash_buscar(
                &tabela,
                matriculas_ausentes[i],
                &resultado
            );

            if (busca != TABELA_BUSCA_NAO_ENCONTRADA) {

                fprintf(stderr, "Falha em busca malsucedida.\n");
                tabela_hash_liberar(&tabela);
                free(alunos);
                free(matriculas_ausentes);
                return false;
            }
        }

        fim = clock();

        if (fim == (clock_t)-1) {

            tabela_hash_liberar(&tabela);
            free(alunos);
            free(matriculas_ausentes);
            return false;
        }

        double tempo_busca_ausente = tempo_ms(inicio, fim);
        size_t sondagens_apos_busca_ausente = tabela.sondagens;

        
        size_t sondagens_busca_ok =
            sondagens_apos_busca_ok - sondagens_insercao;

        size_t sondagens_busca_ausente =
            sondagens_apos_busca_ausente - sondagens_apos_busca_ok;

        double fator_carga =
            (double)tabela.quantidade / (double)tabela.capacidade;

        
        soma_insercao += tempo_insercao;
        soma_busca_ok += tempo_busca_ok;
        soma_busca_ausente += tempo_busca_ausente;

        soma_colisoes += (double)colisoes;
        soma_capacidade += (double)tabela.capacidade;
        soma_fator_carga += fator_carga;

        soma_sondagens_insercao += (double)sondagens_insercao;
        soma_sondagens_busca_ok += (double)sondagens_busca_ok;
        soma_sondagens_busca_ausente += (double)sondagens_busca_ausente;

        printf(
            "Repeticao %zu/%d concluida | Capacidade: %zu | Fator: %.2f%%\n",
            repeticao + 1,
            REPETICOES,
            tabela.capacidade,
            fator_carga * 100.0
        );

        
        fprintf(
            arquivo_csv,
            "REPETICAO,%zu,%zu,%zu,%.6f,%zu,"
            "%.6f,%.6f,%.6f,"
            "%.6f,%.6f,%.6f,"
            "%zu,%zu,%zu\n",

            repeticao + 1,
            quantidade,
            tabela.capacidade,
            fator_carga,
            colisoes,

            tempo_insercao,
            tempo_busca_ok,
            tempo_busca_ausente,

            tempo_insercao * 1000.0 / (double)quantidade,
            tempo_busca_ok * 1000.0 / (double)quantidade,
            tempo_busca_ausente * 1000.0 / (double)quantidade,

            sondagens_insercao,
            sondagens_busca_ok,
            sondagens_busca_ausente
        );

        tabela_hash_liberar(&tabela);
    }

    
    double media_insercao = soma_insercao / REPETICOES;
    double media_busca_ok = soma_busca_ok / REPETICOES;
    double media_busca_ausente = soma_busca_ausente / REPETICOES;

    double media_colisoes = soma_colisoes / REPETICOES;
    double media_capacidade = soma_capacidade / REPETICOES;
    double media_fator_carga = soma_fator_carga / REPETICOES;

    double media_sondagens_insercao =
        soma_sondagens_insercao / REPETICOES;

    double media_sondagens_busca_ok =
        soma_sondagens_busca_ok / REPETICOES;

    double media_sondagens_busca_ausente =
        soma_sondagens_busca_ausente / REPETICOES;

   
    printf("\n========== MEDIAS DO CENARIO ==========\n");
    printf("Registros: %zu\n", quantidade);
    printf("Capacidade media: %.0f\n", media_capacidade);
    printf("Fator de carga medio: %.2f%%\n", media_fator_carga * 100.0);
    printf("Colisoes medias: %.2f\n", media_colisoes);

    printf("Insercao - lote: %.3f ms\n", media_insercao);
    printf("Busca bem-sucedida - lote: %.3f ms\n", media_busca_ok);
    printf("Busca malsucedida - lote: %.3f ms\n", media_busca_ausente);

    printf("Insercao - us/op: %.3f\n",
           media_insercao * 1000.0 / (double)quantidade);

    printf("Busca OK - us/op: %.3f\n",
           media_busca_ok * 1000.0 / (double)quantidade);

    printf("Busca ausente - us/op: %.3f\n",
           media_busca_ausente * 1000.0 / (double)quantidade);

    printf("Sondagens medias na insercao: %.2f\n",
           media_sondagens_insercao);

    printf("Sondagens medias na busca OK: %.2f\n",
           media_sondagens_busca_ok);

    printf("Sondagens medias na busca ausente: %.2f\n",
           media_sondagens_busca_ausente);

   
    fprintf(
        arquivo_csv,
        "MEDIA,0,%zu,%.0f,%.6f,%.2f,"
        "%.6f,%.6f,%.6f,"
        "%.6f,%.6f,%.6f,"
        "%.2f,%.2f,%.2f\n",

        quantidade,
        media_capacidade,
        media_fator_carga,
        media_colisoes,

        media_insercao,
        media_busca_ok,
        media_busca_ausente,

        media_insercao * 1000.0 / (double)quantidade,
        media_busca_ok * 1000.0 / (double)quantidade,
        media_busca_ausente * 1000.0 / (double)quantidade,

        media_sondagens_insercao,
        media_sondagens_busca_ok,
        media_sondagens_busca_ausente
    );

    free(alunos);
    free(matriculas_ausentes);

    return true;
}

int main(void) {

    const size_t cenarios[] = {
        100,
        1000,
        10000
    };

    const size_t total_cenarios =
        sizeof(cenarios) / sizeof(cenarios[0]);

    printf("============================================\n");
    printf("       BENCHMARK - TABELA HASH 117\n");
    printf("============================================\n");
    printf("Repeticoes por cenario: %d\n", REPETICOES);
    printf("Capacidade inicial: %d\n", CAPACIDADE_INICIAL);
    printf("Fator de carga maximo: 70%%\n");

    
    FILE *arquivo_csv = fopen("benchmark/resultados.csv", "w");

    if (arquivo_csv == NULL) {

        fprintf(
            stderr,
            "Erro ao criar benchmark/resultados.csv. "
            "Execute o programa na pasta principal do projeto.\n"
        );

        return EXIT_FAILURE;
    }

   
    fprintf(
        arquivo_csv,
        "tipo,repeticao,registros,capacidade,fator_carga,colisoes,"
        "insercao_lote_ms,busca_ok_lote_ms,busca_ausente_lote_ms,"
        "insercao_us_op,busca_ok_us_op,busca_ausente_us_op,"
        "sondagens_insercao,sondagens_busca_ok,"
        "sondagens_busca_ausente\n"
    );

    
    for (size_t i = 0; i < total_cenarios; i++) {

        if (!executar_cenario(cenarios[i], arquivo_csv)) {

            fclose(arquivo_csv);
            fprintf(stderr, "\nBenchmark interrompido por erro.\n");
            return EXIT_FAILURE;
        }
    }

    if (fclose(arquivo_csv) != 0) {

        fprintf(stderr, "Erro ao finalizar o arquivo CSV.\n");
        return EXIT_FAILURE;
    }

    printf("\n============================================\n");
    printf("BENCHMARK FINALIZADO!\n");
    printf("Resultados salvos em benchmark/resultados.csv\n");
    printf("============================================\n");

    return EXIT_SUCCESS;
}
