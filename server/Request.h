#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>
#include <vector>
#include <unordered_map>

#include <stdlib.h>
#include <string.h>

#include <dirent.h>

#include "config.h"

class Request 
{
public:
	char *method;	
	char *url;
	char *version;	
	std::unordered_map<std::string, std::string> headers;
	char *body;

	Request();
	
	void parseRequest(char *data);
};

#endif
