// #ifndef NETWORK_HEADER_H 
// #define NETWORK_HEADER_H

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <netdb.h>
#include <iostream>
//#include <stdint.h>			/* for uint16_t */
//#include <inttypes.h>		/* for printing uint16_t */
#include <pthread.h>        /* for multi-clienting */

#define DATABASE_NAME "database.dat"
#define SERVER_HOST "141.166.207.144" 
#define SERVER_PORT "30500"

#define SA struct sockaddr

/* Miscellaneous constants */
#define	MAXLINE		4096	/* max text line length */
#define	MAXSOCKADDR  128	/* max socket address structure size */
#define	BUFFSIZE	66535	/* buffer size for reads and writes */
#define	LISTENQ		1024	/* 2nd argument to listen() */
#define SHORT_BUFFSIZE  100     /* For messages I know are short */

#define LISTType "LIST"
#define PULLType "PULL"
#define LOGONType "LOGON"
#define LEAVEType "LEAV"
#define SALTType "SALT"

inline void DieWithError(const char *errorMessage){
	perror(errorMessage);
  	exit(1);
} 
/*Error handling function */
int SetupTCPClientSocket(const char *host, const char *service);
void HandleClient(int cliSock);
