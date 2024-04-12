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

// Cadastra uma nova música
void register_song(int sock_fd, struct music new_song, int role);

// Remove uma música a partir de seu identificador
void remove_song(int sock_fd, int identifier, int role);

// Lista todas as músicas lançadas em um determinado ano
void list_songs_by_year(int sock_fd, int year, int role);

// Lista todas as músicas em um dado idioma lançadas em um certo ano
void list_songs_by_language_and_year(int sock_fd, char * language, int year, int role);

// Lista todas as músicas de um certo tipo
void list_songs_by_type(int sock_fd, char * type, int role);

// Lista todas as informações de uma música dado o seu identificador
void list_song_info_by_id(int sock_fd, int identifier, int role);

// Lista todas as informações de todas as músicas
void list_all_songs_info(int sock_fd, int role);

#endif /* CLIENT_OPERATIONS_H */
