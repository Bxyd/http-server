#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "config.h"
#include "PollEvent.h"

#define MAXLINE 50000
#define SERV_PORT 9734

int findEmptyBuffIndex(char *c[5000])
{
	int n = 0;
	while (n < MAXLISTENNUM)
	{
		if (*(c + n) == 0)
		{
			return n;
		}
		else
		{
			n++;
		}
	}
	return -1;
}

int main()
{
	int listenfd, connfd;
	struct sockaddr_in cliaddr, servaddr;
	socklen_t clilen = sizeof(cliaddr);
	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, MAXLISTENNUM);

	epoll_event epoll_event_temp;

	RequestBuff request_buff;
	std::vector<RequestBuff> request_buff_set;
	request_buff_set.push_back(request_buff);
	request_buff_set[0].fd = listenfd;
	request_buff_set[0].storage_index = 0;

	PollEvent poll_event;
	poll_event.updateEvents(listenfd, 
		READ, 
		0, 
		&request_buff_set[0], 
		&epoll_event_temp); 

	char read_client_buff[MAXLISTENNUM][MAXREQUESTLEN];
	memset(read_client_buff, 0, sizeof(read_client_buff));

	while (true)
	{
		int ready_fd_num = poll_event.doPoll();
		for (int i = 0; i < ready_fd_num; i++)
		{
			int fd = poll_event.getIndexEventItemFd(i);	
			int event_type = poll_event.getIndexEventItemType(i);

			if (fd <= 2)
			{
				continue;
			}

			if (event_type == READ)
			{
				if (fd == listenfd)
				{
					connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
					RequestBuff request_buff;
					request_buff_set.push_back(request_buff);
					int cur_index = request_buff_set.size() - 1;
					request_buff_set[cur_index].fd = connfd;
					request_buff_set[cur_index].storage_index = cur_index;

					poll_event.updateEvents(connfd, 
						READ, 
						0, 
						&request_buff_set[cur_index], 
						&epoll_event_temp);
				}
				else
				{
					int empty_buff_index = findEmptyBuffIndex((char **)read_client_buff);
					if (empty_buff_index < 0)
					{
						continue;
					}
					int read_len = read(fd, read_client_buff[empty_buff_index], MAXREQUESTLEN);
					if (read_len <= 0)
					{
						close(fd);
						continue;
					}
					
					RequestBuff request_buff;
					request_buff_set.push_back(request_buff);
					int cur_index = request_buff_set.size() - 1;
					request_buff_set[cur_index].fd = connfd;
					request_buff_set[cur_index].data_index = empty_buff_index;
					request_buff_set[cur_index].storage_index = cur_index;
					request_buff_set[cur_index].request.parseRequest(read_client_buff[0]
						+ empty_buff_index); 
					
					poll_event.updateEvents(fd, 
						WRITE, 
						1, 
						&request_buff_set[cur_index],
						&epoll_event_temp);
				}
			}
			else if (event_type == WRITE)
			{
				RequestBuff *request_buff = poll_event.getIndexEventItemData(i);						
				Request *http_request = &(request_buff->request);
				FILE *fp = fdopen(fd, "w+");
				Response response;
				response.doResponse(http_request, fp);
		
				fflush(fp);
				fclose(fp);
				close(connfd);

				memset(read_client_buff[request_buff->data_index], 
					0, 
					sizeof(char) * MAXREQUESTLEN);
				request_buff_set.erase(request_buff_set.begin() + request_buff->storage_index);
		
			}
		}
	}
	exit(0);
}
