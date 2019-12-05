#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include "WhoHeader.h"
#include "NetworkHeader.h"

#define MAXIMUM_DATABASE_ENTRY_LENGTH 64
#define FIRSTFIELD_LENGTH 64
#define SECONDFIELD_LENGTH 64
#define MAX_NUM_RECORDS 100

static FILE *filePointer;
static char *userList[MAX_NUM_RECORDS];
static char **nullPtr = NULL;
//static char* songList[MAX_NUM_RECORDS]; // list of song:sha pairings in local database
static int numEntries; // number of entries in local database

void open_database_song ( char *filename) {
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
    //userList[0] = '1';
  }
  numEntries = *no_of_entries;
  return userList;
}


void close_database_song (void) {
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



/**************************************************************************************************************** 
* Takes as a parameter the list of songs:SHA pairings to compare to our local database contents.
*
* Returns a list of songs:SHA pairings of files not stored on our local database based on SHA values.
*
* Songs that are in server but not in client 
****************************************************************************************************************/
char* compareServerToClient(char* inputBuffer, int numBufferEntries) 
{

  int i;
  // allocate a variable for names in the buffer
  char* name = (char*) malloc(SONG_LENGTH);
  char* sha = (char*) malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = (char*) malloc(BUFFSIZE);

  // pointer to a location in 'result' being added to 
  char* ptr = result;

  ptr = ptr+2;

  // number of songs currently in the array being returned
  int numResults = 0;

  // go through the entire input buffer
  for (i = 0; i < numBufferEntries/(SONG_LENGTH+SHA_LENGTH); i++)
  {

      // retrieve the current song
      strncpy(name, inputBuffer, SONG_LENGTH);

      int j;
      for (j = 0; j < 255; j++)
      {
        printf("%c", name[j]);
      }
      printf("\n");

      // move pointer past current song to start of sha field
      inputBuffer += SONG_LENGTH;

      // retrieve the current sha
      strncpy(sha, inputBuffer, SHA_LENGTH);

      // move pointer past current sha to start of next song field
      inputBuffer += SHA_LENGTH;

      // if the song is not found in the database, add it to output buffer
      if(containsSong(sha) == 0) 
      {
        // add song name to result to be returned
        strncpy(ptr, name, SONG_LENGTH);

        // move pointer past song and null character to place sha is to be added
        ptr += SONG_LENGTH;

        // add sha to result to be returned
        strncpy(ptr, sha, SHA_LENGTH);

        ptr = ptr + SHA_LENGTH;

        numResults++;
      }
  }

  // null terminate the result
  strcat(result, "\0");


  // add the length field to the first 2 bytes of result
  convertLengthToTwoBytes(result, numResults);

  return result;
}

/****************************************************************************************************************
* Takes filename and SHA value as parameters and determines if the database contains that file
*  and returns 1 if true and 0 if false. 
****************************************************************************************************************/
int listContainsSong(char* comparedSha, char* inputBuffer, int numBufferEntries)
{
  // for holding the current SHA retrieved by breaking apart the song:SHA pairing with strtok()
  char* shaField = (char*) malloc(SHA_LENGTH);

  // move pointer past first song to start of sha
  inputBuffer += SONG_LENGTH;

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numBufferEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strncpy(shaField, inputBuffer, SHA_LENGTH);

    // move pointer past current sha and past next song name to start of next sha
    inputBuffer += SHA_LENGTH + SONG_LENGTH;
    if(strcmp(comparedSha, shaField) == 0) // if passed in SHA equals current SHA
    {
      return 1; // return true
    }
  }

  return 0; // return false
}

/****************************************************************************************************************
* Takes filename and SHA value as parameters and determines if the database contains that file based on SHA value
* and returns 1 if true and 0 if false.
****************************************************************************************************************/
int containsSong(char* comparedSha) 
{
  // for holding the current song:SHA pairing to be broken apart by strtok()
  char* currentLine = (char *)  malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1);

  // for holding the current SHA retrieved by breaking apart the song:SHA pairing with strtok()
  char* shaField = (char *) malloc(SHA_LENGTH + 1);

  // loop through the list of song:SHA pairings in the local database
  int i;
  for (i = 0; i < numEntries; i++) 
  {
    // get current song and sha into a temporary variable
    strcpy(currentLine, userList[i]);

    // break the current line along the ':' to prepare to get the SHA
    strtok(currentLine, ":"); 

    shaField = strtok(0, ":"); // get the second field of the current line

    shaField[SHA_LENGTH] = '\0'; // turn new line character to null 

    if(strcmp(comparedSha, shaField) == 0) // if passed in SHA equals current SHA
    {
      return 1; // return true
    }
  }

  return 0; // return false
}

void convertLengthToTwoBytes(char* ptr, unsigned long length)
{
  ptr[1] = (uint16_t)length;
  ptr[0] = (uint16_t)length >> 8;

}



// // Testing 
// int main()
// {

//   open_database("username_songs.dat");
//   int numberOfEntries;
//   char **songList;
//   songList = lookup_song_lists("username2", &numberOfEntries);
//   close_database();

//   int i;
//   int j;
//   for (i = 0; i < numberOfEntries; ++i)
//   {
//     for (j = 0; j < 64; ++j)
//     {
//       printf("%c", songList[i][j]);
//     }
//     printf("%s\n", "");
//   }

//   printf("%lu\n", findSize("eventually.mp3"));

//   return 0;
// }
