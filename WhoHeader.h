#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace

void open_database (char *filename, char* dir);
void close_database (void);
void made(int index);
void intializeSongList();

/* Returns an array of strings where each string represents a song name
*  in the local database in the format song_name */
char** lookup_songs (int* no_of_entries);

