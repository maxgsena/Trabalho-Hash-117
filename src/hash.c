#include "hash.h"
#include <string.h>


bool converter_matricula(const char *matricula, uint64_t *chave) {

    if (matricula == NULL || chave == NULL) {
        return false;
    }

    size_t tamanho = strlen(matricula);

    if (tamanho == 0 || tamanho > MAX_DIGITOS_MATRICULA_HASH) {
        return false;
    }

    uint64_t valor = 0;

    for (size_t i = 0; i < tamanho; i++) {

        if (matricula[i] < '0' || matricula[i] > '9') {
            return false;
        }

        valor = valor * 10 + (uint64_t)(matricula[i] - '0');
    }

    *chave = valor;

    return true;
}


static size_t contar_digitos(size_t numero) {

    size_t quantidade = 1;

    while (numero >= 10) {
        numero /= 10;
        quantidade++;
    }

    return quantidade;
}


static size_t numero_para_texto(
    uint64_t numero,
    char *texto,
    size_t capacidade_buffer
) {
    char invertido[20];
    size_t quantidade = 0;

    do {
        if (quantidade >= sizeof(invertido)) {
            return 0;
        }

        invertido[quantidade++] = (char)('0' + numero % 10);

        numero /= 10;

    } while (numero > 0);

    if (quantidade + 1 > capacidade_buffer) {
        return 0;
    }

    for (size_t i = 0; i < quantidade; i++) {
        texto[i] = invertido[quantidade - 1 - i];
    }

    texto[quantidade] = '\0';

    return quantidade;
}

bool hash_meio_do_quadrado(
    const char *matricula,
    size_t capacidade,
    size_t *indice
) {

    if (indice == NULL || capacidade < 2) {
        return false;
    }

    uint64_t chave;

    if (!converter_matricula(matricula, &chave)) {
        return false;
    }

    
    uint64_t quadrado = chave * chave;

    

    char texto_quadrado[32];

    size_t comprimento = numero_para_texto(
        quadrado,
        texto_quadrado,
        sizeof(texto_quadrado)
    );

    if (comprimento == 0) {
        return false;
    }

   
    size_t digitos_centrais = contar_digitos(capacidade - 1);


   
    size_t comprimento_ajustado =
        comprimento > digitos_centrais
        ? comprimento
        : digitos_centrais;

    char numero_ajustado[32];

    size_t zeros = comprimento_ajustado - comprimento;

    if (comprimento_ajustado >= sizeof(numero_ajustado)) {
        return false;
    }

    memset(numero_ajustado, '0', zeros);

    memcpy(
        numero_ajustado + zeros,
        texto_quadrado,
        comprimento
    );

    numero_ajustado[comprimento_ajustado] = '\0';

    
    size_t inicio = (comprimento_ajustado - digitos_centrais) / 2;

    
    size_t valor_central = 0;

    for (size_t i = 0; i < digitos_centrais; i++) {

        size_t digito = (size_t)(numero_ajustado[inicio + i] - '0');

        valor_central = (valor_central * 10 + digito) % capacidade;
    }

    *indice = valor_central;

    return true;
}



static bool capacidade_prima(size_t numero) {

    if (numero < 2) {
        return false;
    }

    if (numero == 2) {
        return true;
    }

    if (numero % 2 == 0) {
        return false;
    }

    for (size_t divisor = 3; divisor <= numero / divisor; divisor += 2) {

        if (numero % divisor == 0) {
            return false;
        }
    }

    return true;
}

bool hash_capacidade_valida(size_t capacidade) {
    return capacidade_prima(capacidade);
}

static size_t somar_modulo(
    size_t a,
    size_t b,
    size_t modulo
) {

    a %= modulo;
    b %= modulo;

    if (a >= modulo - b) {
        return a - (modulo - b);
    }

    return a + b;
}


static size_t multiplicar_modulo(
    size_t a,
    size_t b,
    size_t modulo
) {

    size_t resultado = 0;

    a %= modulo;

    while (b > 0) {

        if (b % 2 != 0) {
            resultado = somar_modulo(resultado, a, modulo);
        }

        b /= 2;

        if (b > 0) {
            a = somar_modulo(a, a, modulo);
        }
    }

    return resultado;
}


bool hash_duplo_passo(
    const char *matricula,
    size_t capacidade,
    size_t *passo
) {

    if (passo == NULL || !capacidade_prima(capacidade)) {
        return false;
    }

    uint64_t chave;

    if (!converter_matricula(matricula, &chave)) {
        return false;
    }

    *passo = 1 + (size_t)(
        chave % (uint64_t)(capacidade - 1)
    );

    return true;
}


bool hash_duplo_posicao(
    const char *matricula,
    size_t capacidade,
    size_t tentativa,
    size_t *indice
) {

    if (indice == NULL || !capacidade_prima(capacidade)) {
        return false;
    }

    size_t hash1;
    size_t hash2;

    
    if (!hash_meio_do_quadrado(
        matricula,
        capacidade,
        &hash1
    )) {
        return false;
    }

    
    if (!hash_duplo_passo(
        matricula,
        capacidade,
        &hash2
    )) {
        return false;
    }

    
    size_t deslocamento = multiplicar_modulo(
        tentativa,
        hash2,
        capacidade
    );

    
    *indice = somar_modulo(
        hash1,
        deslocamento,
        capacidade
    );

    return true;
}