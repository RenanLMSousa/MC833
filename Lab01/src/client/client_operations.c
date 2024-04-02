#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "client_operations.h"
#include "../external_files/storage_handler.h"

#define MAXLINE 3000

void send_all(int __fd, const void *__buf, int __flags){

    int bytes_sent;
    char cur_buf[MAXLINE];
    strcpy(cur_buf, __buf);
    while(strlen(cur_buf) > 0){
        bytes_sent = send(__fd, cur_buf, strlen(cur_buf), __flags);
        memmove(cur_buf, cur_buf + bytes_sent, strlen(cur_buf) - bytes_sent + 1);
    }
    
}

void anexar_header_operacao(char * message , int operacao){
    char strOp[3000];
    char strMessage[3000] = "";
    sprintf(strOp, "%d", operacao);
    strcat(strOp,"\n");
    strcat(strOp,message);
    strcat(strOp,"\n");

    int msg_size = strlen(strOp);

    char str_msg_size[100];
    sprintf(str_msg_size, "%d", msg_size);

    strcat(strMessage,"#HEADER\n");
    strcat(strMessage,"Size=");
    strcat(strMessage,str_msg_size);
    strcat(strMessage,"\n");
    strcat(strMessage,"#BODY\n");
    strcat(strMessage,strOp);
    strcpy(message, strMessage);
}

// Função para cadastrar uma nova música
void cadastrar_musica(int sock_fd,struct music nova_musica) {
    char strMusic[2000];
    music_to_string(nova_musica,strMusic);

    anexar_header_operacao(strMusic, CADASTRAR_UMA_MUSICA);
    send_all(sock_fd,strMusic,0);
}

// Função para remover uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier) {
    char strId[3000];
    sprintf(strId, "%d", identifier);

    anexar_header_operacao(strId, REMOVER_UMA_MUSICA);

    send_all(sock_fd, strId, 0);
}

// Função para listar todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd, int year) {
    char strYear[3000];
    sprintf(strYear, "%d", year);

    anexar_header_operacao(strYear, LISTAR_MUSICAS_POR_ANO);
    send_all(sock_fd, strYear, 0);
}

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, char * idioma, int year) {
    char strYear[3000];
    sprintf(strYear, "%d", year);
    strcat(idioma, strYear);

    anexar_header_operacao(idioma, LISTAR_MUSICAS_POR_IDIOMA_E_ANO);
    send_all(sock_fd, idioma, 0);
}

// Função para listar todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd,  char *tipo) {
    anexar_header_operacao(tipo, LISTAR_MUSICAS_POR_TIPO);
    send_all(sock_fd, tipo, 0);
}

// Função para listar todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd, int identifier) {
    char strId[3000];
    sprintf(strId, "%d", identifier);

    anexar_header_operacao(strId, LISTAR_INFO_MUSICA_POR_ID);
    send_all(sock_fd, strId, 0);
}

// Função para listar todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd) {
    char strOp[3000];
    sprintf(strOp, "%d", LISTAR_TODAS_INFOS_MUSICAS);
    strcat(strOp,"\n");
    send_all(sock_fd, strOp, 0);
}
