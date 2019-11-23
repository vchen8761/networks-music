#include "NetworkHeader.h"
#include "sha-256.c"
#include <time.h>

using namespace std;

static void hash_to_string(char string[65], const uint8_t hash[32])
{
	size_t i;
	for (i = 0; i < 32; i++) {
		string += sprintf(string,"%02x", hash[i]);
	}
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
	cout << "Here" << endl;
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


	// ask user for command (list, diff, sync, leave)
	cout << "Enter Command in Small Case: " << endl;
	char* command = (char*) malloc(5); 
	scanf("%s", command);

	while (strcmp(command, "leave") != 0)
	{
		if (strcmp(command, "logon") == 0)
		{
			// Initialize logon info
			char username[SHORT_BUFFSIZE];
			char hashed_password[65];
			char password[SHORT_BUFFSIZE];
			uint8_t hash[32];

			// Generate salt for password
			time_t sysTime;
		 	time(&sysTime);
			char* salt = ctime(&sysTime);
			char saltArray[SHORT_BUFFSIZE];
			strncpy(saltArray, salt, sizeof(saltArray));
			printf("%s", saltArray);

			// Clear new line from using scanf	
			getchar();

			// Parse credentials input
		 	printf("Enter your username: ");
			fgets(username, SHORT_BUFFSIZE, stdin);
			printf("Enter your password: ");
			fgets(password, SHORT_BUFFSIZE, stdin);
			// Remove new line from fgets input
			password[strlen(password)-1] = '\0';
			
			//Concatenate password and salt
			strncat(password, saltArray, SHORT_BUFFSIZE - strlen(password));
			// Print out salted password for testing
			printf("%s", password);
			fflush(stdout);

			// Hash the salted password
			calc_sha_256(hash, password, strlen(password));
			hash_to_string(hashed_password, hash);

			// Print out credentials for testing
			printf("%s", username);
			printf("%s\n", hashed_password);
			fflush(stdout);
		
			

			// Switch commands
			cout << "Enter Command in Small Case: " << endl;
			scanf("%s", command);
		}
		else if (strcmp(command, "list") == 0)
		{

		}
		else if (strcmp(command, "diff") == 0)
		{

		}
		else if (strcmp(command, "pull") == 0)
		{

		}

	}
	// send LEAVE message to server
	sendLEAVE(sock);
	exit(0);

}






