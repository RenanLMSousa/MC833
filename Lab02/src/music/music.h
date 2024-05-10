#ifndef MUSIC_H
#define MUSIC_H

#define TITLE_LENGTH 100
#define PERFORMER_LENGTH 100
#define LANGUAGE_LENGTH 100
#define MUSIC_TYPE_LENGTH 100
#define CHORUS_LENGTH 3000

struct music {
    int identifier;
    char title[TITLE_LENGTH];
    char performer[PERFORMER_LENGTH];
    char language[LANGUAGE_LENGTH];
    char music_type[MUSIC_TYPE_LENGTH];
    char chorus[CHORUS_LENGTH];
    int release_year;
};

// Imprime os campos da estrutura music
void print_music_details(struct music my_music);

// Adiciona no fim da string buffer uma string chave e um valor inteiro
void append_int(char * buffer, char * key, int value);

// Adiciona ao buffer os campos identifier, title e performer de uma música
void music_to_string_reduced(struct music my_music, char * buffer);

// Adiciona ao buffer todos os campos da música
void music_to_string(struct music my_music, char *buffer);

// Retorna uma estrutura com as informações de str
struct music string_to_music(char * str);

#endif /* MUSIC_H */
