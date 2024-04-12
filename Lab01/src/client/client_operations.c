#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define MAX_INT_STR 12
#define true 1

// Cadastra uma nova música
void register_song(int sock_fd, struct music new_song, int role) {
    char strMusic[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    music_to_string(new_song, strMusic);

    // Forma a string da mensagem e envia para o servidor
    build_message(strMusic, REGISTER_SONG, role);
    if (send_all(sock_fd, strMusic, strlen(strMusic)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Remove uma música a partir de seu identificador
void remove_song(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, REMOVE_SONG, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas lançadas em um determinado ano
void list_songs_by_year(int sock_fd, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "ReleaseYear=";
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_SONGS_BY_YEAR, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas em um dado idioma lançadas em um certo ano
void list_songs_by_language_and_year(int sock_fd, char * language, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Language=";
    strcat(message, language);
    strcat(message, "ReleaseYear=");
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_SONGS_BY_LANGUAGE_AND_YEAR, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas de um certo tipo
void list_songs_by_type(int sock_fd,  char * type, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "MusicType=";
    strcat(message, type);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_SONGS_BY_TYPE, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as informações de uma música dado o seu identificador
void list_song_info_by_id(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_SONG_INFO_BY_ID, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as informações de todas as músicas
void list_all_songs_info(int sock_fd, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    
    // Forma a string da mensagem e envia para o servidor
    build_message(message, LIST_ALL_SONGS_INFO, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}
