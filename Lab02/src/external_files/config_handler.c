#include "config_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

// Percorre o arquivo de configuração e cria um objeto com as informações relevantes
configuration read_configuration(const char *filename) {
    configuration config;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro opening configuration file.\n");
        exit(EXIT_FAILURE);
    }
    char line[MAX_LINE_LENGTH];

    // Percorre as linhas do arquivo e preenche os respectivos campos
    while (fgets(line, sizeof(line), file) != NULL) {
        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");
        if (strcmp(key, "PORTA") == 0) {
            strcpy(config.port, value);
        } else if (strcmp(key, "IP") == 0) {
            strcpy(config.ip, value);
        }
    }
    fclose(file);
    return config;
}
