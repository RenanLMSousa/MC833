#ifndef SERVER_OPERATIONS_H
#define SERVER_OPERATIONS_H

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
#include "../external_files/config_handler.h"

// Definições para operações
#define LIST_SONGS_BY_TYPE 1
#define LIST_ALL_SONGS_INFO 2
#define DOWNLOAD_SONG 3

#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Lista todas as músicas de um certo tipo, retorna o número de músicas encontradas
int list_songs_by_type(char * body, char * output);

// Lista todas as informações de todas as músicas, retorna o número de músicas
int list_all_songs_info(char * output);

// Baixa uma música dado um identificador, retorna 1 se não houver arquivo desse identificador
int download_song(char * body, configuration serverConfig);

#endif /* SERVER_OPERATIONS_H */
