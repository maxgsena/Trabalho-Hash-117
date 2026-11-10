
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "tabela_hash.h"


static Estudante criar_estudante(unsigned int numero) {

    Estudante estudante = {0};

    snprintf(
        estudante.matricula,
        sizeof(estudante.matricula),
        "%09u",
        numero
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


static bool encontrar_colisao_tripla(Estudante grupo[3]) {

    size_t contagens[101] = {0};

    unsigned int candidatos[101][3] = {0};

    for (unsigned int numero = 1;
         numero <= 100000;
         numero++) {

        Estudante estudante = criar_estudante(numero);

        size_t indice;

        if (!hash_meio_do_quadrado(
            estudante.matricula,
            101,
            &indice
        )) {
            return false;
        }

        if (contagens[indice] < 3) {

            candidatos[indice][contagens[indice]] = numero;

            contagens[indice]++;

            if (contagens[indice] == 3) {

                for (size_t i = 0; i < 3; i++) {
                    grupo[i] = criar_estudante(
                        candidatos[indice][i]
                    );
                }

                return true;
            }
        }
    }

    return false;
}


static size_t localizar_posicao(
    const TabelaHash *tabela,
    const char *matricula
) {

    for (size_t i = 0; i < tabela->capacidade; i++) {

        if (tabela->entradas[i].estado == ENTRADA_OCUPADA &&
            strcmp(
                tabela->entradas[i].estudante.matricula,
                matricula
            ) == 0) {

            return i;
        }
    }

    return tabela->capacidade;
}


static void teste_insercao_busca_duplicidade(void) {

    printf("\n[TC01, TC02 e TC03] Insercao, busca e duplicidade\n");

    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

    Estudante resultado = {0};

  
    assert(
        tabela_hash_buscar(
            &tabela,
            "999999999",
            &resultado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

  
    Estudante aluno = criar_estudante(123);

    assert(
        tabela_hash_inserir(
            &tabela,
            &aluno
        ) == TABELA_INSERCAO_SUCESSO
    );

    
    assert(
        tabela_hash_buscar(
            &tabela,
            aluno.matricula,
            &resultado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(resultado.nome, aluno.nome) == 0);

    
    assert(
        tabela_hash_inserir(
            &tabela,
            &aluno
        ) == TABELA_INSERCAO_DUPLICADA
    );

    assert(tabela.quantidade == 1);

    
    Estudante invalido = criar_estudante(124);

    strcpy(invalido.matricula, "12A45");

    assert(
        tabela_hash_inserir(
            &tabela,
            &invalido
        ) == TABELA_INSERCAO_PARAMETRO_INVALIDO
    );

    tabela_hash_liberar(&tabela);

    printf("APROVADO: insercao, busca e duplicidade.\n");
}


static void teste_colisoes_e_remocao(void) {

    printf("\n[TC04 e TC05] Colisoes e remocao\n");

    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

    Estudante grupo[3];

    
    assert(encontrar_colisao_tripla(grupo));

    size_t indice_inicial;

    assert(hash_meio_do_quadrado(
        grupo[0].matricula,
        101,
        &indice_inicial
    ));

    printf("Matriculas que colidem:\n");

    for (size_t i = 0; i < 3; i++) {

        size_t indice;

        assert(hash_meio_do_quadrado(
            grupo[i].matricula,
            101,
            &indice
        ));

        printf(
            "%s -> indice inicial %zu\n",
            grupo[i].matricula,
            indice
        );

        assert(indice == indice_inicial);

        assert(
            tabela_hash_inserir(
                &tabela,
                &grupo[i]
            ) == TABELA_INSERCAO_SUCESSO
        );
    }

   
    assert(tabela.colisoes >= 2);

    assert(tabela.quantidade == 3);

    
    size_t posicao_removida = localizar_posicao(
        &tabela,
        grupo[0].matricula
    );

    assert(posicao_removida == indice_inicial);

   
    assert(
        tabela_hash_remover(
            &tabela,
            grupo[0].matricula
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    assert(
        tabela.entradas[posicao_removida].estado
        == ENTRADA_REMOVIDA
    );

    assert(tabela.quantidade == 2);
    assert(tabela.removidos == 1);

    Estudante resultado;

    
    for (size_t i = 1; i < 3; i++) {

        assert(
            tabela_hash_buscar(
                &tabela,
                grupo[i].matricula,
                &resultado
            ) == TABELA_BUSCA_ENCONTRADA
        );
    }

    
    assert(
        tabela_hash_buscar(
            &tabela,
            grupo[0].matricula,
            &resultado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

    
    assert(
        tabela_hash_inserir(
            &tabela,
            &grupo[0]
        ) == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.quantidade == 3);
    assert(tabela.removidos == 0);

    assert(
        tabela.entradas[posicao_removida].estado
        == ENTRADA_OCUPADA
    );

    tabela_hash_liberar(&tabela);

    printf("APROVADO: tres chaves colidentes, remocao e reutilizacao.\n");
}


static void teste_atualizacao(void) {

    printf("\n[TC06] Atualizacao\n");

    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

    Estudante aluno = criar_estudante(200);

    assert(
        tabela_hash_inserir(
            &tabela,
            &aluno
        ) == TABELA_INSERCAO_SUCESSO
    );

    Estudante atualizado = aluno;

    strcpy(atualizado.nome, "Nome Atualizado");

    strcpy(atualizado.curso, "Engenharia de Software");

    atualizado.periodo = 8;

    
    assert(
        tabela_hash_atualizar(
            &tabela,
            aluno.matricula,
            &atualizado
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    Estudante resultado;

    assert(
        tabela_hash_buscar(
            &tabela,
            aluno.matricula,
            &resultado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(strcmp(resultado.nome, "Nome Atualizado") == 0);

    assert(strcmp(
        resultado.curso,
        "Engenharia de Software"
    ) == 0);

    assert(resultado.periodo == 8);

    
    strcpy(atualizado.matricula, "000000999");

    assert(
        tabela_hash_atualizar(
            &tabela,
            aluno.matricula,
            &atualizado
        ) == TABELA_MODIFICACAO_PARAMETRO_INVALIDO
    );

    
    assert(
        tabela_hash_buscar(
            &tabela,
            aluno.matricula,
            &resultado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    tabela_hash_liberar(&tabela);

    printf("APROVADO: atualizacao e preservacao da chave.\n");
}


static void teste_condicoes_limite(void) {

    printf("\n[TC07] Tabela vazia e condicoes-limite\n");

    TabelaHash tabela = {0};

    
    assert(!tabela_hash_inicializar(&tabela, 100));

    
    assert(tabela_hash_inicializar(&tabela, 2));

    Estudante aluno1 = criar_estudante(301);
    Estudante aluno2 = criar_estudante(302);
    Estudante resultado;

    
    assert(
        tabela_hash_buscar(
            &tabela,
            aluno1.matricula,
            &resultado
        ) == TABELA_BUSCA_NAO_ENCONTRADA
    );

    assert(
        tabela_hash_remover(
            &tabela,
            aluno1.matricula
        ) == TABELA_MODIFICACAO_NAO_ENCONTRADA
    );

    
    assert(
        tabela_hash_inserir(
            &tabela,
            &aluno1
        ) == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.capacidade == 2);

    
    assert(
        tabela_hash_inserir(
            &tabela,
            &aluno2
        ) == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.capacidade == 5);
    assert(tabela.quantidade == 2);

   
    assert(
        tabela_hash_buscar(
            &tabela,
            aluno1.matricula,
            &resultado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    assert(
        tabela_hash_buscar(
            &tabela,
            aluno2.matricula,
            &resultado
        ) == TABELA_BUSCA_ENCONTRADA
    );

    
    assert(
        tabela_hash_remover(
            &tabela,
            aluno1.matricula
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    assert(
        tabela_hash_remover(
            &tabela,
            aluno2.matricula
        ) == TABELA_MODIFICACAO_SUCESSO
    );

    assert(tabela.quantidade == 0);

    
    tabela_hash_listar(&tabela);

    tabela_hash_liberar(&tabela);

    assert(tabela.entradas == NULL);

    printf("APROVADO: tabela vazia e capacidade pequena.\n");
}


static void teste_redimensionamento(void) {

    printf("\n[TC08] Redimensionamento e preservacao\n");

    TabelaHash tabela = {0};

    assert(tabela_hash_inicializar(&tabela, 101));

    
    for (unsigned int i = 1; i <= 70; i++) {

        Estudante aluno = criar_estudante(i);

        assert(
            tabela_hash_inserir(
                &tabela,
                &aluno
            ) == TABELA_INSERCAO_SUCESSO
        );
    }

   
    assert(tabela.capacidade == 101);
    assert(tabela.quantidade == 70);

    printf("Capacidade antes do limite: %zu\n", tabela.capacidade);

    
    Estudante novo = criar_estudante(71);

    assert(
        tabela_hash_inserir(
            &tabela,
            &novo
        ) == TABELA_INSERCAO_SUCESSO
    );

    assert(tabela.capacidade == 211);
    assert(tabela.quantidade == 71);

    printf("Nova capacidade: %zu\n", tabela.capacidade);

    Estudante resultado;

    
    for (unsigned int i = 1; i <= 71; i++) {

        Estudante esperado = criar_estudante(i);

        assert(
            tabela_hash_buscar(
                &tabela,
                esperado.matricula,
                &resultado
            ) == TABELA_BUSCA_ENCONTRADA
        );

        assert(strcmp(
            resultado.nome,
            esperado.nome
        ) == 0);
    }

    assert(tabela.removidos == 0);

    tabela_hash_liberar(&tabela);

    printf("APROVADO: os 71 registros foram preservados.\n");
}


int main(void) {

    printf("========================================\n");
    printf("   SUITE DE TESTES - CONFIGURACAO 117\n");
    printf("========================================\n");

    teste_insercao_busca_duplicidade();

    teste_colisoes_e_remocao();

    teste_atualizacao();

    teste_condicoes_limite();

    teste_redimensionamento();

    printf("\n========================================\n");
    printf("TODOS OS TESTES FORAM APROVADOS!\n");
    printf("========================================\n");

    return 0;
}
