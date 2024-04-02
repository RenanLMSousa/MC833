#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_handler.h"

#define MAX_LINE_LENGTH 256

// Função para ler as músicas do arquivo e armazená-las em uma lista
int read_music_list(struct music music_list[MAX_SONGS], const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo.\n");
        return 0;
    }

    int num_songs = 0;

    // Lê cada linha do arquivo
    char line[MAX_LINE_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        // Quebra a linha em campos separados por vírgula
        char *token = strtok(line, ",");
        int i = 0;
        while (token != NULL) {
            switch (i) {
                case 0:
                    music_list[num_songs].identifier = atoi(token);
                    break;
                case 1:
                    strncpy(music_list[num_songs].title, token, TITLE_LENGTH);
                    break;
                case 2:
                    strncpy(music_list[num_songs].performer, token, PERFORMER_LENGTH);
                    break;
                case 3:
                    strncpy(music_list[num_songs].language, token, LANGUAGE_LENGTH);
                    break;
                case 4:
                    strncpy(music_list[num_songs].music_type, token, MUSIC_TYPE_LENGTH);
                    break;
                case 5:
                    strncpy(music_list[num_songs].chorus, token, CHORUS_LENGTH);
                    break;
                case 6:
                    music_list[num_songs].release_year = atoi(token);
                    break;
                default:
                    break;
            }
            token = strtok(NULL, ",");
            i++;
        }
        num_songs++;
    }

    fclose(file);

    return num_songs;
}
