#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include<WS2tcpip.h>

#define FOR_EVERY_CLIENT(var) if(pendingClients.size()) for (int var = pendingClients.size()-1;var>=0;var--)

// Store data about the http client - perhaps login info etc.
//struct CLIENT_DESCRIPTOR {
//
//};
struct CLIENT_STRUCTURE {									// define the structure for storing the details regarding the clients
	//CLIENT_DESCRIPTOR info;								// define struct for client info
	SOCKET _sockid;											// store the id from master buffer
	time_t timeWhenConnected;								// store time of connection
	bool talksHTTP;											// Set to true if the client does talk http protocol - reset every time new data is received
	char address[INET6_ADDRSTRLEN];							// store the ip address of client
};