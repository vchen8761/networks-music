# networks-music

GetMyMusic is a networked application that enables multiple machines to ensure that they have the same files in their music directory.

How to run:
Run the client and server on different mathcs machines using the following command line arguments:

./Project4Client -s [Server IP]:[Port]
  
./Project4Server -p [Port]
  
The client with prompt the user to logon using their username and password. 
The user is assumed to have an account because REGISTER option is not implemented.

After entering their credentials, the user can type in any of the following commands:
- List
- Logon
- Diff
- Pull
- Leave

List command list the songs that the server has associated with the username.
Logon command sends new credentials to the server for authentication.
Diff command request the differences in song lists between the client and server.
Pull command updates the server and the client list to match.
Leave command disconnects client from user.

Hashing and Encryption:
Passwords not sent as plaintext over the network. Rather, the user has an associated salt in the server database.
This salt along with SHA-256 are used to secure the password.
SHA-256 is implemented using: https://github.com/amosnier/sha-2


Testing:
This program was tested on mathcs machines 01,02, and 03. 
The sample logon credentials are:
username: user1
password: password123


