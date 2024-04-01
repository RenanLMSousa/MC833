#include <stdio.h>
#include <string.h>
#include "client_operations.h"
#include "../external_files/storage_handler.h"


#define MAXLINE 1000

void send_all(int __fd, const void *__buf, int __flags){

    int bytes_sent;
    char cur_buf[MAXLINE];
    strcpy(cur_buf,__buf);

    while(strlen(cur_buf) > 0){
        bytes_sent = send(__fd,cur_buf,strlen(cur_buf),__flags);
        memmove(cur_buf, cur_buf + bytes_sent, strlen(cur_buf) - bytes_sent + 1);
    }

}

void anexar_header_operacao(char * message , int operacao){


    strcat(operacao,"\n");
    strcat(operacao,message);

    return 1;
}

// Função para cadastrar uma nova música
void cadastrar_musica(int sock_fd,struct music nova_musica) {
    // Falta mandar o arquivo da música em sí
    char  strMusic[1000];
    
    music_to_string(nova_musica,strMusic);
    anexar_header_operacao(strMusic,CADASTRAR_UMA_MUSICA);
    send_all(sock_fd,strMusic,0);

}

// Função para remover uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier) {

}

// Função para listar todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd,int year) {
    // ID, titulo e interprete
}

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, const char *idioma, int year) {
    // ID, titulo e interprete
}

// Função para listar todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd,const char *tipo) {
    // ID, titulo e interprete
}

// Função para listar todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd,int identifier) {

}

// Função para listar todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd) {

}
