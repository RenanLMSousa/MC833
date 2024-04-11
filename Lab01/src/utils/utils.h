#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>

#define MAXLINE 10000
#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Envia todos os bytes do buffer
int send_all(int s, char *buf, int len);

// Recebe todos os bytes enviados por send_all
int recv_all(int sock_fd, char *buf);

// Quebra a string em tokens para obter o tamanho da mensagem e o retorna
int obtem_tamanho(char * header);

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna o tamanho da mensagem
int remove_cabecalho(char *message, char *body);

// Função auxiliar para substituir o tamanho no cabeçalho
void troca_size(char *str, int size);

// Anexa cabeçalho da operação ao corpo da mensagem
void anexar_header_operacao(char *message, int operacao, int role);

#endif /* UTILS_H */
