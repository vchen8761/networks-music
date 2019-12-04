
#include "NetworkHeader.h"
#include "WhoHeader.h"


using namespace std;

char* user_name;

const char* byte_to_binary(uint8_t x, char* binary)
{
    binary[0] = '\0';

    int z;
    for (z = 128; z > 0; z >>= 1)
    {
        strcat(binary, ((x & z) == z) ? "1" : "0");
    }

    return binary;
}

unsigned long retrieveLength(char* packet)
{
		char firstBin[17]; char secondBin[9];
		byte_to_binary(packet[4], firstBin);
		byte_to_binary(packet[5], secondBin);
		strcat(firstBin, secondBin);
		return strtoul(firstBin, NULL, 2);
}

unsigned long receiveResponse(int sock, char* response)
{
	// receive response message from server
	int retrievedLength = 0; // boolean representing whether we have retrieved value from length field
	unsigned long length_Message = 0; // 2 byte field contains length of message
	int totalBytesRcvd = 0; // total number of bytes received
	for (;;)
	{
		char buffer[BUFFSIZE];
		ssize_t numBytesRcvd = recv(sock, buffer, BUFFSIZE-1, 0);
		//printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
		//printf("hello buffer received: %s\n", buffer); // debugging
		if (numBytesRcvd < 0)
			DieWithError("recv() failed");
		else if (numBytesRcvd == 0)
			DieWithError("recv() failed: connection closed prematurely");
		buffer[numBytesRcvd] = '\0'; // append null-character

		// append received buffer to response
		int u;
		for (u = totalBytesRcvd; u < totalBytesRcvd+numBytesRcvd; u++)
		{
			response[u] = buffer[u-totalBytesRcvd];
		}

		// retrieve the length field from message. (located 4th-5th bytes)
		if (!retrievedLength && numBytesRcvd >= 6)
		{
			length_Message = retrieveLength(response);

			retrievedLength = 1;
		}

		// update totalBytesRcvd;
		totalBytesRcvd = totalBytesRcvd + numBytesRcvd;

		if (totalBytesRcvd == 4 + 2 + length_Message)
		{
			response[totalBytesRcvd] = '\0';
			return length_Message;
		}

	}
}


// receives and sets response packet to response
void HandleClient(int cliSock)
{
	std::string musicDirName = "username_songs.dat";
	char *cmusicDirName = new char[musicDirName.length() + 1];
	strcpy(cmusicDirName, musicDirName.c_str());
	// open database file
	open_database_song(cmusicDirName);
		
	for (;;) // breaks when leave message is received
	{
			// receive response message from server
			char buffer[BUFFSIZE];
			ssize_t numBytesRcvd = 0;

			while (buffer[numBytesRcvd - 1] != '\n')		
			{
				numBytesRcvd = recv(cliSock, buffer, BUFFSIZE-1, 0);
				//printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
				//printf("buffer received: %s\n", buffer); // debugging
				if (numBytesRcvd < 0)
					DieWithError((char*) "recv() failed");
				else if (numBytesRcvd == 0)
					DieWithError((char*) "recv() failed: connection closed prematurely");
				buffer[numBytesRcvd] = '\0'; // append null-character
			}

			buffer[strlen(buffer) - 1] = '\0';
			printf("\n%s", buffer);

	
			// Check the message type (first 4 bytes in the message)
			char typeField[5];
			int i;
			for (i = 0; i < 4; i++)
			{
				typeField[i] = buffer[i];
			}
			typeField[4] = '\0';

			printf("\nType of message received: %s\n", typeField); // debugging
			fflush(stdout);

			// Received LIST message
			if (strcmp(typeField, LISTType) == 0)
			{	
				int numEntries; // specifies number of songs
				
				char** songs = lookup_song_lists(user_name, &numEntries);	// get songs from database

				char listResponse[BUFFSIZE];	// create message

				for (int i = 0; i < numEntries; ++i)
				{
					strncat(listResponse, songs[i], strlen(songs[i]));		// Go through each song and concatenate
				}

				strncat(listResponse, "\n", 1); // terminate with new line 
				
				// send listResponse packet
				int length_response = numEntries*(MAX_SONGNAME_LENGTH); // length of listResponse packet
				ssize_t numBytesSent = send(cliSock, listResponse, length_response, 0);
				if (numBytesSent < 0)
				{
					DieWithError("send() failed");
				}
				
			}

			// Received PULL message
			else if (strcmp(typeField, PULLType) == 0)
			{
				
			}

			//LOGON MESSAGE
			else if (strcmp(typeField, LOGONType) == 0)
			{
				char *username;
				char *password;
				username = strtok(buffer, "@");
				username = strtok(NULL, "@");
				//std::string username_temp(username);
				password = strtok(NULL, "@");
					
				// Open and parse database file for username
				std::string database_name = "database.dat";
				char *cdatabase_name = new char[database_name.length() + 1];
				strcpy(cdatabase_name, database_name.c_str());
				int no_of_entries = 0;
				int *num = &no_of_entries;
				open_database(cdatabase_name);
				char **data = lookup_user_names(username, num);
				delete [] cdatabase_name;

				printf("\nDatabase: %s", data[0]);
				printf("\nClient: %s", password);
				fflush(stdout);

				// Authenticate client password using database entry
				char *db_password = data[0];
				char temp[SHORT_BUFFSIZE];
				strncat(temp, db_password, strlen(db_password));
				db_password = strtok(temp, ":");
				db_password = strtok(NULL, ":");
		
				//TODO: Possible buffer overflow when identity buffer is greater	
				char identityBuffer[5];
				if (strcmp(db_password, password) == 0) 
				{
					strncat(identityBuffer, "True", 4);	
					//user_name = (char *) username_temp;	
					user_name = username;		
				}
				else 
				{
					strncat(identityBuffer, "False", 5);		
				}
				
				strncat(identityBuffer, "\n", 1);

				// Send salt to client
				ssize_t numBytesSent = send(cliSock, identityBuffer, strlen(identityBuffer), 0);
				if (numBytesSent < 0)
		  		DieWithError((char*)"send() failed");	
			}

			else if (strcmp(typeField, LEAVEType) == 0)
			{
				// Close database
				close(cliSock); // Close client socket
				return;
			}
			else if (strcmp(typeField, SALTType) == 0)
			{
					char *username;
					username = strtok(buffer, "@");
					username = strtok(NULL, "@");
					

					// Open and parse database file for username
					std::string database_name = "database.dat";
					char *cdatabase_name = new char[database_name.length() + 1];
					strcpy(cdatabase_name, database_name.c_str());
					int no_of_entries = 0;
					int *num = &no_of_entries;
					open_database(cdatabase_name);
					char **data = lookup_user_names(username, num);

					// If user not found in database handle case.

					// Parse salt from data received from database
				  char *salt = data[0];
					char temp[SHORT_BUFFSIZE];
					strncat(temp, salt, strlen(salt));
					salt = strtok(temp, ":");
					char saltBuffer[SHORT_BUFFSIZE];
					strncat(saltBuffer, salt, strlen(salt));
					strncat(saltBuffer, "\n", 1);

					// Send salt to client
					ssize_t saltBuffLen = strlen(saltBuffer);
					ssize_t numBytesSent = send(cliSock, saltBuffer, saltBuffLen, 0);
					if (numBytesSent < 0)
		  			DieWithError((char*)"send() failed");	
					
					delete [] cdatabase_name;
			}
			else
			{
				// do nothing
			}
	}
}
