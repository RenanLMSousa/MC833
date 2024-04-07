#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "music.h"

// Imprime os campos da estrutura music
void print_music_details(struct music my_music) {
    printf("Identifier: %d\n", my_music.identifier);
    printf("Title: %s\n", my_music.title);
    printf("Performer: %s\n", my_music.performer);
    printf("Language: %s\n", my_music.language);
    printf("Music Type: %s\n", my_music.music_type);
    printf("Chorus: %s\n", my_music.chorus);
    printf("Release Year: %d\n", my_music.release_year);
}

// Adiciona no fim da string buffer strings de chave e valor
void append_string(char * buffer, char * key, char * value) {
    strcat(buffer, key);
    strcat(buffer, value);
    strcat(buffer, "\n");
}

// Adiciona no fim da string buffer uma chave e um inteiro
void append_int(char * buffer, char * key, int value) {
    char temp_buffer[200]; // Buffer temporário para converter int para string
    sprintf(temp_buffer, "%d", value);
    append_string(buffer, key, temp_buffer);
}

// Adiciona ao buffer os campos identifier, title e performer de uma música
void music_to_string_reduced(struct music my_music, char * buffer) {
    append_int(buffer, "Identifier=", my_music.identifier);
    append_string(buffer, "Title=", my_music.title);
    append_string(buffer, "Performer=", my_music.performer);
}

// Adiciona ao buffer todos os campos da música
void music_to_string(struct music my_music, char *buffer) {
    music_to_string_reduced(my_music, buffer);
    append_string(buffer, "Language=", my_music.language);
    append_string(buffer, "MusicType=", my_music.music_type);
    append_string(buffer, "Chorus=", my_music.chorus);
    append_int(buffer, "ReleaseYear=", my_music.release_year);
}

struct music string_to_music(char * str) {
    struct music my_music;
    char *token = strtok(str, "=");

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
    // Obtém ano de lançamento
    token = strtok(NULL, "=");
    token = strtok(NULL, "\n");
    my_music.release_year = atoi(token);

    return my_music;
}