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
#define LIST_SONGS_BY_TYPE 1
#define LIST_ALL_SONGS_INFO 2
#define DOWNLOAD_SONG 3

#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Lista todas as músicas de um certo tipo
void list_songs_by_type(int sock_fd, char * type);

// Lista todas as informações de todas as músicas
void list_all_songs_info(int sock_fd);

// Baixa uma música dado um identificador
void download_song(int sockf_fd, int identifier, char * port);

#endif /* CLIENT_OPERATIONS_H */
