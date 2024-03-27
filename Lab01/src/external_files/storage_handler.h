#ifndef STORAGE_HANDLER_H
#define STORAGE_HANDLER_H

#include "../music/music.h"

#define MAX_SONGS 100

// Função para ler as músicas do arquivo e armazená-las em uma lista
int read_music_list(struct music music_list[MAX_SONGS], const char *filename);
#endif /* STORAGE_HANDLER_H */
