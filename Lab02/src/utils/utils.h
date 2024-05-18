#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>


#define CHUNK_SIZE 500
#define MAX_CHUNKS 10000
#define MAXLINE 10000
#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Envia todos os bytes do buffer
int send_all(int s, char *buf, int len);

// Recebe todos os bytes enviados por send_all
int recv_all(int sock_fd, char *buf);

// Quebra a string em tokens para obter o tamanho da mensagem e o retorna
int get_size(char * header);

// Remove o cabeçalho do servidor, escrevendo o corpo em body, retorna o tamanho da mensagem
int remove_header(char *message, char *body);

// Função auxiliar para substituir o tamanho no cabeçalho
void set_size(char *str, int size);

// Monta a mensagem
void build_message(char * message , int operation);

#endif /* UTILS_H */
