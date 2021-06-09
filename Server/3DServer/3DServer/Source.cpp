#define WIN32_LEAN_AND_MEAN // Stop the compiler from using "extra" definitions from windows.h which already includes some version of winsock

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
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

map<string, HttpContext*> openConnections;

int main() {
	return server_start();
}

void parser(CLIENT_STRUCTURE& client) {
	// Http parser goes here - this is called when the socket has data on the buffer to be read
	Http http;
	if (client.talksHTTP) return;
	HttpContext* http_context = nullptr;
	try {
		http_context = http.readHeader(client);
		client.talksHTTP=true;
		string opt = http_context->get_param("page");
		if (opt == "/api/login")
		{
			try {
			http_context->init_Grafica();
			string set_value = http.gen_RandomId(20);
			if (openConnections.find(set_value) != openConnections.end())
				http.sendResponse(*http_context, 409, "", {});
			openConnections[set_value] = http_context;
			cout << set_value;
			http.sendResponse(*http_context, 200, "", { {string("Set-Cookie"), string("session=") + string(set_value)},
														{string("Transfer-Encoding"), string("chunked")},
														{string("Content-Type"), string("application/octet-stream")} });
			return;
			}
			catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
		if (opt == "/api/load")
		{
			int lenght = 0;
			if (http_context->get_param("content-length") != "")
				lenght = stoi(http_context->get_param("content-length"));
			string body = http.readBody(client, lenght);
			try{
				string cookie = http_context->get_param("cookie");
				string id = cookie.substr(cookie.find('='));
				id.erase(0, 1);
				HttpContext *video_feed = openConnections.at(id);
				video_feed->get_Grafica().loadObject(body);
				http.sendResponse(*http_context, 200, "",{});
			}
			catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			}
			catch (runtime_error e) {
				http.sendResponse(*http_context, 422, e.what(), {});
			}
			catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
		if (opt == "/api/close")
		{
			try {
				string cookie = http_context->get_param("cookie");
				string id = cookie.substr(cookie.find('='));
				id.erase(0, 1);
				HttpContext* video_feed = openConnections.at(id);
				http.sendChunk(*video_feed, "", 0);
				delete video_feed;
				openConnections.erase(id);
				http.sendResponse(*http_context, 200, "", {});
			}
			catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			}
			catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
	}
	catch (string s)
	{
		printf(s.c_str());
	}
	if (http_context != nullptr)
		delete http_context;
	else
		closeSelectedClient(client);
	return;
}
