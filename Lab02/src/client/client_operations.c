#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../music/music.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define MAX_INT_STR 12
#define true 1

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

// Baixa uma música dado um identificador
void download_song(int sockf_fd, int identifier, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    append_int(message, "Identifier=", identifier);
    // Forma a string da mensagem e envia para o servidor
    build_message(message, DOWNLOAD_SONG, role);
    if (send_all(sockf_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}