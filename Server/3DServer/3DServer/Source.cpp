#define WIN32_LEAN_AND_MEAN // Stop the compiler from using "extra" definitions from windows.h which already includes some version of winsock

#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include "Sockets.h"
#include "Grafica.h"

using namespace std;


// We can't write functional code in this cpp file
// The reason is this: we need the obj files of all the cpp's to be used in unit testing. But this cpp file will be compiled to .obj file which
// will of course contain the definition and declaration of 'main' function. 
// But guess what: the unit testing project already has a 'main' function. By linking the .obj file there will be a conflict
// Thus we separate all the logic from this file and write it into different 'cpp' files


int main() {
	return server_start();
}

bool parser(CLIENT_STRUCTURE& client) {
	// Http parser goes here - this is called when the socket has data on the buffer to be read

	return true;
}
