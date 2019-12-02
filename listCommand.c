#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#define MAXIMUM_DATABASE_ENTRY_LENGTH 64
#define FIRSTFIELD_LENGTH 64
#define SECONDFIELD_LENGTH 64
#define MAX_NUM_RECORDS 100

static FILE *filePointer;
static char *userList[MAX_NUM_RECORDS];
static char **nullPtr = NULL;

void open_database ( char *filename) {
  if ( (filePointer = fopen(filename,"r")) == NULL ) {
    fprintf(stderr, "Error: Can't open file %s\n", filename);
    exit(1);
  }
}

// Given a username, this method returns the associated songs

char **lookup_song_lists (char *user_name, int *no_of_entries) {

  char *firstField = (char *) malloc(FIRSTFIELD_LENGTH + 1);
  char *secondField = (char *) malloc(SECONDFIELD_LENGTH + 1);
  char *currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  int foundAMatch = 0;
  int validHostName = 0;

  *no_of_entries = 0;

  while (fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH,
         filePointer) != NULL ) {

    firstField = strtok(currentLine, ":");
    secondField = strtok(NULL, ":");
    if ( (strcmp(firstField, "hostname") != 0) ||
   (strcmp(secondField, user_name) != 0 ) ) {
      continue;
    }
    
    validHostName = 1;

    while ( fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH, 
      filePointer) !=  NULL ) {

      firstField = strtok(currentLine, ":");
      if (( secondField = strtok(NULL, ":")) == NULL) {
  break;
      }

      // If we've reached here, the hostname is valid, and there has been a match.
      if (!foundAMatch) {
  foundAMatch = 1;
      }

      userList[*no_of_entries] = (char *) malloc(100);
      sprintf(userList[(*no_of_entries)++],"%s:%s", firstField, secondField);

    }
    break;
  }

  if (!validHostName) {
    return nullPtr;
  } else if (!foundAMatch) {
    // Make userList nonNull ptr
    userList[0] = "1";
  }
  return userList;
}


void close_database (void) {
  fclose(filePointer);
}


// Function for finding the size of a file
// Called with: findSize("file.mp3")

long int findSize(char file_name[]) 
{ 
    // opening the file in read mode 
    FILE* fp = fopen(file_name, "r"); 
  
    // checking if the file exist or not 
    if (fp == NULL) { 
        printf("File Not Found!\n"); 
        return -1; 
    } 
  
    fseek(fp, 0L, SEEK_END); 
  
    // calculating the size of the file 
    long int res = ftell(fp); 
  
    // closing the file 
    fclose(fp); 
  
    return res; 
}


// Testing 
int main()
{

  open_database("username_songs.dat");
  int numberOfEntries;
  char **songList;
  songList = lookup_song_lists("username2", &numberOfEntries);
  close_database();

  int i;
  int j;
  for (i = 0; i < numberOfEntries; ++i)
  {
    for (j = 0; j < 64; ++j)
    {
      printf("%c", songList[i][j]);
    }
    printf("%s\n", "");
  }

  printf("%lu\n", findSize("eventually.mp3"));

  return 0;
}




