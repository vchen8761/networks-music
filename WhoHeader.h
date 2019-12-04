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
char* compareClientToServer(char* inputBuffer, int numBufferEntries);
char* compareServerToClient(char* inputBuffer, int numBufferEntries);
int listContainsSong(char* comparedSha, char* inputBuffer, int numBufferEntries);
void convertLengthToTwoBytes(char* ptr, unsigned long length);
int containsSong(char* comparedSha) ;

/**
* Takes as a parameter a pointer to the first of 2 bytes of a field specifying length.
*
* Shifts the first byte 8 orders of magnitude to the left and adds it to the value
* of the bit on the right
**/
unsigned int getLength(char* field);


