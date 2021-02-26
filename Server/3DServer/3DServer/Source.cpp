#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include "Definitions.h"
#include "Grafica.h"

using namespace std;

Grafica render;
SOCKET listening_socket;
int server_port = 5050;
fd_set master, read_fds;
vector<CLIENT_STRUCTURE> pendingClients;


int main() {
	SOCKADDR_IN server_config;

	if (!render.Init()) {
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


	closesocket(listening_socket);
	WSACleanup();
	render.Destroy();
	return 0;
}

void ProcessConnections() {
	unsigned int fdmax = listening_socket;
	struct sockaddr_in clientinfo;
	char strAddr[INET6_ADDRSTRLEN];
	int selRes;
	SOCKET client;

	timeval timeout = { 0 , 500000 };
	time_t  currentTime, lastPingTime;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	FD_SET(listening_socket, &master);

	while (true) {
		read_fds = master;

		// TODO: Check here if already connected sockets are active or inactive

		// Get new connections - check if server socket contains "connections" to be read
		if ((selRes = select(fdmax + 1, &read_fds, NULL, NULL, &timeout)) <=0 ) continue;

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
						protoClient._sockid = client;
						strcpy(protoClient.address, InetNtopA(clientinfo.sin_family, &(clientinfo.sin_addr), strAddr, INET6_ADDRSTRLEN));

						// Set timeout for recv command - or else it will block
						DWORD maxWaitTime = 5000;
						setsockopt(protoClient._sockid, SOL_SOCKET, SO_RCVTIMEO, (char*)&maxWaitTime, sizeof(maxWaitTime));

						// Measure the time the socket connected
						time(&protoClient.timeWhenConnected);

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

void parser(CLIENT_STRUCTURE &client) {
	// Http parser goes here
}

// Remove client/socket
void closeSelectedClient(CLIENT_STRUCTURE& client) {
	closesocket(client._sockid);

	FD_CLR(client._sockid, &master);
	for (j = pendingClients.size() - 1; j >= 0 && (pendingClients[j]._sockid != client._sockid); j--);
	pendingClients.erase(pendingClients.begin() + j);
}