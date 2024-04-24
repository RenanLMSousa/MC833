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

void print_music_details(struct music my_music);
void music_to_string_reduced(struct music my_music, char * buffer);
void music_to_string(struct music my_music, char *buffer);
struct music string_to_music(char * str);
#endif /* MUSIC_H */
