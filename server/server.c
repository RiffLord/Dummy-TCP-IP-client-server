/*
 *	TCP/IP Server
 *	
 *	While running it listens for connection
 *	requests from clients. After a connection
 *	is successfully established it sends a message
 *	and waits for the client to transmit its data.
 *	The data is reelaborated and sent back to the
 *	client; the server continues to listen for new
 *	connection requests until terminated by the user.
 */
 
 

#if defined WIN32
#include <winsock.h>
#else

//	If winsock.h is unavailable	

#define closesocket close

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>

#endif // defined

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define PORT 8080
#define STRL 200
#define BUFF 500



//	The data received from the client contains two strings
struct strings {
    char str1[STRL];
    char str2[STRL];
};



//============================== Function prototypes ==============================//

void toUppercase(char*, char*);
void toLowercase(char*, char*);
void sendToClient(int, char*);
void sendStringsToClient(int, struct strings*);
void receiveFromClient(int, struct strings*);
void error(const char*);
void initWinSock();
void clearWinSock();

//=================================================================================//

int main(int argc, char *argv[]) {
    int nSockFd, nNewSockFd;
    int nClientLength;
    struct strings sUnformatted, sFormatted;			//	Stores the data received from the client and the data to send back to it respectively
    struct sockaddr_in serverAddress, clientAddress;

    initWinSock();
	
	//	Initializes a socket
	//	AF_INET: Internet Protocol v4 Family
	//	SOCK_STREAM: socket type for TCP connections
	//	0: default protocol
    nSockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (nSockFd < 0) {
        error("Error initializing socket...");
    }
	
	//	Initializes the server address with the necessary data,
	//	including IP & Port numbers
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");	//	Converts the IP address to a 32-bit number
    serverAddress.sin_port = htons(PORT);					//	Converts the port number from host to network short byte order

	//	Binds the address to the socket and checks for errors.
	//	Closes the socket and terminates execution if anything went wrong.
    if (bind(nSockFd, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        error("Error binding server address to socket...");
        closesocket(nSockFd);
        clearWinSock();

        return 0;
    }

	//	Listens for connection requests.
	//	The server can accept a maximum 
	//	of 5 connections at a time.
    if (listen(nSockFd, 5) < 0) {
        error("listen() failed...");
        closesocket(nSockFd);
        clearWinSock();

        return 0;
    }

    printf("%s\n%s%d\n", "Waiting for incoming connection requests...", "Port: ", ntohs(serverAddress.sin_port));
	
	//	Infinite loop to keep the server running and listening for new connections
    for (;;) {
        nClientLength = sizeof(clientAddress);	//	The size of the clientAddress is passed to accept()
		
		//	Accepts a connection request from a client at a temporary socket
        nNewSockFd = accept(nSockFd, (struct sockaddr*) &clientAddress, &nClientLength);

        if (nNewSockFd < 0) {
            error("Error accepting...");
            closesocket(nSockFd);
            clearWinSock();

            return 0;
        }

        printf("%s%s%d\n", inet_ntoa(clientAddress.sin_addr), ": ACCEPTED\nPort: ", ntohs(clientAddress.sin_port));
        
		//	Sends a message notifying the client that a conncection has been established
		sendToClient(nNewSockFd, "Connection successful");

		//	Receives data from the client and displays it on screen
        receiveFromClient(nNewSockFd, &sUnformatted);
        printf("%s%s\n", "\nString 1: ", sUnformatted.str1);
        printf("%s%s\n", "\nString 2: ", sUnformatted.str2);
		
		//	Formats the strings received
        toUppercase(sFormatted.str1, sUnformatted.str1);
        toLowercase(sFormatted.str2, sUnformatted.str2);
		
		//	Sends the formatted data back to the client.
        sendStringsToClient(nNewSockFd, &sFormatted);
    }

	//	Once the user terminates the program, closes the socket and exits
    closesocket(nSockFd);
    clearWinSock();

    return 0;
}



//============================== Function definitions ==============================//


//	Formats the string received from the client
void toUppercase(char* sFormatted, char* sUnformatted) {
    int charPointer = 0;
	
	//	Iterates through the unformatted string converting the characters to uppercase
    while (sUnformatted[charPointer]) {
        sFormatted[charPointer] = toupper(sUnformatted[charPointer]);
        charPointer++;
    }
}


//	Formats the string received from the client
void toLowercase(char* sFormatted, char* sUnformatted) {
    int charPointer = 0;

	//	Iterates through the unformatted string converting the characters to lowercase
    while (sUnformatted[charPointer]) {
        sFormatted[charPointer] = tolower(sUnformatted[charPointer]);
        charPointer++;
    }
}

//	Sends data to the client
void sendToClient(int sock, char* buffer) {
    int n = send(sock, buffer, BUFF, 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error writing to client...");
    }
}

//	Sends the string struct to the client
void sendStringsToClient(int sock, struct strings* str) {
    int n = send(sock, str, sizeof(*str), 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error writing to client...");
    }
}

//	Receives a string struct from the client
void receiveFromClient(int sock, struct strings *str) {
    int n = recv(sock, str, sizeof(*str), 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error reading from client...");
    }
}

//	Prints an error message and terminates the program with a code
void error(const char *msg) {
    perror(msg);
    exit(1);
}

//	Initializes the Windows Sockets API.
//	Needed to ensure that Windows Sockets are
//	supported by the system
void initWinSock() {
    #if defined WIN32
    WSADATA wsaData;
	
	//	Specifies the requested Windows Socket version
	//	and obtains the necessary implementation details
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
        error("Error at WSAStartup...\n");
    #endif // defined
}

//	Calls WSACleanup to close the socket
void clearWinSock() {
    #if defined WIN32
    WSACleanup();
    #endif // defined
}
