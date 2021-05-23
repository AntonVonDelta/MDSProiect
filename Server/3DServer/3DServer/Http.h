#pragma once

#define WIN32_LEAN_AND_MEAN // Stop the compiler from using "extra" definitions from windows.h which already includes some version of winsock

#include <iostream>
#include <fstream>
#include <Windows.h>
#include "Sockets.h"
#include "Grafica.h"
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <locale>

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "GL/freeglut.h"
#include "Sockets.h"

using namespace std;

class HttpContext
{
private:
	map<string, string> request_params;
	CLIENT_STRUCTURE Client;
	Grafica* data; // This is used to store Grafica object. This is created on heap so must be destroyed on class destructor
public:
	~HttpContext();
	HttpContext() =default;
	string getSessionId();
	void init_Grafica();
	Grafica* get_Grafica();
	void set_request_params(map<string, string> rp);
	string get_param(string);
	CLIENT_STRUCTURE& getClient();
	void setClient(CLIENT_STRUCTURE);
};

class Http
{
public:
	static HttpContext readHeader(CLIENT_STRUCTURE&);
	static string readBody(CLIENT_STRUCTURE&, int);
	static void sendResponse(HttpContext&,int,string,map<string,string>);
	static void sendChunk(HttpContext&);
};

