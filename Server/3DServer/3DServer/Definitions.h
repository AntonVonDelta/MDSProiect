#pragma once

#include <Windows.h>
#include <WinSock2.h>
#include <ws2tcpip.h>

// Store data about the http client - perhaps login info etc.
//struct CLIENT_DESCRIPTOR {
//
//};
struct CLIENT_STRUCTURE {									// define the structure for storing the details regarding the clients
	//CLIENT_DESCRIPTOR info;								// define struct for client info
	SOCKET _sockid;											// store the id from master buffer
	time_t timeWhenConnected;								// store time of connection
	char address[INET6_ADDRSTRLEN];							// store the ip address of client
};