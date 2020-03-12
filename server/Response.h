#ifndef RESPONSE_H
#define RESPONSE_H

#include <unordered_map>
#include "Request.h"

class Response
{
public:
	char *version;
	char *code;
	char *desc;
	std::unordered_map<std::string, std::string> headers;
	char *body;
	int body_size;

	Response();
	~Response();

	void doResponse(Request *request, FILE *stream);	
	void outputToFile(FILE *stream);
	void responseFileContent(const char *file_path);
	void showDirContent();	
};

#endif
