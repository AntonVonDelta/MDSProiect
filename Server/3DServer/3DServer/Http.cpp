#include "Http.h"

HttpContext::~HttpContext()
{
	if (data != NULL) {
		data->destroy();
		delete data;
	}	
}

void HttpContext::init_Grafica() {
	data = new Grafica;
	data->init();
}
Grafica* HttpContext::get_Grafica() {
	return data;
}
void HttpContext::set_request_params(map<string, string> rp) {
	request_params = rp;
}
string HttpContext::get_param(string s) {
	return request_params[s];
}

void HttpContext::setClient(CLIENT_STRUCTURE client) {
	Client = client;
}

HttpContext Http::readHeader(CLIENT_STRUCTURE &client_struct)
{
	HttpContext http_context;
	http_context.setClient(client_struct);
	char* buff=new char[1000];
	char *initial_buff = buff;
	string line;
	int len=0;
	char old_char;
	char new_char;
	bool new_line=false;
	bool parser_result = recvAll(client_struct,buff,1);
	new_char = *buff;
	while (parser_result)
	{
		len += 1;
		old_char = new_char;
		new_char = *buff;
		if (new_line && new_char != '\r')
			new_line = false;

		if (old_char == '\r' && new_char == '\n' && !new_line)
		{	
			new_line = true;
		}
		if (old_char == '\n' && new_char == '\r' && new_line)
		{
			parser_result = recvAll(client_struct, buff + 1, 1);
			++buff;
			break;
		}

		parser_result = recvAll(client_struct, buff+1, 1);
		++buff;
	}
	if (!parser_result)
		throw string("Disconnected!");

	*buff = 0;
	string str(initial_buff);
	printf("%s\n",str.c_str());
	int poz1 = str.find(' ');
	int poz_opt = str.find('?');
	int poz2 = str.substr(poz1+1).find(' ');
	string request_query;
	if(poz_opt!=-1 && poz_opt<poz2)
		request_query = str.substr(poz1 + 1 , poz_opt - poz1 -1);
	else
		request_query = str.substr(poz1 + 1, poz2);
	string opt = str.substr(poz_opt + 1, (poz2-poz_opt+3));
	map<string, string> new_request_params;
	string key = "page";
	string value = request_query;
	new_request_params[key] = value;
	if (str.find("/load") != -1)
	{

	}
	if (poz_opt != -1)
	{
		// /move?direction={direction}&amount={amount}
		// /rotate?direction={direction}&amount={amount}

		int poz_and = opt.find('&');
		while (poz_and != -1)
		{
			int poz_equal = opt.find('=');
			int poz_right = opt.find('{');
			int poz_left = opt.find('}');
			string actual_opt = opt.substr(0, poz_equal);
			string actual_amount = opt.substr(poz_right+1, poz_left - poz_right - 1);
			new_request_params[actual_opt] = actual_amount;
			opt = opt.substr(poz_and+1);
			poz_and = opt.find('&');
		}
		int poz_equal = opt.find('=');
		int poz_right = opt.find('{');
		int poz_left = opt.find('}');
		string actual_opt = opt.substr(0, poz_equal);
		string actual_amount = opt.substr(poz_right + 1, poz_left - poz_right - 1);
		new_request_params[actual_opt] = actual_amount;
	}
	str = str.substr(str.find("\r\n") + 2);

	while (str.size() > 2)
	{
		poz1 = str.find(':');
		poz2 = str.find("\r\n");
		string key = str.substr(0, poz1);
		string value = str.substr(poz1+1, poz2 - poz1-1);
		transform(key.begin(), key.end(), key.begin(), ::tolower);
		new_request_params[key]=value;
		str = str.substr(poz2 + 2);
	}
	http_context.set_request_params(new_request_params);

	printf("%s\n", http_context.get_param("dnt").c_str());

	delete[] initial_buff;
	return http_context;
}

string Http::readBody(CLIENT_STRUCTURE& client_struct, int lenth)
{
	if (lenth == 0) return string();

	char* buff = new char[lenth+1];
	bool parser_result = recvAll(client_struct, buff, lenth);
	if (!parser_result)
		throw "Disconnected!";

	buff[lenth] = 0;
	string str(buff);
	delete[] buff;

	return str;
}

void Http::sendResponse(HttpContext& http_context,int responseCode,string body, map<string, string> fields)
{
	char buffer[1000];
	int n, a = 5, b = 3;
	int response;
	map<int, string> codes = {
		{200,"200 OK"},
		{404,"404 Not Found"},
		{409,"409 Conflict"},
		{500,"500 Internal Server Error"}
	};
	int content_length = body.length();
	if(body=="" || content_length == 0)
		response = sprintf_s(buffer,1000, "HTTP/1.1 %s\r\n", codes[responseCode].c_str());
	else
	{
		response = sprintf_s(buffer,1000, "HTTP/1.1 %s\r\nContent-Type: text/html;\r\nServer: Apache\r\nConnection: Close\r\nContent-Lenght: %d\r\n\r\n", codes[responseCode].c_str(),content_length);
	}
	int len = strlen(buffer);
	sendAll(http_context.getClient(), buffer, len);
	char* cstr = new char[200];
	strcpy_s(cstr,200, body.c_str());
	sendAll(http_context.getClient(), cstr, body.length());
}

CLIENT_STRUCTURE& HttpContext::getClient() {
	return Client;
}