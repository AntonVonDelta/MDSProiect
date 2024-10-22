#include "Http.h"

HttpContext::~HttpContext() {
	closeSelectedClient(Client);
	if (data != nullptr) {
		data->destroy();
		delete data;
	}
}

void HttpContext::initGrafica(int width, int height) {
	data = new Grafica(width, height);
	if (!data->init()) throw string("Could not init the graphics!");
}
Grafica* HttpContext::getGrafica() {
	return data;
}
void HttpContext::setRequestParams(map<string, string> rp) {
	request_params = rp;
}
string HttpContext::getParam(string s) {
	return request_params[s];
}

void HttpContext::setClient(CLIENT_STRUCTURE client) {
	Client = client;
}
void HttpContext::setActivity() {
	time(&lastActivity);
}

time_t HttpContext::getInactivity() {
	time_t currentTime;
	time(&currentTime);
	return currentTime - lastActivity;
}
string HttpContext::getSessionId() {
	return SessionId;
}

CLIENT_STRUCTURE& HttpContext::getClient() {
	return Client;
}

void HttpContext::setSessionId(string sessionId) {
	SessionId = sessionId;
}



string Http::getContentType(string extension) {
	return Http::TYPE_CODES.at(extension);
}

HttpContext* Http::readHeader(CLIENT_STRUCTURE& client_struct) {
	HttpContext* http_context = new HttpContext();
	char* buff = new char[HEADER_BUFF_SIZE];
	char* initial_buff = buff;
	int read_bytes = 0;
	string line;
	char old_char;
	char new_char;
	bool new_line = false;
	bool parser_result;

	parser_result = recvAll(client_struct, buff, 1);
	read_bytes = 1;
	new_char = *buff;
	while (parser_result) {
		if (read_bytes >= HEADER_BUFF_SIZE - 1) throw string("Buffer overflow: header too big!");

		old_char = new_char;
		new_char = *buff;
		if (new_line && new_char != '\r')
			new_line = false;

		if (old_char == '\r' && new_char == '\n' && !new_line) {
			new_line = true;
		}
		if (old_char == '\n' && new_char == '\r' && new_line) {
			parser_result = recvAll(client_struct, buff + 1, 1);
			++buff;
			break;
		}

		parser_result = recvAll(client_struct, buff + 1, 1);
		read_bytes++;
		buff++;
	}
	if (!parser_result)
		throw string("Disconnected!");

	// Create the header string
	*buff = 0;
	string str(initial_buff);

	// Get the request url
	string request_page;
	map<string, string> request_params;
	int poz1 = str.find(' ');
	int poz2 = str.find(' ', poz1 + 1);
	int poz_opt = str.find('?');

	if (poz_opt != string::npos && poz_opt < poz2)
		request_page = str.substr(poz1 + 1, poz_opt - poz1 - 1);
	else
		request_page = str.substr(poz1 + 1, poz2 - poz1 - 1);
	request_params["page"] = request_page;

	// Get the requested parameters
	if (poz_opt != string::npos) {
		string opt = str.substr(poz_opt, (poz2 - poz_opt));

		// /move?direction={direction}&amount={amount}
		// /rotate?direction={direction}&amount={amount}
		while (opt.find('=') != string::npos) {
			opt = opt.erase(0, 1);

			int poz_equal = opt.find('=');
			int end = opt.find('&');
			if (end == string::npos) {
				end = opt.size();
			}
			string actual_key = opt.substr(0, poz_equal);
			string actual_value = opt.substr(poz_equal + 1, end - poz_equal - 1);

			request_params[actual_key] = actual_value;
			opt = opt.substr(end);
		}
	}
	str = str.substr(str.find("\r\n") + 2);

	// Get the http header fields
	while (str.size() > 2) {
		poz1 = str.find(':');
		poz2 = str.find("\r\n");
		string key = str.substr(0, poz1);
		string value = eraseStartSpaces(str.substr(poz1 + 1, poz2 - poz1 - 1));
		transform(key.begin(), key.end(), key.begin(), ::tolower);

		request_params[key] = value;
		str = str.substr(poz2 + 2);
	}

	// Construct the http context
	http_context->setClient(client_struct);
	http_context->setRequestParams(request_params);

	// Debugging
	cout << request_page << endl;

	delete[] initial_buff;
	return http_context;
}

string Http::readBody(CLIENT_STRUCTURE& client_struct, int lenth) {
	if (lenth == 0) return string();

	char* buff = new char[lenth + 1];
	bool parser_result = recvAll(client_struct, buff, lenth);
	if (!parser_result)
		throw string("Disconnected!");

	buff[lenth] = 0;
	string str(buff);
	delete[] buff;

	return str;
}

void Http::sendResponse(HttpContext& http_context, int responseCode, string body, map<string, string> fields) {
	int content_length = body.length();
	bool network_result;
	ostringstream header;

	header << "HTTP/1.1 " << Http::HTTP_CODES.at(responseCode) << "\r\nConnection: Close\r\n" << "Access-Control-Allow-Origin: *\r\n";
	if (content_length != 0) {
		header << "Content-Length: " << content_length << "\r\n";
	}

	for (auto const& x : fields) {
		header << x.first << ": " << x.second << "\r\n";
	}
	header << "\r\n";

	string response = header.str();
	network_result = sendAll(http_context.getClient(), response.c_str(), response.size());

	if (content_length != 0) network_result = sendAll(http_context.getClient(), body.c_str(), body.length());

	if (!network_result) throw string("Disconnected!");
}

string Http::genRandomId(const int len) {

	string tmp_s;
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	srand((unsigned)time(NULL));

	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i)
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];

	return tmp_s;
}

void Http::sendChunk(HttpContext& http_context, const char* buffer, int len) {
	ostringstream body_stream;
	string body_string;
	bool result;

	body_stream << hex << len << "\r\n";
	body_string = body_stream.str();
	result = sendAll(http_context.getClient(), body_string.c_str(), body_string.size());

	if (len != 0) result = sendAll(http_context.getClient(), buffer, len);

	result = sendAll(http_context.getClient(), "\r\n", 2);

	if (result == false)
		throw "Disconnected";
}

string Http::eraseStartSpaces(string txt) {
	int len = 0;
	for (char& c : txt) {
		if (c == ' ') len++;
		else break;
	}
	txt.erase(0, len);
	return txt;
}
const map<int, string> Http::HTTP_CODES = Http::createHttpCodes();
const map<string, string> Http::TYPE_CODES = Http::createTypeCodes();
