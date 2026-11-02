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
