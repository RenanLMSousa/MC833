#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
#include "../external_files/storage_handler.h"

#define MAXLINE 3000

// Função para cadastrar uma nova música
void cadastrar_musica(int sock_fd, char * body) {
    char *token = strtok(body, "=");
}

// Função para remover uma música a partir de seu identificador
void remover_musica(int sock_fd, int identifier) {
}

// Função para listar todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(int sock_fd, int year) {
}

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(int sock_fd, char * idioma, int year) {
}

// Função para listar todas as músicas de um certo tipo
void listar_musicas_por_tipo(int sock_fd,  char *tipo) {
}

// Função para listar todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(int sock_fd, int identifier) {
}

// Função para listar todas as informações de todas as músicas
void listar_todas_infos_musicas(int sock_fd) {
}
