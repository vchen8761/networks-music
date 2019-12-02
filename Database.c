
#include "WhoHeader.h"

#define MAXIMUM_DATABASE_ENTRY_LENGTH 90
#define FIRSTFIELD_LENGTH 8
#define SECONDFIELD_LENGTH 20
#define MAX_NUM_RECORDS 100
#define SHA_LENGTH 128


static FILE *filePointer;
static char* songList[MAX_NUM_RECORDS]; // list of songs in local database
static char **nullPtr = NULL;
static int numEntries; // number of entries in local database
static char* songDir;

void open_database (char *filename, char* dir) {
  if ( (filePointer = fopen(filename,"r+")) == NULL ) {
    fprintf(stderr, "Error: Can't open file %s\n", filename);
    exit(1);
  }

  songDir = strdup(dir);
  //sync_database(dir);
  intializeSongList();
  printf("Number of songs: %d\n", numEntries);
}

void close_database (void) {
  // fclose(filePointer);
  int i;
  for (i = 0; i < numEntries; i++)
  {
    free(songList[i]);
  }
  numEntries = 0;
  fclose(filePointer);
}

void intializeSongList()
{
  char* currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding song name when going through the database
  char* firstField = malloc(SONG_LENGTH + 1);

  // intialize the number of entries
  numEntries = 0;

  // loop through the entries on each line of the database until null is encountered
  while (fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH, filePointer) !=  NULL ) 
  { 
      // retrieve the name of the song at the current line
      firstField = strtok(currentLine, ":");

      // allocate space for a song in the classwide variable
      songList[numEntries] = (char *) malloc(SONG_LENGTH + 2); 

      // add a song and its SHA to the list of entries in the classwide variable
      sprintf(songList[(numEntries)++],"%s:%s", firstField);

  }
}

/****************************************************************************************************************
* Takes as a parameter a location in which to store the number of files in the database.
*
* Returns an array containing the song names in the database
****************************************************************************************************************/
char** lookup_songs (int* no_of_entries) 
{
  // specify the number of songs in the list into the passed in variable
  *no_of_entries = numEntries;

  // return the location to where the contents of the local database where stored
  return songList;
}




