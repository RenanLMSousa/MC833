#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CONFIG_LEN 32

typedef struct {
    char port[MAX_CONFIG_LEN];
    char ip[MAX_CONFIG_LEN];
} configuration;

// Percorre o arquivo de configuração e cria um objeto com as informações relevantes
configuration read_configuration(const char *filename);

#endif /* CONFIG_H */
