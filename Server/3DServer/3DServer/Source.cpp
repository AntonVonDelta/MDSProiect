#define WIN32_LEAN_AND_MEAN // Stop the compiler from using "extra" definitions from windows.h which already includes some version of winsock

#include <iostream>
#include <fstream>
#include <vector>
#include <Windows.h>
#include "Sockets.h"
#include "Grafica.h"
#include "Http.h"

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
	Http http;
	if (client.talksHTTP) return true;

	try {
		HttpContext* http_context = http.readHeader(client);
		client.talksHTTP=true;
		string opt = http_context->get_param("page");
		if (opt == "/api/login")
		{
			http_context->init_Grafica();
			string set_value = http.gen_RandomId(20);
			cout << set_value;
			http.sendResponse(*http_context, 200, "dasduasdasdadjhjdasdasd", {});
		}
		if (opt == "/api/load")
		{
			int lenght = 0;
			if (http_context->get_param("content-length") != "")
				lenght = stoi(http_context->get_param("content-length"));
			string body = http.readBody(client, lenght);
			try {
				http_context->get_Grafica().loadObject(body);
				http.sendResponse(*http_context, 200, "dasduasdasdadjhjdasdasd", {});
			}
			catch (runtime_error e) {
				http.sendResponse(*http_context, 422, e.what(), {});
			}
			closeSelectedClient(client);
		}
	}
	catch (string s)
	{
		printf(s.c_str());
	}
	return true;
}
