/*
 *	TCP/IP Client
 *
 *	When started it attempts to contact the server. 
 *	After a connection is successfully established
 *	it receives a message from the server notifying
 *	of the connection and the user is prompted to enter
 *	the specified data. The data is sent to the server,
 *	reelaborated, sent back to the client and displayed
 *	on screen. The connection is closed and exectution terminated.
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
#include <netdb.h>

#endif // defined

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8080
#define STRL 200
#define BUFF 500



//	The data to be sent to the server contains two strings
struct strings {
    char str1[STRL];
    char str2[STRL];
};



//============================== Function prototypes ==============================//

void receiveFromServer(int, char*);
void receiveStringsFromServer(int, struct strings*);
void sendStrings(int, struct strings*);
void error(const char*);
void initWinSock();
void clearWinSock();

//=================================================================================//

int main(int argc, char* argv[]) {
    char buffer[BUFF];
    int nSockFd;
    struct strings sOriginalString, sNewString;	//	Stores the data entered by the user and received from the server respectively
    struct sockaddr_in serverAddr;
    struct hostent *server;

    initWinSock();
	
    server = gethostbyname("localhost");
	
	//	Initializes a socket using the Internet Protocol v4
	//	Family, standard TCP socket type & default settings
    nSockFd = socket(AF_INET, SOCK_STREAM, 0);

    if (nSockFd < 0)
        error("Error opening socket...");

	//	Initializes the server address
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

	//	Requests a connection to the server, closes the socket and terminates execution if anything goes wrong.
    if (connect(nSockFd, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
        error("Error connecting to server...");
        closesocket(nSockFd);
        clearWinSock();

        return 0;
    }


	//	Connection successful:
    printf("%s: Connceted\nPort: %d\n", inet_ntoa(serverAddr.sin_addr), ntohs(serverAddr.sin_port));

    system("PAUSE");
    receiveFromServer(nSockFd, buffer);
    printf("%s\n", buffer);

	//	Promts the user to enter the data
    printf("Insert first string: ");
    scanf("%s", sOriginalString.str1);
    printf("Insert second string: ");
    scanf("%s", sOriginalString.str2);
	
	//	Sends the unformatted data 
    sendStrings(nSockFd, &sOriginalString);
    receiveStringsFromServer(nSockFd, &sNewString);

    printf("\nFirst string: %s\n", sNewString.str1);
    printf("Second string: %s\n", sNewString.str2);
    system("PAUSE");
	
	//	Closes the socket and terminates exectution.
    closesocket(nSockFd);
    clearWinSock();

    return 0;
}



//============================== Function definitions ==============================//

//	Receives messages from the server
void receiveFromServer(int sock, char* buffer) {
    int n = recv(sock, buffer, BUFF, 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error reading from socket...");
    }
}

//	Receives the string struct from the server
void receiveStringsFromServer(int sock, struct strings* str) {
    int n = recv(sock, str, BUFF, 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error reading from server...");
    }
}

//	Sends the data entered by the user to the server
void sendStrings(int sock, struct strings* str) {
    int n = send(sock, str, BUFF, 0);

    if (n < 0) {
        closesocket(sock);
        clearWinSock();
        error("Error sending to server...");
    }
}


//	Prints an error message and terminates the program
void error(const char* msg) {
    perror(msg);
    exit(1);
}

//	Initializes the Windows Sockets API
void initWinSock() {
    #if defined WIN32
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (res)
        error("Error at WSAStartup...");
    #endif // defined
}

//	Calls WSACleanup to close the socket
void clearWinSock() {
    #if defined WIN32
    WSACleanup();
    #endif // defined
}
