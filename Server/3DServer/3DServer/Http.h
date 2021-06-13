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
	string SessionId;
	Grafica *data=nullptr; // This is used to store Grafica object. This is created on heap so must be destroyed on class destructor
	time_t lastActivity;
public:
	~HttpContext();
	HttpContext() {
		time(&lastActivity);
	}

	HttpContext(const HttpContext & other) {
		throw string("Context cannot be copied!");
	}

	void initGrafica();
	void setRequestParams(map<string, string> rp);
	void setClient(CLIENT_STRUCTURE);
	void setSessionId(string);
	void setActivity();

	time_t getInactivity();
	string getSessionId();
	Grafica* getGrafica();
	string getParam(string);
	CLIENT_STRUCTURE& getClient();
};

class Http
{
private:
	static const int HEADER_BUFF_SIZE = 5000;

	static map<int, string> createHttpCodes() {
		map<int, string> codes = {
			{200,"200 OK"},
			{404,"404 Not Found"},
			{409,"409 Conflict"},
			{422,"422 Unprocessable failure"},
			{500,"500 Internal Server Error"}
		};
		return codes;
	}
	static map<string, string> createTypeCodes() {
		map<string, string> codes = {
			{"js","text/javascript"},
			{"html","text/html"},
			{"css"," text/css "},
			{"png","image/png"},
			{"jpg","image/jpeg"},
			{"ico","image/jpeg"}
		};
		return codes;
	}

	static string eraseStartSpaces(string);
public:

	static const map<int, string> HTTP_CODES;
	static const map<string, string> TYPE_CODES;

	static string getContentType(string extension);
	static string genRandomId(const int);
	static HttpContext* readHeader(CLIENT_STRUCTURE&);
	static string readBody(CLIENT_STRUCTURE&, int);
	static void sendResponse(HttpContext&,int,string,map<string,string>);
	static void sendChunk(HttpContext&,const char*,int);
};
