# networks-music

sample logon credentials:

username: user1
password: password123

GetMyMusic is a networked application that enables multiple machines to ensure that they have the same files in their music directory.

username_songs.dat files contains song names and corresponding size/SHAs for the server.
username_songs_client.dat files contains song names and corresponding size/SHAs for the client.
listCommand.c is used to extract song names given username from the database given.
Database.c is used to extract login information from database securely.
HandleClient.cpp contains code for server side handling different requests.
To compile type in make
From client side type in the format ./Project4Client [-s <server IP/Name>[:<port>]]
From server side type in the format ./Project4Server -p <port>


