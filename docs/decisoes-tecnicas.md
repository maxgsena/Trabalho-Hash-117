# Decisoes Tecnicas

## 1. Funcao Hash H3 - Meio do Quadrado

A funcao H3 foi implementada utilizando o metodo
do meio do quadrado.

### Conversao da chave

A matricula e armazenada como texto, mas deve
conter exclusivamente caracteres numericos.

Para o calculo do hash, convertemos a matricula
para um inteiro sem sinal de 64 bits.

Nesta primeira versao, aceitamos matriculas
de 1 a 9 digitos.

Essa restricao garante que o quadrado da chave
possa ser armazenado em uint64_t sem overflow.

### Calculo do indice

1. Converter a matricula para inteiro.
2. Elevar a chave ao quadrado.
3. Determinar a quantidade de digitos centrais,
   conforme a quantidade de digitos de (m - 1).
4. Extrair os digitos centrais.
5. Aplicar o modulo pela capacidade da tabela.

### Exemplo

Matricula: 12345

Quadrado: 152399025

Capacidade: 101

Digitos centrais: 399

Indice: 399 % 101 = 96

### Tratamento de entradas invalidas

A funcao rejeita:
- Matriculas vazias;
- Matriculas com letras ou simbolos;
- Matriculas com mais de 9 digitos;
- Capacidades menores que 2.

### Implementacao

Arquivos:
- src/hash.h
- src/hash.c
- tests/test_hash.c
