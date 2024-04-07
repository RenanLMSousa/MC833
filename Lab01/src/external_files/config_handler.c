#include "config_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

// Percorre o arquivo de configuração e cria um objeto com as informações relevantes
configuracao ler_configuracao(const char *filename) {
    configuracao config;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo de configuração.\n");
        exit(EXIT_FAILURE);
    }
    char linha[MAX_LINE_LENGTH];
    while (fgets(linha, sizeof(linha), file) != NULL) {
        char *chave = strtok(linha, "=");
        char *valor = strtok(NULL, "=");
        if (strcmp(chave, "PORTA") == 0) {
            strcpy(config.porta, valor);
        } else if (strcmp(chave, "IP") == 0) {
            strcpy(config.ip, valor);
        }
    }
    fclose(file);
    return config;
}
