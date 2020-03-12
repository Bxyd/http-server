#include "Request.h"

Request::Request() : method(nullptr), url(nullptr), version(nullptr), body(nullptr) {} 

void Request::parseRequest(char *data)
{
	/* 
	   解析请求行 
	*/
	char *method = data;
	char *url = nullptr;
	char *version = nullptr;

	char *it = data;		//it指针指向请求行首字符
	while (*it != '\0' && *it != '\r')
	{
		if (*it == ' ')
		{
			if (url == nullptr)
			{
				url = it + 1;
			}
			else
			{
				version = it + 1;
			}
			*it = '\0';
		}
		it++;
	}
	*it = '\0';				//it指针此时指向'\r',将其改为'\0'
	this->method = method;
	this->url = url;
	this->version = version;
	/* 
	   请求行解析完成
	   接下来解析首部行 
	*/
	it += 2;				//it指针此时指向首部行首字符
	char *line = it;		//令line指针永远指向行首字符	
	while (*line != '\0' && *line != '\r')
	{
		char *key = nullptr;
		char *value = nullptr;

		while (*(it++) != ':');	//it指针指向冒号后一个字符

		*(it - 1) = '\0';
		key = line;
		value = it + 1;
		while (*it != '\0' && *it != '\r')
		{
			it++;
		}
		*it = '\0';		//it指针指向'\r'	
		std::pair<std::string, std::string> item(key, value);
		this->headers.insert(item);

		it += 2;		//it指针指向下一行首字符
		line = it;		//更新迭代变量
	}
	/*
	   首部行解析完成
	   接下来判断实体主体是否存在，
	*/
	if (*line == '\r')
	{
		if (this->headers.count("Content-Length"))
		{
			const char *body_len = this->headers["Content-Length"].c_str();
			if (body_len != nullptr)
			{
				int len = atoi(body_len);
				line = line + 2;
				*(line + len) = '\0';
				this->body = line;
			}
		}
		else
		{
			this->body = line + 2;;
		}
	}
}
