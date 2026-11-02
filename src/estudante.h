#ifndef ESTUDANTE_H
#define ESTUDANTE_H

#define TAM_MATRICULA 20
#define TAM_NOME 100
#define TAM_CURSO 100

typedef struct {
    char matricula[TAM_MATRICULA];
    char nome[TAM_NOME];
    char curso[TAM_CURSO];
    unsigned int periodo;
} Estudante;

#endif
