#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
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

// Função para cadastrar uma nova música, retorna 1 se o identificador já estiver sendo usado e 2 se o limite de músicas for atingido
int cadastrar_musica(char * body) {
    struct music my_music, music_list[MAX_SONGS];
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
    
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (my_music.identifier == music_list[i].identifier) {
            return 1;
        }
    }
    if (n < MAX_SONGS) {
        music_list[n] = my_music;
        write_music_list(music_list, n + 1, FILEPATH);
        return 0;
    }
    else {
        return 2;
    }
}

// Função para remover uma música a partir de seu identificador, retorna 1 se não existe música com esse identificador
int remover_musica(char * body) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    int identifier;

    // Gets identifier
    token = strtok(NULL, "\n");
    identifier = atoi(token);

    int found = 0, remove_i;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].identifier == identifier) {
            found = 1;
            remove_i = i;
            break;
        }
    }

    if (found == 1) {
        struct music new_list[MAX_SONGS];
        int j = 0;
        for (int i = 0; i < n; i++) {
            if (i != remove_i) {
                new_list[j] = music_list[i];
                j++;
            }
        }
        n = j;
        write_music_list(new_list, n, FILEPATH);
        return 0;
    }
    return 1;
}

// Função para listar todas as músicas lançadas em um determinado ano, retorna o número de músicas encontradas
int listar_musicas_por_ano(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char strMusic[3000];
    int year;

    // Gets year
    token = strtok(NULL, "\n");
    year = atoi(token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].release_year == year) {
            music_to_string_reduced(music_list[i], strMusic);
            strcat(output, strMusic);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Função para listar todas as músicas em um dado idioma lançadas em um certo ano, retorna o número de músicas encontradas
int listar_musicas_por_idioma_e_ano(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char language[LANGUAGE_LENGTH], strMusic[3000];
    int year;

    // Gets language
    token = strtok(NULL, "\n");
    strcpy(language, token);
    // Gets year
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    year = atoi(token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if ((music_list[i].release_year == year) && (strcmp(music_list[i].language, language)) == 0) {
            music_to_string_reduced(music_list[i], strMusic);
            strcat(output, strMusic);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Função para listar todas as músicas de um certo tipo, retorna o número de músicas encontradas
int listar_musicas_por_tipo(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char type[MUSIC_TYPE_LENGTH], strMusic[3000];

    // Gets type
    token = strtok(NULL, "\n");
    strcpy(type, token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (strcmp(music_list[i].music_type, type) == 0)  {
            music_to_string_reduced(music_list[i], strMusic);
            strcat(output, strMusic);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Função para listar todas as informações de uma música dado o seu identificador, retorna 1 se não existe música com esse identificador
int listar_info_musica_por_id(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "="), strMusic[3000];
    int identifier;

    // Gets identifier
    token = strtok(NULL, "\n");
    identifier = atoi(token);

    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].identifier == identifier) {
            music_to_string(music_list[i], strMusic);
            strcat(output, strMusic);
            strcat(output, "\n");
            return 0;
        }
    }
    return 1;
}

// Função para listar todas as informações de todas as músicas
int listar_todas_infos_musicas(char * output) {

    struct music music_list[MAX_SONGS];
    char strMusic[3000];
    int n = read_music_list(music_list,FILEPATH);
    
    for(int i = 0 ; i < n; i++){
        music_to_string(music_list[i], strMusic);
        strcat(output, strMusic);
        strcat(output, "\n");
    }

    return n;
}
