# Dummy-TCP-IP-client-server
C implementation of a dummy client-server system. The server accepts & establishes connections from the client, which prompts
the user to enter two strings in input. The client sends the strings to the server, converts the first one to uppercase, the second
one to lowercase and sends them back to the client, which displays the results and terminates.

<b>Building & Running</b>

Download & import each file into a dedicated project in the IDE of your choice. Depending on your settings, you may have to notify
your compiler to link the <i>ws2_32</i> library to the projects. Build all, run the server, then run the client.

Alternatively, use the command line to navigate to wherever the files are downloaded and enter the command 
<i>gcc server.c -o server -lws2_32</i> run the executable. Repeat this process in a seperate command line for the client.
To build from the command line your C compiler must be listed in your system's PATH Environment Variable.
