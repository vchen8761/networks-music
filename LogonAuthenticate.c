#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>  // For isspace

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


// This method returns 0 for a non-existent username,
// returns 1 for a existing username but incorrect password,
// and returns 2 for a correct username and password. 

int lookup_user_names (char *user_name, char *password) 
{

  char *firstField = (char *) malloc(FIRSTFIELD_LENGTH + 1);              // field for storing usernames
  char *secondField = (char *) malloc(SECONDFIELD_LENGTH + 1);            // field for storing passwords
  char *currentLine = (char *) malloc(MAXIMUM_DATABASE_ENTRY_LENGTH + 1); // field for the current line

  int validUserName = 0;    // does the username exist
  int validPassword = 0;    // is it the correct password

  // iterate through the database of usernames and passwords
  while (fgets(currentLine, MAXIMUM_DATABASE_ENTRY_LENGTH, filePointer) != NULL ) 
  {
    // set variables to the first and second fields of each database line
    firstField = strtok(currentLine, ":");
    secondField = strtok(NULL, ":");

    // if statement to check validity of username and password
    if(strcmp(firstField, user_name) == 0)    // if given username equals an entry
    {
      validUserName = 1;                      // mark username as valid
      if(strcmp(secondField, password) == 0)  // assuming username is valid, if given password equals the correct password
      {
        validPassword = 1;                    // mark password as valid
      }
    }
  }

  // Cases for each scenario
  if(validUserName == 0) {return 0;} // If the username does not exist

  if(validUserName == 1 && validPassword == 0){return 1;} // if the username exists but the password is wrong

  if(validUserName == 1 && validPassword == 1){return 2;} // if the username and pasword are both correct

  return 0;
}

void close_database (void) {
  fclose(filePointer);
}

// Testing 
int main()
{

  open_database("username-password.dat");
  lookup_user_names("joe", "rosen");
  close_database();

  open_database("username-password.dat");
  lookup_user_names("jeremiah", "yikes");
  close_database();

  open_database("username-password.dat");
  lookup_user_names("teresa", "black");
  close_database();

  open_database("username-password.dat");
  lookup_user_names("irene", "passwordpassword");
  close_database();

  open_database("username-password.dat");
  lookup_user_names("edward", "youshallnotpassword");
  close_database();

  return 0;
}




