#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace

void open_database (char *filename);
char **lookup_user_names (char *host_name, int *no_of_entries);
void close_database (void);
void made(int index);
void open_database_song (char *filename);
void close_database_song (void);
/* Returns an array of strings where each string represents a song name
*  in the local database in the format song_name */
char** lookup_song_lists (char *user_name, int *no_of_entries);

