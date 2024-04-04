#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
#include "../external_files/storage_handler.h"

#define MAXLINE 3000

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

// Cadastra uma nova música, retorna 1 se não foi possivel cadastrar
int cadastrar_musica(char * body) {
    struct music my_music, music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    // Obtém identificador
    token = strtok(NULL, "\n");
    my_music.identifier = atoi(token);
    // Obtém título
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.title, token);
    // Obtém intérprete
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.performer, token);
    // Obtém linguagem
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.language, token);
    // Obtém tipo de música
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.music_type, token);
    // Obtém refrão
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    strcpy(my_music.chorus, token);
    // Obtém ano de lançamento de lançamento
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    my_music.release_year = atoi(token);
    
    int n = read_music_list(music_list, FILEPATH);
    // Verifica se o identificador já está em uso
    for (int i = 0; i < n; i++) {
        if (my_music.identifier == music_list[i].identifier) {
            return 1;
        }
    }
    // Verifica se o limite máximo de músicas não foi alcançado
    if (n < MAX_SONGS) {
        music_list[n] = my_music;
        write_music_list(music_list, n + 1, FILEPATH);
        return 0;
    }
    else {
        return 2;
    }
}

// Remove uma música a partir de seu identificador, retorna 1 se não existe música com esse identificador
int remover_musica(char * body) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    int identifier;

    // Obtém identificador
    token = strtok(NULL, "\n");
    identifier = atoi(token);

    // Identifica índice da música que vai ser removida
    int found = 0, remove_i;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].identifier == identifier) {
            found = 1;
            remove_i = i;
            break;
        }
    }

    // Verifica se o identificador foi encontrado, se sim remove a musica da lista
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
        // Sobrescreve no arquivo
        write_music_list(new_list, n, FILEPATH);
        return 0;
    }
    return 1;
}

// Lista todas as músicas lançadas em um determinado ano, retorna o número de músicas encontradas
int listar_musicas_por_ano(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    int year;

    // Obtém ano de lançamento
    token = strtok(NULL, "\n");
    year = atoi(token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].release_year == year) {
            music_to_string_reduced(music_list[i], output);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Lista todas as músicas em um dado idioma lançadas em um certo ano, retorna o número de músicas encontradas
int listar_musicas_por_idioma_e_ano(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char language[LANGUAGE_LENGTH];
    int year;

    // Obtém linguagem
    token = strtok(NULL, "\n");
    strcpy(language, token);
    // Obtém ano de lançamento
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    year = atoi(token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if ((music_list[i].release_year == year) && (strcmp(music_list[i].language, language)) == 0) {
            music_to_string_reduced(music_list[i], output);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Lista todas as músicas de um certo tipo, retorna o número de músicas encontradas
int listar_musicas_por_tipo(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char type[MUSIC_TYPE_LENGTH];

    // Obtém tipo de música
    token = strtok(NULL, "\n");
    strcpy(type, token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (strcmp(music_list[i].music_type, type) == 0)  {
            music_to_string_reduced(music_list[i], output);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Lista todas as informações de uma música dado o seu identificador, retorna 1 se não existe música com esse identificador
int listar_info_musica_por_id(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    int identifier;

    // Obtém identificador
    token = strtok(NULL, "\n");
    identifier = atoi(token);

    int n = read_music_list(music_list, FILEPATH);
    for (int i = 0; i < n; i++) {
        if (music_list[i].identifier == identifier) {
            music_to_string(music_list[i], output);
            strcat(output, "\n");
            // Como o identificador é único, retornamos
            return 0;
        }
    }
    return 1;
}

// Lista todas as informações de todas as músicas, retorna o número de músicas
int listar_todas_infos_musicas(char * output) {
    struct music music_list[MAX_SONGS];
    int n = read_music_list(music_list,FILEPATH);
    
    for(int i = 0 ; i < n; i++){
        music_to_string(music_list[i], output);
        strcat(output, "\n");
    }

    return n;
}
