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

// Definições para operações
#define REGISTER_SONG 1
#define REMOVE_SONG 2
#define LIST_SONGS_BY_YEAR 3
#define LIST_SONGS_BY_LANGUAGE_AND_YEAR 4
#define LIST_SONGS_BY_TYPE 5
#define LIST_SONG_INFO_BY_ID 6
#define LIST_ALL_SONGS_INFO 7

#define MAX_HEADER_SIZE 1000
#define MAX_BODY_SIZE 30000

// Envia todos os bytes do buffer
int send_all(int s, char * buf, int len);

// Recebe todos os bytes enviados por send_all
int recv_all(int sock_fd, char * buf);

// Cadastra uma nova música, retorna 1 se não foi possivel cadastrar
int register_song(char * body);

// Remove uma música a partir de seu identificador, retorna 1 se não existe música com esse identificador
int remove_song(char * body);

// Lista todas as músicas lançadas em um determinado ano, retorna o número de músicas encontradas
int list_songs_by_year(char * body, char * output);

// Lista todas as músicas em um dado idioma lançadas em um certo ano, retorna o número de músicas encontradas
int list_songs_by_language_and_year(char * body, char * output);

// Lista todas as músicas de um certo tipo, retorna o número de músicas encontradas
int list_songs_by_type(char * body, char * output);

// Lista todas as informações de uma música dado o seu identificador, retorna 1 se não existe música com esse identificador
int list_song_info_by_id(char * body, char * output);

// Lista todas as informações de todas as músicas, retorna o número de músicas
int list_all_songs_info(char * output);

#endif /* SERVER_OPERATIONS_H */
