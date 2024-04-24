#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CONFIG_LEN 32

typedef struct {
    char porta[MAX_CONFIG_LEN];
    char ip[MAX_CONFIG_LEN];
} configuracao;

// Percorre o arquivo de configuração e cria um objeto com as informações relevantes
configuracao ler_configuracao(const char *filename);

#endif /* CONFIG_H */
