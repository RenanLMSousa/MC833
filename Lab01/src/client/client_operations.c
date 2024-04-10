#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define MAX_INT_STR 12
#define true 1

// Cadastra uma nova música
void cadastrar_musica(int sock_fd, struct music nova_musica, int role) {
    char strMusic[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    music_to_string(nova_musica, strMusic);

    anexar_header_operacao(strMusic, CADASTRAR_UMA_MUSICA, role);
    if (send_all(sock_fd, strMusic, strlen(strMusic)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Remove uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    anexar_header_operacao(message, REMOVER_UMA_MUSICA, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "ReleaseYear=";
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_ANO, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, char * idioma, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Language=";
    strcat(message, idioma);
    strcat(message, "ReleaseYear=");
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_IDIOMA_E_ANO, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd,  char *tipo, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "MusicType=";
    strcat(message, tipo);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_TIPO, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    anexar_header_operacao(message, LISTAR_INFO_MUSICA_POR_ID, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}

// Lista todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    
    anexar_header_operacao(message, LISTAR_TODAS_INFOS_MUSICAS, role);
    if (send_all(sock_fd, message, strlen(message)) < 0) {
        perror("str_echo: send error");
        return;
    }
}
