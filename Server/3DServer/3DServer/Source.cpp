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
FILETIME last_processed;
int OPENGL_WIDTH = 300;
int OPENGL_HEIGHT = 300;

int main(int argc,char* argv[]) {
	if (argc >= 2) {
		string port_str = argv[1];
		server_port = stoi(port_str);
	}
	if (argc >= 4) {
		string width_str = argv[2];
		string height_str = argv[3];
		OPENGL_WIDTH = stoi(width_str);
		OPENGL_HEIGHT =stoi(height_str);
	}

	return server_start();
}

// Called in a loop before processing incoming packets
void worker() {
	Http http;

	for (auto it = openConnections.cbegin(); it != openConnections.cend();) {
		HttpContext* connection = it->second;

		if (connection->getInactivity() > 60) {
			// Finish chunk
			http.sendChunk(*connection, "", 0);

			delete connection;
			openConnections.erase(it++);
			continue;
		}

		try {
			Grafica* grafica = connection->getGrafica();
			grafica->nextScene();
			http.sendChunk(*connection, grafica->getBuffer(), grafica->getBufferSize());
		} catch (...) {
			delete connection;
			openConnections.erase(it++);
			continue;
		}

		++it;
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
		string opt = http_context->getParam("page");

		client.talksHTTP = true;

		if (opt == "/api/login") {
			try {
				string generated_cookie = http.genRandomId(20);
				http_context->initGrafica(OPENGL_WIDTH,OPENGL_HEIGHT);
				http_context->getGrafica()->nextScene();

				if (openConnections.find(generated_cookie) != openConnections.end()) {
					http.sendResponse(*http_context, 409, "", {});
				} else {
					openConnections[generated_cookie] = http_context;
					cout << generated_cookie << endl;
					http.sendResponse(*http_context, 200, "", {
						{string("X-Authorize"), generated_cookie},
						{string("Transfer-Encoding"), string("chunked")},
						{string("Content-Type"), string("application/octet-stream")},
						{string("X-Image-Width"), to_string(http_context->getGrafica()->getWidth())},
						{string("X-Image-Height"), to_string(http_context->getGrafica()->getHeight())}
						});
					return;
				}
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}else if (opt == "/api/load") {
			int lenght = 0;
			if (http_context->getParam("content-length") != "") {
				lenght = stoi(http_context->getParam("content-length"));
				string body = http.readBody(client, lenght);

				try {
					string cookie = http_context->getParam("x-authorize");
					cout << cookie << endl;

					HttpContext* video_feed = openConnections.at(cookie);
					video_feed->getGrafica()->loadObject(body);
					video_feed->getGrafica()->nextScene();
					video_feed->setActivity();

					http.sendResponse(*http_context, 200, "", {});
				} catch (out_of_range) {
					http.sendResponse(*http_context, 404, "", {});
				} catch (runtime_error e) {
					http.sendResponse(*http_context, 422, e.what(), {});
				} catch (exception e) {
					http.sendResponse(*http_context, 500, e.what(), {});
				}
			}
		} else if (opt == "/api/move") {
			try {
				string cookie = http_context->getParam("x-authorize");
				int direction = stoi(http_context->getParam("direction"));
				float amount = stof(http_context->getParam("amount"));

				HttpContext* video_feed = openConnections.at(cookie);
				video_feed->getGrafica()->moveScene(direction, amount);
				video_feed->setActivity();

				http.sendResponse(*http_context, 200, "", {});
			} catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		} else if (opt == "/api/rotate") {
			try {
				string cookie = http_context->getParam("x-authorize");
				int direction = stoi(http_context->getParam("direction"));
				float amount = stof(http_context->getParam("amount"));

				HttpContext* video_feed = openConnections.at(cookie);
				video_feed->getGrafica()->rotateScene(direction, amount);
				video_feed->setActivity();

				http.sendResponse(*http_context, 200, "", {});
			} catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		}else if (opt == "/api/close") {
			try {
				string cookie = http_context->getParam("x-authorize");
				HttpContext* video_feed = openConnections.at(cookie);
				http.sendChunk(*video_feed, "", 0);

				delete video_feed;
				openConnections.erase(cookie);

				http.sendResponse(*http_context, 200, "", {});
			} catch (out_of_range) {
				http.sendResponse(*http_context, 404, "", {});
			} catch (exception e) {
				http.sendResponse(*http_context, 500, e.what(), {});
			}
		} else {
			// Static file management
			try {
				if (opt.find("..") != string::npos) {
					throw string("Path traversal attack!");
				}
				if (opt == "/") opt = "/index.html";
				if (opt.find(".") == string::npos || opt.size()-1-opt.find(".")==0) {
					throw string("No extension defined!");
				}

				string path = "www";
				string extension = opt.substr(opt.find("."));
				extension.erase(0, 1);
				path += opt;

				ifstream fi(path, std::ios::binary);
				std::stringstream buffer;
				if (!fi.is_open()) {
					throw string("Resursa inexistenta!");
				}

				buffer << fi.rdbuf();
				http.sendResponse(*http_context, 200, buffer.str(), { {"Content-Type",Http::getContentType(extension)} });
			} catch (exception e) {
				http.sendResponse(*http_context, 404, "", {});
			}
			
		}
	} catch (string s) {
		cout << s << endl;
	}

	if (http_context != nullptr) delete http_context;
	else closeSelectedClient(client);
}
