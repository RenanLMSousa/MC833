#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include "../music/music.h"

#define MAX_SONGS 100
#define FILEPATH "../../storage/music_list.csv"

// Lê as músicas do arquivo e as armazena em uma lista
int read_music_list(struct music music_list[MAX_SONGS], const char *filename);
// Escreve as musicas de uma lista no arquivo
void write_music_list(struct music music_list[MAX_SONGS], int n, const char *filename);
#endif /* STORAGE_HANDLER_H */
