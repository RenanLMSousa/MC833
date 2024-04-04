#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include "../music/music.h"

#define MAX_SONGS 100
#define FILEPATH "../../storage/music_list.csv"

// Função para ler as músicas do arquivo e armazená-las em uma lista
int read_music_list(struct music music_list[MAX_SONGS], const char *filename);
void write_music_list(struct music music_list[MAX_SONGS], int n, const char *filename);
#endif /* STORAGE_HANDLER_H */
