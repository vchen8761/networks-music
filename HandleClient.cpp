
#include "NetworkHeader.h"
#include "WhoHeader.h"

using namespace std;

// receives and sets response packet to response
void HandleClient(int cliSock)
{

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
			// retrieve all the songs from the database
			
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
