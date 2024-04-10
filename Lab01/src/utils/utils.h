#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

#define MAXLINE 10000 // Tamanho máximo da linha
#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Envia todos os bytes do buffer
int send_all(int s, char *buf, int len);

// Recebe todos os bytes enviados por send_all
int recv_all(int sock_fd, char *buf);

// Verifica se o header representa o servidor, se sim retorna o tamanho, se não retorna -1
int verificar_header(char *header);

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna -1 se houver erro e o tamanho do body caso contrário
int remove_cabecalho(char *message, char *body);

// Função auxiliar para substituir o tamanho no cabeçalho
void troca_size(char *str, int size);

// Anexa cabeçalho da operação ao corpo da mensagem
void anexar_header_operacao(char *message, int operacao, int role);

#endif /* UTILS_H */
