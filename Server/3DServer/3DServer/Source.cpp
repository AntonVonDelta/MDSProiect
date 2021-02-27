#define WIN32_LEAN_AND_MEAN // Stop the compiler from using "extra" definitions from windows.h which already includes some version of winsock

#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <WinSock2.h>
#include<WS2tcpip.h>
#include "Definitions.h"
#include "Grafica.h"

using namespace std;

#define HTTP_HEADER_MAX_SIZE 800

Grafica render;
SOCKET listening_socket;
int server_port = 5050;
fd_set master, read_fds;
vector<CLIENT_STRUCTURE> pendingClients;

void processConnections();
bool parser(CLIENT_STRUCTURE&);
bool recvAll(CLIENT_STRUCTURE& client, char* buffer, unsigned int len);
bool sendAll(CLIENT_STRUCTURE& client, char* buffer, unsigned int len);
bool isClientValid(CLIENT_STRUCTURE& client);
void closeSelectedClient(CLIENT_STRUCTURE& client);


int main() {
	SOCKADDR_IN server_config;

	if (!render.init()) {
		cout << "OpenGL Init error!";
		return 0;
	}
	printf("OpenGL initialized succesfully!");

	// Init sockets
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData)) {
		printf( "WSAStartup failed! Could not initialize sockets!");
		return 0;
	}

	// Create the listening server socket
	listening_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listening_socket == INVALID_SOCKET) {
		printf( "Error opening the socket!");
		return 0;
	}

	// Bind to the listening address
	server_config.sin_family = AF_INET;
	server_config.sin_port = htons(server_port);
	server_config.sin_addr.s_addr = htonl(INADDR_ANY);
	memset(server_config.sin_zero, 0, sizeof(server_config.sin_zero));


	if (bind(listening_socket, (SOCKADDR*)&server_config, sizeof(server_config)) == SOCKET_ERROR) {
		printf("Error in binding socket to port");
		closesocket(listening_socket);
		WSACleanup();	// We better cleanup ourselves: it's possible the port will get locked next time we run (cause timeout)
		return 0;
	}

	if (listen(listening_socket, SOMAXCONN) == SOCKET_ERROR) {
		printf("Error in listening!");
		closesocket(listening_socket);
		WSACleanup();
		return 0;
	}

	processConnections();

	closesocket(listening_socket);
	WSACleanup();
	render.destroy();
	return 0;
}

void processConnections() {
	unsigned int fdmax = listening_socket;
	struct sockaddr_in clientinfo;
	char strAddr[INET6_ADDRSTRLEN];
	int selRes;
	time_t current_time;
	SOCKET client;

	timeval timeout = { 0 , 500000 };

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(listening_socket, &master);

	while (true) {
		read_fds = master;

		// Check client validity after 5 sec. timeout
		time(&current_time);
		FOR_EVERY_CLIENT(z) {
			if (pendingClients[z].timeWhenConnected != -1 && current_time - pendingClients[z].timeWhenConnected > 2) {
				pendingClients[z].timeWhenConnected = -1;	// already checked time
				if(!isClientValid(pendingClients[z])) closeSelectedClient(pendingClients[z]);
			}
		}

		// Get new connections - check if server socket contains "connections" to be read
		if ((selRes = select(fdmax + 1, &read_fds, NULL, NULL, &timeout)) <1 ) continue;

		for (unsigned int i = 0; i <= fdmax && selRes > 0; i++) {
			if (FD_ISSET(i, &read_fds)) {
				if (i == listening_socket) {
					int sz = sizeof(clientinfo);
					client = accept(listening_socket, (struct sockaddr*)&clientinfo, &sz);

					if (client == SOCKET_ERROR)	printf("One accept() failed !");
					else {
						CLIENT_STRUCTURE protoClient;

						FD_SET(client, &master);		//Quite ingineous: just think what would happen if client>fdmax->the 'for' would also include this new added client...this doesn't happen BECAUSE the client is added to 'master' not read_fds so FD_ISSET above fails...
						if (client > fdmax) fdmax = client;

						memset(&protoClient, 0, sizeof(protoClient));
						protoClient.talksHTTP = false;
						protoClient._sockid = client;
						strcpy_s(protoClient.address, INET6_ADDRSTRLEN,InetNtopA(clientinfo.sin_family, &(clientinfo.sin_addr), strAddr, INET6_ADDRSTRLEN));

						// Measure the time the socket connected
						time(&protoClient.timeWhenConnected);

						// Set timeout for recv command - or else it will block
						DWORD maxWaitTime = 5000;
						setsockopt(protoClient._sockid, SOL_SOCKET, SO_RCVTIMEO, (char*)&maxWaitTime, sizeof(maxWaitTime));

						// Add to our clients
						pendingClients.push_back(protoClient);
					}
				} else {
					// Our non-server socket has something on the queue
					int j = -1;
					for (j = pendingClients.size() - 1; j >= 0 && (pendingClients[j]._sockid != i); j--);
					parser(pendingClients[j]);
				}
			}
		}
	}
}

bool parser(CLIENT_STRUCTURE &client) {
	// Http parser goes here - this is called when the socket has data on the buffer to be read

	char* buff = new char[HTTP_HEADER_MAX_SIZE];
	bool parser_result = false;

	if (!(parser_result = recvAll(client, buff, 200))) {
		closeSelectedClient(client);
		return false;
	}

	buff[200 - 1] = 0;
	printf("Received: %s", buff);

	delete[] buff;
}

// Block until all bytes are received
bool recvAll(CLIENT_STRUCTURE& client, char* buffer, unsigned int len) {
	int recvlen = 0;
	unsigned int readSize = 0;

	while (readSize < len) {
		if ((recvlen = recv(client._sockid, buffer + readSize, len - readSize, 0)) < 1) {
			return false;
		}
		readSize += recvlen;
	}
	return true;
}

// Block till all bytes are sent
bool sendAll(CLIENT_STRUCTURE& client, char* buffer, unsigned int len) {
	int sentlen;
	unsigned int sentSize = 0;

	while (sentSize < len) {
		if ((sentlen = send(client._sockid, buffer + sentSize, len - sentSize, 0)) < 0) {
			return false;
		}
		sentSize += sentlen;
	}
	return true;
}

// Determines if the client should be kept or not
bool isClientValid(CLIENT_STRUCTURE& client) {
	return client.talksHTTP;
}

// Remove client/socket
void closeSelectedClient(CLIENT_STRUCTURE& client) {
	int j;

	closesocket(client._sockid);

	FD_CLR(client._sockid, &master);
	for (j = pendingClients.size() - 1; j >= 0 && (pendingClients[j]._sockid != client._sockid); j--);
	pendingClients.erase(pendingClients.begin() + j);
}