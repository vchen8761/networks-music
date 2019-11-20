
#include "NetworkHeader.h"
#include "WhoHeader.h"

using namespace std;

// receives and sets response packet to response
void HandleClient(int cliSock)
{

	for (;;) // breaks when leave message is received
	{
		// receive message from client
		char rcvMessage[BUFFSIZE];
		// receive response message from server
		int totalBytesRcvd = 0; // total number of bytes received
		for (;;)
		{
			char buffer[BUFFSIZE];
			ssize_t numBytesRcvd = recv(cliSock, buffer, BUFFSIZE-1, 0);
			//printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
			//printf("buffer received: %s\n", buffer); // debugging
			if (numBytesRcvd < 0)
				DieWithError((char*) "recv() failed");
			else if (numBytesRcvd == 0)
				DieWithError((char*) "recv() failed: connection closed prematurely");
			buffer[numBytesRcvd] = '\0'; // append null-character

			// append received buffer to response
			int u;
			for (u = totalBytesRcvd; u < totalBytesRcvd+numBytesRcvd; u++)
			{
				rcvMessage[u] = buffer[u-totalBytesRcvd];
				cout << buffer[u-totalBytesRcvd] << endl;
			}
			rcvMessage[totalBytesRcvd] = '\0';
		}



	
		// Check the message type (first 4 bytes in the message)
		char typeField[5];
		int i;
		for (i = 0; i < 4; i++)
		{
			typeField[i] = rcvMessage[i];
		}
		typeField[4] = '\0';

		printf("type of message received: %s\n", typeField); // debugging


		// Received LIST message
		if (strcmp(typeField, LISTType) == 0)
		{
			// retrieve all the songs from the database
			
		}


		// Received PULL message
		else if (strcmp(typeField, PULLType) == 0)
		{
			
		}

		//LOGON MESSAGE
		else if (strcmp(typeField, LOGONType) == 0)
		{
			
		}

		else if (strcmp(typeField, LEAVEType) == 0)
		{
			// Close database
			close(cliSock); // Close client socket
			return;
		}

		else
		{
			// do nothing
		}
	}
}
