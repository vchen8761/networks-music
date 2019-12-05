#include "NetworkHeader.h"
#include "WhoHeader.h"
#include "sha-256.c"
#include <time.h>
#include <string.h>

using namespace std;

string user_name2;
char** listOfSongs;
int serverNumEntries;

static void hash_to_string(char string[65], const uint8_t hash[32])
{
	size_t i;
	for (i = 0; i < 32; i++) {
		string += sprintf(string,"%02x", hash[i]);
	}
}

unsigned int getLength(char* field)
{
	char firstBin[17]; char secondBin[9];
	byte_to_binary(field[0], firstBin);
	byte_to_binary(field[1], secondBin);
	strcat(firstBin, secondBin);
	return strtoul(firstBin, NULL, 2);
}	


// Constructs and sends LIST message to server.
void sendLIST(int sock)
{
	// construct LIST message
	char listMessage[BUFFSIZE];
	strcpy(listMessage, LISTType);
	strncat(listMessage, ":", 1);
	//strncat(listMessage, user_name2, strlen(user_name2));
	strncat(listMessage, "\n", 1);
	
	// send LIST message to server
	ssize_t numBytesSent = send(sock, listMessage, 4+2, 0);
	if (numBytesSent < 0)
	{
		DieWithError("send() failed");
	}
}


// Receive the song list
// uses global variable listOfSongs
void receiveSongList(int sock)
{

	// receive response message from server
	char buffer[BUFFSIZE];
	ssize_t numBytesRcvd = 0;
	serverNumEntries = 0;
	cout << "in receive song list" << endl;

	while (buffer[numBytesRcvd - 1] != '\n')		
	{
		numBytesRcvd = recv(sock, buffer, BUFFSIZE-1, 0);
		//printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
		//printf("buffer received: %s\n", buffer); // debugging
		if (numBytesRcvd < 0)
			DieWithError((char*) "recv() failed");
		else if (numBytesRcvd == 0)
			DieWithError((char*) "recv() failed: connection closed prematurely");
		buffer[numBytesRcvd] = '\0'; // append null-character
	}
	cout << "between" << endl;


	char* song = strtok(buffer, ":");
	int i = 0;

	while(strcmp(song, "\n") != 0)
	{
		listOfSongs[i] = song;
		printf("%s\n", listOfSongs[i]);
		fflush(stdout);
		serverNumEntries++;
		song = strtok(NULL, ":");
		song = strtok(NULL, ":");
		i++;
	}
	cout << "after between" << endl;
}

/**************************************************************************************************************** 
* Takes as a parameter the list of songs:SHA pairings to compare to our local database contents.
*
* Returns a list of songs:SHA pairings of files not stored on the server database based on SHA values. 
*
* Songs that are in client but not in server
****************************************************************************************************************/
char* compareClientToServer() 
{
  int numClientEntries; // specifies number of songs
  char* chr = strdup(user_name2.c_str());
  cout << "before look up song list" << endl;
  char** clientSongs = lookup_song_lists(chr, &numClientEntries); // get songs from database
  // BUG: cant come out of look up song list
  cout << "after look up song list" << endl;

  close_database_song();
  free(chr);
  cout << "after close song list" << endl;

  // current line of local database
  char* currentLine = (char*) malloc(SONG_LENGTH + SHA_LENGTH + 1);

  // allocate a variable for names in the buffer
  char* name = (char*) malloc(SONG_LENGTH);
  char* clientSha = (char*) malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = (char*) malloc(BUFFSIZE);

  // pointer to a location in 'result' being added to 
  char* ptr = result;

  // move pointer past location that will store the length field
  ptr += 2;

  // number of songs currently in the array being returned
  int numResults = 0; 


  // go through the entire client song list
  int i;
    cout << "before for loop" << endl;

  for (i = 0; i < numClientEntries; i++)
  {
      // retrieve the current song and sha
      strcpy(currentLine, clientSongs[i]);

      // get the song name only
      name = strtok(currentLine, ":");

      // retrieve the SHA
      clientSha = strtok(NULL, ":");

      // if the song is not found in the server song list, add it to output buffer
      char* currentLine2 = (char*) malloc(SONG_LENGTH + SHA_LENGTH + 1);
      int k;
      bool found = false;
        cout << "before inner for loop" << endl;

      for(k = 0; k < serverNumEntries; k++)
      {	

      	strcpy(currentLine2, listOfSongs[k]);
      	strtok(currentLine2, ":");

      	if(strtok(NULL, ":") == clientSha){
      		found = true;
      	}
      }
      if(found == false) // FALSE
      {
        // add song name : SHA to result to be returned
        strncpy(ptr, name, SONG_LENGTH); 

        ptr += SONG_LENGTH;

        strncpy(ptr, clientSha, SHA_LENGTH);
        ptr += SHA_LENGTH;
        numResults++;
      }
  }
	//printf("numEntries %d\n", numResults);
  // null terminate the result
  strcat(result, "\0");

  // add the length field to the first 2 bytes of result
  convertLengthToTwoBytes(result, numResults);

  return result;
}



/**************************************************************************************************************** 
* Takes as a parameter the list of songs:SHA pairings to compare to our local database contents.
*
* Returns a list of songs:SHA pairings of files not stored on our local database based on SHA values.
*
* Songs that are in server but not in client 
****************************************************************************************************************/
char* compareServerToClient() 
{

  int numClientEntries; // specifies number of songs
  char* chr = strdup(user_name2.c_str());
  char** clientSongs = lookup_song_lists(chr, &numClientEntries); // get songs from database
  close_database_song();
  free(chr);

  // current line of local database
  char* currentLine = (char*) malloc(SONG_LENGTH + SHA_LENGTH + 1);

  // allocate a variable for names in the buffer
  char* name = (char*) malloc(SONG_LENGTH);
  char* serverSha = (char*) malloc(SHA_LENGTH + 1);

  // string to be returned containing songs
  char* result = (char*) malloc(BUFFSIZE);

  // pointer to a location in 'result' being added to 
  char* ptr = result;

  // move pointer past location that will store the length field
  ptr += 2;

  // number of songs currently in the array being returned
  int numResults = 0; 


  // go through the entire server song list
  int i;
  for (i = 0; i < serverNumEntries; i++)
  {
      // retrieve the current song and sha
      strcpy(currentLine, listOfSongs[i]);

      // get the song name only
      name = strtok(currentLine, ":");

      // retrieve the SHA
      serverSha = strtok(NULL, ":");

      // if the song is not found in the client song list, add it to output buffer
      char* currentLine2 = (char*) malloc(SONG_LENGTH + SHA_LENGTH + 1);
      int k;
      bool found = false;
      for(k = 0; k < numClientEntries; k++)
      {	
      	strcpy(currentLine2, clientSongs[k]);
      	strtok(currentLine2, ":");

      	if(strtok(NULL, ":") == serverSha){
      		found = true;
      	}
      }
      if(found == false) // FALSE
      {
        // add song name : SHA to result to be returned
        strncpy(ptr, name, SONG_LENGTH); 

        ptr += SONG_LENGTH;

        strncpy(ptr, serverSha, SHA_LENGTH);
        ptr += SHA_LENGTH;
        numResults++;
      }
  }
	//printf("numEntries %d\n", numResults);
  // null terminate the result
  strcat(result, "\0");

  // add the length field to the first 2 bytes of result
  convertLengthToTwoBytes(result, numResults);

  return result;
}


// prints every song and SHA combination.
// numEntries represents number of song.
void printLIST(char* listResponse, unsigned long numEntries)
{
	// print the names of the songs in the server to stdout
	//// need to implement
}

// Constructs and sends LEAVE message to server.
void sendLEAVE(int sock)
{
	// Construct LEAVE message
	char leaveMessage[BUFFSIZE];
	strcpy(leaveMessage, LEAVEType);
	// length field is zero
	leaveMessage[4] = 0x0;
	leaveMessage[5] = 0x0;
	//cout << "In send leave1" << endl;

	ssize_t stringLen = strlen(leaveMessage); 
  	// send LEAVE message to server
	ssize_t numBytesSent = send(sock, leaveMessage, stringLen, 0);
	  if (numBytesSent < 0)
	    DieWithError((char*)"send() failed");
	
}

// Prints every song names in given packet.
// numSongs represents number of songs in packet
// First two bytes: length field, then song name, SHA, song name, SHA,...
void printDIFF(char* packet, unsigned long numSongs)
{

} 

void sendLOGON(int sock)
{
	// Initialize logon info
	char username[SHORT_BUFFSIZE];
	char hashed_password[65];
	char password[SHORT_BUFFSIZE];
	uint8_t hash[32];

	// Parse username input
 	printf("Enter your username: ");
	fgets(username, SHORT_BUFFSIZE, stdin);
	// Remove new line from fgets input
	username[strlen(username)-1] = '\0';

	// Parse password input
	printf("Enter your password: ");
	fgets(password, SHORT_BUFFSIZE, stdin);
	// Remove new line from fgets input
	password[strlen(password)-1] = '\0';
	
	// Request salt from server based on username
	char saltBuffer[SHORT_BUFFSIZE];
	memset(saltBuffer, 0, SHORT_BUFFSIZE);
	ssize_t stringLen = strlen(username); 
	strncpy(saltBuffer, "SALT", 4);
	strncat(saltBuffer, "@", 1);
	strncat(saltBuffer, username, stringLen);
	strncat(saltBuffer, "\n", 1);
	
	ssize_t saltBuffLen = strlen(saltBuffer);
	ssize_t numBytesSent = send(sock, saltBuffer, saltBuffLen, 0);
	if (numBytesSent < 0)
	  DieWithError((char*)"send() failed");	
	
	// Receive salt
	char saltArray[SHORT_BUFFSIZE];
	memset(saltArray, 0, SHORT_BUFFSIZE);
	ssize_t numBytes = 0;
	// Receive until new line character 
	while (saltArray[numBytes - 1] != '\n') {
		/* Receive up to the buffer size (minus 1 to leave space for
 		 a null terminator) bytes from the sendor */
		numBytes = recv(sock, saltArray, SHORT_BUFFSIZE - 1, 0);		
		if (numBytes < 0)
			DieWithError("recv() failed");
		else if (numBytes == 0)
			DieWithError("recv() failed, connection closed prematurely");
	}
	saltArray[strlen(saltArray) - 1] = '\0';
	printf("Salt: %s\n", saltArray);

	char empty[SHORT_BUFFSIZE];
	if(strcmp(saltArray, empty) == 0) {
		printf("%s\n", "Incorrect credentials");
		return;
	}

	//Concatenate password and salt
	strncat(password, saltArray, strlen(saltArray));
	memset(saltArray, 0, SHORT_BUFFSIZE);

	// Hash the salted password
	calc_sha_256(hash, password, strlen(password));
	hash_to_string(hashed_password, hash);

	// Construct buffer with command, username, hashed_password
	// (seperated by @)
	char logon_info[BUFFSIZE];
	memset(logon_info, 0, BUFFSIZE);
	strncpy(logon_info, "LOGON", 5);
	strncat(logon_info, "@", 1);
 	strncat(logon_info, username, strlen(username));
	strncat(logon_info, "@", 1);
	strncat(logon_info, hashed_password, strlen(hashed_password));
	strncat(logon_info, "\n", 1);

	printf("%s", logon_info);

	// Send logon_info to server for authentication
	ssize_t bufferLen = strlen(logon_info); 
 	// send logon_info message to server
	numBytesSent = send(sock, logon_info, bufferLen, 0);
	if (numBytesSent < 0)
	  DieWithError((char*)"send() failed");

	// RECEIVE ANSWER FROM SERVER: Is password hash correct or not?
	char identityBuffer[SHORT_BUFFSIZE];
	memset(identityBuffer, 0, SHORT_BUFFSIZE);
	numBytes = 0;
	// Receive until new line character 
	while (identityBuffer[numBytes - 1] != '\n') {
		/* Receive up to the buffer size (minus 1 to leave space for
 		 a null terminator) bytes from the sendor */
		numBytes = recv(sock, identityBuffer, SHORT_BUFFSIZE - 1, 0);		
		if (numBytes < 0)
			DieWithError("recv() failed");
		else if (numBytes == 0)
			DieWithError("recv() failed, connection closed prematurely");
	}

	// If correct credentials then prints True, otherwise prints False.
	printf("%s", identityBuffer);

	if (strcmp(identityBuffer, "True\n") == 0)
		user_name2 = username;

}


int SetupTCPClientSocket(const char *host, const char *service)
{
	// Tell the system what kind(s) of address info we want
	struct addrinfo addrCriteria; // Criteria for address match
	memset(&addrCriteria, 0, sizeof(addrCriteria)); // Zero out structure
	addrCriteria.ai_family = AF_UNSPEC; //v4 or v6 is ok
	addrCriteria.ai_socktype = SOCK_STREAM	; // Only streaming sockets
	addrCriteria.ai_protocol = IPPROTO_TCP; // Only TCP protocol

	// Get address(es)
	struct addrinfo *servAddr; // Holder for returned list of server addrs
	int rtnVal = getaddrinfo(host, service, &addrCriteria, &servAddr);
	if (rtnVal != 0)
		DieWithError((char*)"gai_strerror(rtnVal)");

	int sock = -1;
	struct addrinfo *addr;
	for (addr = servAddr; addr != NULL; addr = addr->ai_next)
	{
		// Create a reliable, stream socket using TCP
		sock = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
		if (sock < 0)
			continue; // Socket creation failed; try next address

		// Establish the connection
		if (connect(sock, addr->ai_addr, addr->ai_addrlen) == 0)
			break; // Scoekt connection succeeded; break and return socket

		close(sock); // Socket connection failed; try next address
		sock = -1;
	}

	freeaddrinfo(servAddr); // Free addrinfo allocated in getaddrinfo()
	return sock;
}

const char *serverHost;

/* function declarations */
int main (int argc, char *argv[])
{
	// Argument parsing variables
  serverHost = SERVER_HOST;
  //unsigned short serverPort = atoi(SERVER_PORT);
  char *servPortString;
  char c;
  int i;

  if ((argc < 1) || (argc > 3)) {
    printf("Error: Usage Project0Client [-s <server IP/Name>[:<port>]]\n");
    exit(1);
  }

  for (i = 1; i < argc; ++i) {
    if (argv[i][0] == '-') {
      c = argv[i][1];

      /* found an option, so look at next
       * argument to get the value of 
       * the option */
      switch (c) {

      case 's':
	serverHost = strtok(argv[i+1],":");
	if ((servPortString = strtok(NULL, ":")) != NULL) {
	  //serverPort = htons(atoi(servPortString));
	}
	break;
      default:
	break;
      }
    }
  }

  	// create a connected TCP socket
	int sock = SetupTCPClientSocket(serverHost, servPortString);
	if (sock < 0)
		DieWithError((char*) "SetupTCPClientSocket() failed");

	while (user_name2 == "")
		sendLOGON(sock);

	cout << user_name2 << endl;

	// open database file
	//open_database("username_songs.dat");

	// ask user for command 
	cout << "Enter Command in Small Case: " << endl;
	char* command = (char*) malloc(5); 
	scanf("%s", command);

	while (strcmp(command, "leave") != 0)
	{
		if (strcmp(command, "list") == 0)
		{
			// send LIST message to server
			sendLIST(sock);
			cout << "out of sendList" << endl;
			// receive listResponse from server
			//char listResponse[BUFFSIZE];
			receiveSongList(sock);

			cout << "after receive response" << endl;
			//printLIST(listResponse, length_Message);

			// read another command from user
			printf("Enter Another Command in Small Case: ");
			scanf("%s", command);

		}
		else if (strcmp(command, "diff") == 0)
		{
			sendLIST(sock);
			// update server side songs
			receiveSongList(sock);
			cout << "after receive song list" << endl;
			// calculate different song files between client and server
			char* clientSongs = compareClientToServer(); // songs that are in client but not in server
			cout << "after compare client to server" << endl;
			char* serverSongs = compareServerToClient(); // songs that are in server but not in client

			cout << clientSongs << endl;
			cout << serverSongs << endl;

			// wait for another command
			printf("Enter Another Command in Small Case: ");
			scanf("%s", command);

		}
		else if (strcmp(command, "pull") == 0)
		{

		}

	}
	// send LEAVE message to server
	sendLEAVE(sock);
	//exit(0);

}






