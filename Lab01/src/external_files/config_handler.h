#ifndef CONFIG_H
#define CONFIG_H

#define MAX_IP_NAME 32

typedef struct {
    char porta[MAX_IP_NAME];
    char ip[MAX_IP_NAME];
} Configuracao;

// Percorre o arquivo de configuração e cria um objeto com as informações relevantes
Configuracao ler_configuracao(const char *filename);

#endif /* CONFIG_H */
