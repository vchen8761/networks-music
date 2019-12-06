# A Makefile for project 4
# Comments start with a pound sign

# Macros
CLIENT_SRCS=  Project4Client.cpp
SERVER_SRCS=  Project4Server.cpp HandleClient.cpp
SRCS=Database.c listCommand.c
HDRS=NetworkHeader.h WhoHeader.h
CFLAGS= -Wall
LFLAGS= -lpthread
CC= g++

#All commands must start with a tab (not spaces)
#Some macros are built in.  $@ is the name of the 
#target being built

all: clean Project4Server Project4Client

Project4Server: $(SERVER_SRCS) $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(SERVER_SRCS) $(SRCS) $(LFLAGS)

Project4Client: $(CLIENT_SRCS) $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRCS) HandleClient.cpp $(SRCS) $(LFLAGS)

clean:
	-rm -f Project4Server Project4Client *.o