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
time_t last_processed;

int main() {
	return server_start();
}

// Called in a loop before processing incoming packets
void worker() {
	Http http;
	time_t current_time;
	time(&current_time);

	if (current_time - last_processed > 2 && a) {
		last_processed = current_time;

		for (auto it = openConnections.cbegin(); it != openConnections.cend();) {
			HttpContext* connection = it->second;

			try {
				Grafica* grafica = connection->get_Grafica();
				http.sendChunk(*connection, grafica->getBuffer(), grafica->getBufferSize());
			} catch (...) {
				delete connection;
				openConnections.erase(it++);
				continue;
			}

			++it;
		}
	}
}

void parser(CLIENT_STRUCTURE& client) {
	// Http parser goes here - this is called when the socket has data on the buffer to be read
	Http http;
	HttpContext* http_context = nullptr;

	if (client.talksHTTP) {
		return;
	}

	try {
		http_context = http.readHeader(client);
		string opt = http_context->get_param("page");

		client.talksHTTP = true;

		if (opt == "/api/login") 		{
			try {
				string generated_cookie = http.gen_RandomId(20);
				http_context->init_Grafica();
				http_context->get_Grafica()->nextScene();

				if (openConnections.find(generated_cookie) != openConnections.end()) {
					http.sendResponse(*http_context, 409, "", {});
				} else {
					openConnections[generated_cookie] = http_context;
					http.sendResponse(*http_context, 200, "", {
						{string("Set-Cookie"), string("session=") + generated_cookie},
						{string("Transfer-Encoding"), string("chunked")},
						{string("Content-Type"), string("application/octet-stream")}
						});
					return;
				}
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
		if (opt == "/api/load") 		{
			int lenght = 0;
			if (http_context->get_param("content-length") != "") {
				lenght = stoi(http_context->get_param("content-length"));
				string body = http.readBody(client, lenght);

				try {
					string cookie = http_context->get_param("cookie");
					string id = cookie.substr(cookie.find('='));
					id.erase(0, 1);

					HttpContext* video_feed = openConnections.at(id);
					video_feed->get_Grafica()->loadObject(body);
					video_feed->get_Grafica()->nextScene();

					http.sendResponse(*http_context, 200, "", {});
				} catch (out_of_range) {
					http.sendResponse(*http_context, 404, "", {});
				} catch (runtime_error e) {
					http.sendResponse(*http_context, 422, e.what(), {});
				} catch (exception e) {
					http.sendResponse(*http_context, 500, e.what(), {});
				}
			}
		}
		if (opt == "/api/move") {
			try {
				string cookie = http_context->get_param("cookie");
				int direction= stoi(http_context->get_param("direction"));
				float amount = stof(http_context->get_param("amount"));
				string id = cookie.substr(cookie.find('='));
				id.erase(0, 1);

				HttpContext* video_feed = openConnections.at(id);
				video_feed->get_Grafica()->setMoveAmount(amount);
				video_feed->get_Grafica()->moveScene(direction);

				http.sendResponse(*http_context, 200, "", {});
			} catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
		if (opt == "/api/rotate") {
			try {
				string cookie = http_context->get_param("cookie");
				int direction = stoi(http_context->get_param("direction"));
				float amount = stof(http_context->get_param("amount"));
				string id = cookie.substr(cookie.find('='));
				id.erase(0, 1);

				HttpContext* video_feed = openConnections.at(id);
				video_feed->get_Grafica()->setRotateAmount(amount);
				video_feed->get_Grafica()->rotateScene(direction);

				http.sendResponse(*http_context, 200, "", {});
			} catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
		if (opt == "/api/close") 		{
			try {
				string cookie = http_context->get_param("cookie");
				string id = cookie.substr(cookie.find('='));
				id.erase(0, 1);

				HttpContext* video_feed = openConnections.at(id);
				http.sendChunk(*video_feed, "", 0);

				delete video_feed;
				openConnections.erase(id);

				http.sendResponse(*http_context, 200, "", {});
			}catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			}catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}
	} catch (string s) 	{
		cout<<s<<endl;
	}

	if (http_context != nullptr) delete http_context;
	else closeSelectedClient(client);
}
