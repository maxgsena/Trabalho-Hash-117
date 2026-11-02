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


## 2. Tratamento de Colisoes C4 - Hash Duplo

O tratamento de colisoes utiliza a tecnica de hash duplo.

### Funcao principal

A primeira funcao hash e a H3, implementada
pelo metodo do meio do quadrado.

### Funcao auxiliar

Foi adotada a seguinte funcao:

h2(k) = 1 + (k mod (m - 1))

Essa funcao determina o passo utilizado
nas sondagens.

### Calculo das posicoes

A posicao visitada em cada tentativa e calculada por:

p_i(k) = (h1(k) + i * h2(k)) mod m

### Capacidade da tabela

Foram escolhidas capacidades primas.

Essa decisao garante que o passo gerado pela funcao
auxiliar seja coprimo com a capacidade da tabela,
permitindo percorrer todas as posicoes.

Capacidades compostas sao rejeitadas pela implementacao.

### Teste de colisao

Foram utilizadas duas matriculas:

000000001
000000032

Ambas produzem o indice inicial 1 pela H3.

Entretanto, suas funcoes auxiliares geram passos diferentes:

Matricula 000000001: h2 = 2
Matricula 000000032: h2 = 33

Com capacidade 101, a segunda matricula visita:

1, 34, 67, 100...

O teste confirmou que as 101 posicoes podem ser
percorridas sem repeticao antes de completar o ciclo.

### Arquivos

- src/hash.h
- src/hash.c
- tests/test_hash_duplo.c


## 3. Insercao e Busca

A insercao e a busca utilizam as funcoes H3 e C4.

### Inicializacao

A tabela e inicializada por meio de alocacao
dinamica utilizando calloc.

A capacidade inicial deve ser um numero primo.

### Insercao

Para inserir um estudante:

1. Validar a matricula;
2. Calcular o indice inicial utilizando H3;
3. Calcular o passo utilizando H2;
4. Verificar se a posicao esta ocupada;
5. Em caso de colisao, avancar utilizando
   o hash duplo;
6. Verificar se a matricula ja existe;
7. Armazenar o estudante na primeira posicao
   disponivel, respeitando os marcadores de remocao.

Matriculas duplicadas sao rejeitadas.

### Busca

A busca utiliza a mesma sequencia de sondagem
da insercao.

A operacao termina quando:
- A matricula e encontrada;
- Uma posicao vazia e encontrada;
- Todas as posicoes sao verificadas.

### Contabilizacao

Colisoes:
Contabilizamos uma colisao quando a posicao
inicial de uma nova matricula esta ocupada
por outra matricula.

Sondagens:
Contabilizamos cada posicao examinada durante
as operacoes de insercao e busca.

### Limitacao atual

Nesta etapa, o crescimento automatico da tabela
ainda nao foi implementado.

Essa funcionalidade sera adicionada junto ao
rehashing.

### Arquivos

- src/tabela_hash.h
- src/tabela_hash.c
- tests/test_tabela_hash.c
