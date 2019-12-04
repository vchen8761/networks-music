#include "NetworkHeader.h"
#include "WhoHeader.h"
#include "sha-256.c"
#include <time.h>

using namespace std;

string user_name2;

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
	// length field is zero
	listMessage[4] = 0x0;
	listMessage[5] = 0x0;

	// printf("LIST TYPE??: ");
	// int i;
	// for (i = 0; i < 4; i++)
	// {
	// 	printf("%c", listMessage[i]);
	// }
	// printf("\n");
	
	// send LIST message to server
	ssize_t numBytesSent = send(sock, listMessage, 4+2, 0);
	if (numBytesSent < 0)
	{
		DieWithError("send() failed");
	}
}

// prints every song and SHA combination from listResponse.
// numEntries represents number of song in listResponse.
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
		unsigned long i;
		for (i = 0; i < numSongs; i++)
		{
			// retrieve song name from packet
			char songName[MAX_SONGNAME_LENGTH+1];
			strncpy(songName, packet + 2 + i*(MAX_SONGNAME_LENGTH+SHA_LENGTH), MAX_SONGNAME_LENGTH);
			songName[MAX_SONGNAME_LENGTH] = '\0';

			printf("%s\n", songName);
		}
		printf("\n");
} 

void sendLOGON(int sock)
{
	// Initialize logon info
	char username[SHORT_BUFFSIZE];
	char hashed_password[65];
	char password[SHORT_BUFFSIZE];
	uint8_t hash[32];

	// Clear new line from using scanf	
	getchar();

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
	ssize_t stringLen = strlen(username); 
	strncat(saltBuffer, "SALT", 4);
	strncat(saltBuffer, "@", 1);
	strncat(saltBuffer, username, stringLen);
	strncat(saltBuffer, "\n", 1);
	
	ssize_t saltBuffLen = strlen(saltBuffer);
	ssize_t numBytesSent = send(sock, saltBuffer, saltBuffLen, 0);
	if (numBytesSent < 0)
	  DieWithError((char*)"send() failed");	
	
	// Receive salt
	char saltArray[SHORT_BUFFSIZE];
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

	//Concatenate password and salt
	strncat(password, saltArray, strlen(saltArray));
	memset(saltArray, 0, SHORT_BUFFSIZE);

	// Hash the salted password
	calc_sha_256(hash, password, strlen(password));
	hash_to_string(hashed_password, hash);

	// Construct buffer with command, username, hashed_password
	// (seperated by @)
	char logon_info[BUFFSIZE];
	strncat(logon_info, "LOGON", 5);
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
		if (strcmp(command, "logon") == 0)
		{
			sendLOGON(sock);
			// Switch commands
			cout << "Enter Command in Small Case: " << endl;
			scanf("%s", command);
		}
		else if (strcmp(command, "list") == 0)
		{
			// send LIST message to server
			sendLIST(sock);

			// receive listResponse from server
			char listResponse[BUFFSIZE];
			unsigned long length_Message = receiveResponse(sock, listResponse);
			printLIST(listResponse, length_Message);

			// read another command from user
			printf("Enter Another Command in Small Case: ");
			scanf("%s", command);

		}
		else if (strcmp(command, "diff") == 0)
		{
			sendLIST(sock);
			// receive listResponse from server
			char listResponse[BUFFSIZE];
			unsigned long length_Message = receiveResponse(sock, listResponse);

			// calculate different song files between client and server
			char* clientSongs = compareClientToServer(listResponse+6, length_Message); // songs that are in client but not in server
			char* serverSongs = compareServerToClient(listResponse+6, length_Message); // songs that are in server but not in client

			// retrieve lengths of clientSongs and serverSongs
			unsigned long lengthClient = getLength(clientSongs);
			unsigned long lengthServer = getLength(serverSongs);

			// print songs in client but not in server
			printf("Songs in client but not in server: \n");
			printDIFF(clientSongs, lengthClient);

			// print songs in server but not in client
			printf("Songs in server but not in client: \n");
			printDIFF(serverSongs, lengthServer);

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
	exit(0);

}






