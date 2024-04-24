#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "storage_handler.h"

#define MAX_LINE_LENGTH 512 // Tamanho máximo de cada linha do arquivo

// Lê as músicas do arquivo e as armazena em uma lista, retorna o número de músicas lidas
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
        // Quebra a linha em campos separados por |
        char *token = strtok(line, "|");
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
            token = strtok(NULL, "|");
            i++;
        }
        num_songs++;
    }

    fclose(file);

    return num_songs;
}

// Escreve as musicas de uma lista no arquivo
void write_music_list(struct music music_list[MAX_SONGS], int n, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        return;
    }

    // Escreve cada música na lista no arquivo CSV
    for (int i = 0; i < n; i++) {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%d\n",
                music_list[i].identifier,
                music_list[i].title,
                music_list[i].performer,
                music_list[i].language,
                music_list[i].music_type,
                music_list[i].chorus,
                music_list[i].release_year);
    }

    fclose(file);
}