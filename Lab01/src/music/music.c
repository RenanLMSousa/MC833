#include <stdio.h>
#include <string.h>
#include "music.h"

// Function to print the details of the music
void print_music_details(struct music my_music) {
    printf("Identifier: %d\n", my_music.identifier);
    printf("Title: %s\n", my_music.title);
    printf("Performer: %s\n", my_music.performer);
    printf("Language: %s\n", my_music.language);
    printf("Music Type: %s\n", my_music.music_type);
    printf("Chorus: %c\n", my_music.chorus);
    printf("Release Year: %d\n", my_music.release_year);
}

void music_to_string(struct music my_music, char *buffer) {
    strcpy(buffer, "Identifier: ");
    char temp_buffer[20]; // buffer temporário para converter int para string
    sprintf(temp_buffer, "%d\n", my_music.identifier);
    strcat(buffer, temp_buffer);

    strcat(buffer, "Title:");
    strcat(buffer, my_music.title);
    strcat(buffer, "\n");

    strcat(buffer, "Performer:");
    strcat(buffer, my_music.performer);
    strcat(buffer, "\n");

    strcat(buffer, "Language:");
    strcat(buffer, my_music.language);
    strcat(buffer, "\n");

    strcat(buffer, "Music Type:");
    strcat(buffer, my_music.music_type);
    strcat(buffer, "\n");

    strcat(buffer, "Chorus:");
    strcat(buffer, my_music.chorus);
    strcat(buffer, "\n");

    strcat(buffer, "Release Year:");
    sprintf(temp_buffer, "%d\n", my_music.release_year);
    strcat(buffer, temp_buffer);
}