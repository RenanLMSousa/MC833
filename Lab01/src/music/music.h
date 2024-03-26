#ifndef MUSIC_H
#define MUSIC_H

#define TITLE_LENGTH 100
#define PERFORMER_LENGTH 100
#define LANGUAGE_LENGTH 50
#define MUSIC_TYPE_LENGTH 50
#define FILENAME_LENGTH 100

struct music {
    int identifier;
    char title[TITLE_LENGTH];
    char performer[PERFORMER_LENGTH];
    char language[LANGUAGE_LENGTH];
    char music_type[MUSIC_TYPE_LENGTH];
    int release_year;
    char filename[FILENAME_LENGTH];
};

// Function prototype to print music details
void print_music_details(struct music my_music);
void music_to_string(struct music my_music, char *buffer);
#endif /* MUSIC_H */
