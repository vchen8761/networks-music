#include "NetworkHeader.h"
#include "WhoHeader.h"
#include <string.h>


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
			char buffer[BUFFSIZE];
			ssize_t numBytesRcvd = 0;

			cout << "in receive Response" << endl;
			while (buffer[numBytesRcvd - 1] != '\n')		
			{
				cout << "inside while loop before recv" << endl;
				numBytesRcvd = recv(sock, buffer, BUFFSIZE-1, 0);
				cout << "inside while loop after recv" << endl;

				//printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
				//printf("buffer received: %s\n", buffer); // debugging
				if (numBytesRcvd < 0)
					DieWithError((char*) "recv() failed");
				else if (numBytesRcvd == 0)
					DieWithError((char*) "recv() failed: connection closed prematurely");
				buffer[numBytesRcvd] = '\0'; // append null-character
			}

			buffer[strlen(buffer) - 1] = '\0';
			return strlen(buffer);
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
			memset(buffer,0,BUFFSIZE);
			ssize_t numBytesRcvd = 0;

			while (buffer[numBytesRcvd - 1] != '\n')		
			 {
				numBytesRcvd = recv(cliSock, buffer, BUFFSIZE-1, 0);
				printf("numBytesRcvd: %zu\n", numBytesRcvd); // debugging
				printf("buffer received: %s\n", buffer); // debugging
				if (numBytesRcvd < 0)
					DieWithError((char*) "recv() failed");
				else if (numBytesRcvd == 0)
					DieWithError((char*) "recv() failed: connection closed prematurely");
				buffer[numBytesRcvd] = '\0'; // append null-character
			 }
				cout << strlen(buffer) << endl;
				int k;
				for (k = 0; k < 4; k++)
			{	
				cout << endl;
				cout << buffer[k] << endl;
			}

				buffer[strlen(buffer) - 1] = '\0';
			
			cout << "here" << endl;
			printf("\n%s", buffer);

			
			// Check the message type (first 4 bytes in the message)
			char typeField[5];
			int i;
			for (i = 0; i < 4; i++)
			{
				typeField[i] = buffer[i];
				//cout << buffer[i] << endl;
			}
			typeField[4] = '\0';

			printf("\nType of message received: %s\n", typeField); // debugging
			fflush(stdout);

			// Received LIST message
			if (strcmp(typeField, LISTType) == 0)
			{	

				int numEntries; // specifies number of songs
				
				char** songs = lookup_song_lists(user_name, &numEntries);	// get songs from database
				close_database_song();
				cout << numEntries << endl;
				cout << "after look up" << endl;
				char listResponse[BUFFSIZE];	// create message
				memset(listResponse, 0, BUFFSIZE);

				for (int i = 0; i < numEntries; ++i)
				{
					strncat(listResponse, songs[i], strlen(songs[i]));		// Go through each song and concatenate
				}
				cout << "after for loop" << endl;


				strncat(listResponse, "\n", 1); // terminate with new line 
				
				// send listResponse packet
				cout << listResponse << endl;
				ssize_t numBytesSent = send(cliSock, listResponse, strlen(listResponse), 0);

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
				close_database();
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
				memset(identityBuffer, 0, 5);
				if (strcmp(db_password, password) == 0) 
				{
					strncat(identityBuffer, "True", 4);				
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
				close_database();
				// If user not found in database handle case.
				if (no_of_entries == 0) {
						char emptyBuffer[1];
						strncpy(emptyBuffer, "\n", 1);
						send(cliSock, emptyBuffer, strlen(emptyBuffer), 0);
						continue;
				}

				// Parse salt from data received from database
			  	char *salt = data[0];
				char temp[SHORT_BUFFSIZE];
				memset(temp, 0, SHORT_BUFFSIZE);
				strncat(temp, salt, strlen(salt));
				salt = strtok(temp, ":");
				char saltBuffer[SHORT_BUFFSIZE];
				memset(saltBuffer, 0, SHORT_BUFFSIZE);
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
