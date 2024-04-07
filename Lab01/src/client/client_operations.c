#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../external_files/storage_handler.h"

#define MAXLINE 3000
#define MAX_INT_STR 12

// Envia todos os bytes do buffer
void send_all(int __fd, const void *__buf, int __flags){

    int bytes_sent;
    char cur_buf[MAXLINE];
    strcpy(cur_buf, __buf);
    while(strlen(cur_buf) > 0){
        bytes_sent = send(__fd, cur_buf, strlen(cur_buf), __flags);
        memmove(cur_buf, cur_buf + bytes_sent, strlen(cur_buf) - bytes_sent + 1);
    }
}

// Anexa cabeçalho da operação ao corpo da mensagem
void anexar_header_operacao(char * message , int operacao, int role){
    char strOut[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "", strMessage[MAX_BODY_SIZE] = "";

    // Copia a mensagem para um buffer temporário
    strcat(strMessage, message);
    strcat(strMessage,"\n");
    int msg_size = strlen(strMessage);

    // Cria o cabeçalho
    sprintf(strOut, "#HEADER\nSize=%d\nRole=%d\nOperation=%d\n#BODY\n", msg_size, role, operacao);
    strcat(strOut, strMessage);

    // Coloca a mensagem com o cabeçalho na variável original
    strcpy(message, strOut);
}

// Cadastra uma nova música
void cadastrar_musica(int sock_fd, struct music nova_musica, int role) {
    char strMusic[MAX_HEADER_SIZE + MAX_BODY_SIZE];
    music_to_string(nova_musica, strMusic);

    anexar_header_operacao(strMusic, CADASTRAR_UMA_MUSICA, role);
    send_all(sock_fd,strMusic,0);
}

// Remove uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    anexar_header_operacao(message, REMOVER_UMA_MUSICA, role);
    send_all(sock_fd, message, 0);
}

// Lista todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "ReleaseYear=";
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_ANO, role);
    send_all(sock_fd, message, 0);
}

// Lista todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, char * idioma, int year, int role) {
    char strYear[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Language=";
    strcat(message, idioma);
    strcat(message, "ReleaseYear=");
    sprintf(strYear, "%d\n", year);
    strcat(message, strYear);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_IDIOMA_E_ANO, 0);
    send_all(sock_fd, message, role);
}

// Lista todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd,  char *tipo, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "MusicType=";
    strcat(message, tipo);

    anexar_header_operacao(message, LISTAR_MUSICAS_POR_TIPO, 0);
    send_all(sock_fd, message, role);
}

// Lista todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd, int identifier, int role) {
    char strId[MAX_INT_STR], message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "Identifier=";
    sprintf(strId, "%d\n", identifier);
    strcat(message, strId);

    anexar_header_operacao(message, LISTAR_INFO_MUSICA_POR_ID, 0);
    send_all(sock_fd, message, role);
}

// Lista todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd, int role) {
    char message[MAX_HEADER_SIZE + MAX_BODY_SIZE] = "";
    
    anexar_header_operacao(message, LISTAR_TODAS_INFOS_MUSICAS, 0);
    send_all(sock_fd, message, role);
}
