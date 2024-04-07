#ifndef CLIENT_OPERATIONS_H
#define CLIENT_OPERATIONS_H

#include "../music/music.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// Definições para operações
#define CADASTRAR_UMA_MUSICA 1
#define REMOVER_UMA_MUSICA 2
#define LISTAR_MUSICAS_POR_ANO 3
#define LISTAR_MUSICAS_POR_IDIOMA_E_ANO 4
#define LISTAR_MUSICAS_POR_TIPO 5
#define LISTAR_INFO_MUSICA_POR_ID 6
#define LISTAR_TODAS_INFOS_MUSICAS 7

#define MAX_HEADER_SIZE 100
#define MAX_BODY_SIZE 3000

// Envia todos os bytes do buffer
void send_all(int __fd, const void *__buf, int __flags);

// Cadastra uma nova música
void cadastrar_musica(int sock_fd, struct music nova_musica, int role);

// Remove uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier, int role);

// Lista todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd, int year, int role);

// Lista todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, char *idioma, int year, int role);

// Lista todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd, char *tipo, int role);

// Lista todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd, int identifier, int role);

// Lista todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd, int role);

#endif /* CLIENT_OPERATIONS_H */
