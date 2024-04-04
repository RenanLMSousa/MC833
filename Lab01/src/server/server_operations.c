#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
#include "../external_files/storage_handler.h"

#define MAXLINE 3000

// Função para cadastrar uma nova música
void cadastrar_musica(char * body) {
    struct music my_music;
    char *token = strtok(body, "=");
    // Gets identifier
    token = strtok(NULL, "\n");
    my_music.identifier = atoi(token);
    // Gets title
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.title, token);
    // Gets performer
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.performer, token);
    // Gets language
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.language, token);
    // Gets music type
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.music_type, token);
    // Gets chorus
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.chorus, token);
    // Gets release year
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    my_music.release_year = atoi(token);
    print_music_details(my_music);
}

// Função para remover uma música a partir de seu identificador
void remover_musica(char * body) {
    char *token = strtok(body, "=");
    int identifier;

    // Gets identifier
    token = strtok(NULL, "\n");
    identifier = atoi(token);
}

// Função para listar todas as músicas lançadas em um determinado ano
void listar_musicas_por_ano(char * body) {
    char *token = strtok(body, "=");
    int year;

    // Gets year
    token = strtok(NULL, "\n");
    year = atoi(token);
}

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano
void listar_musicas_por_idioma_e_ano(char * body) {
    char *token = strtok(body, "=");
    char language[LANGUAGE_LENGTH];
    int year;

    // Gets language
    token = strtok(NULL, "\n");
    strcpy(language, token);
    // Gets year
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    year = atoi(token);
}

// Função para listar todas as músicas de um certo tipo
void listar_musicas_por_tipo(char * body) {
    char *token = strtok(body, "=");
    char type[MUSIC_TYPE_LENGTH];

    // Gets type
    token = strtok(NULL, "\n");
    strcpy(type, token);
}

// Função para listar todas as informações de uma música dado o seu identificador
void listar_info_musica_por_id(char * body) {
    char *token = strtok(body, "=");
    int identifier;

    // Gets identifier
    token = strtok(NULL, "\n");
    identifier = atoi(token);
}

// Função para listar todas as informações de todas as músicas
void listar_todas_infos_musicas(char * body) {
}
