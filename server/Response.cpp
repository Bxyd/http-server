#include "Response.h"

const char *home_url = "/";
const char *webpageo_url = "/webpage";
const char *webpaget_url = "/webpage/";

const char *error_file = "webpage/404.html";

Response::Response() : version(nullptr), code(nullptr), desc(nullptr), body(nullptr), body_size(0) {}

Response::~Response()
{
	if (this->body)
	{
		free(this->body);
		this->body = nullptr;
	}
}

void Response::doResponse(Request *request, FILE *stream)
{
	if (request->url == nullptr)
	{
		return;
	}

	if (!strcmp(home_url, request->url) 
	 	|| !strcmp(webpageo_url, request->url) 
	 	|| !strcmp(webpaget_url, request->url))
	{
		showDirContent();
	}
	else
	{
		responseFileContent(request->url + 1);
	}

	char content_len[25];
	sprintf(content_len, "%d", this->body_size);
	this->headers.insert(std::pair<std::string, std::string>("Content-Length",content_len));		

	outputToFile(stream);	

	this->headers.erase(this->headers.begin(), this->headers.end());
}

void Response::outputToFile(FILE *stream)
{
	fprintf(stream, "%s %s %s\r\n", 
		this->version, 
		this->code, 
		this->desc
	);

	for (std::unordered_map<std::string, std::string>::iterator it = this->headers.begin(); 
		it != this->headers.end(); it++)	
	{
		fprintf(stream, "%s: %s\r\n", it->first.c_str(), it->second.c_str());	
	}

	fprintf(stream, "\r\n");


	if (this->body != nullptr && this->body_size > 0)
	{
		fwrite(this->body, this->body_size, 1, stream);
	}
}

void Response::responseFileContent(const char *file_path)
{
	FILE *fileptr = fopen(file_path, "rb");
	if (nullptr == fileptr)
	{
		fileptr = fopen(error_file, "rb");
		this->version = "HTTP/1.1";
		this->code = "404";
		this->desc = "Not Found";
	}
	else
	{
		this->version = "HTTP/1.1";	
		this->code = "200";
		this->desc = "OK";
	}

	int len = strlen(file_path);
	if (file_path[len - 1] == 'l' || this->code == "404")
	{
		this->headers.insert(std::pair<std::string, std::string>("Content-Type","text/html"));
	}
	else
	{
		this->headers.insert(std::pair<std::string, std::string>("Content-Type","image/jpeg"));
	}

	fseek(fileptr, 0, SEEK_END);
	this->body_size = ftell(fileptr);
	rewind(fileptr);
	this->body = (char *)malloc((this->body_size + 1));
	memset(this->body, 0, this->body_size + 1);
	fread(this->body, this->body_size, 1, fileptr);
	fclose(fileptr);

	return;
}

void Response::showDirContent()
{
	this->version = "HTTP/1.1";	
	this->code = "200";
	this->desc = "OK";

	char *path = "webpage";
	char *html = "<html> <ul> %s </ul> </html>";
	char *li = "<li><a href='/webpage/%s'>webpage/%s</a></li>";

	char li_str[500];
	memset(li_str, 0, sizeof(li_str));
	char *li_str_ptr = li_str;
	DIR *dp = opendir(path);
	struct dirent *dirp;

	while ((dirp = readdir(dp)) != nullptr)
	{
		if (dirp->d_type != DT_DIR)
		{
			sprintf(li_str_ptr, li, dirp->d_name, dirp->d_name);
			li_str_ptr = li_str + strlen(li_str);
		}
	}
	closedir(dp);

	this->body_size = (strlen(li_str) + strlen(html) - 2) * sizeof(char);
	this->body = (char *)malloc(this->body_size + 1);
	memset(this->body, 0, this->body_size + 1);
	sprintf(this->body, html, li_str);
	return;
}

