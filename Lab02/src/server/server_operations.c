#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server_operations.h"
#include "../external_files/storage_handler.h"
#include "../utils/utils.h"

#define true 1

// Lista todas as músicas de um certo tipo, retorna o número de músicas encontradas
int list_songs_by_type(char * body, char * output) {
    struct music music_list[MAX_SONGS];
    char *token = strtok(body, "=");
    char type[MUSIC_TYPE_LENGTH];

    // Obtém tipo de música
    token = strtok(NULL, "\n");
    strcpy(type, token);

    int counter = 0;
    int n = read_music_list(music_list, FILEPATH);

    // Percorre a lista de músicas e adiciona no output aquelas com tipo correto
    for (int i = 0; i < n; i++) {
        if (strcmp(music_list[i].music_type, type) == 0)  {
            music_to_string_reduced(music_list[i], output);
            strcat(output, "\n");
            counter++;
        }
    }
    return counter;
}

// Lista todas as informações de todas as músicas, retorna o número de músicas
int list_all_songs_info(char * output) {
    struct music music_list[MAX_SONGS];
    int n = read_music_list(music_list,FILEPATH);
    
    // Percorre a lista de músicas e adiciona todas no output
    for(int i = 0 ; i < n; i++){
        music_to_string(music_list[i], output);
        strcat(output, "\n");
    }

    return n;
}
