#ifndef POLLEVENT_H
#define POLLEVENT_H

#include <sys/epoll.h>
#include "Response.h"

#define UNKNOW 0
#define READ 1
#define WRITE 2

#define MAXEVENTS 20

class RequestBuff
{
public:
	Request request;
	int fd;
	int data_index;
	int storage_index;

	RequestBuff();
};

class PollEvent
{
public:
	int epollfd;
	struct epoll_event *event_items;
	int max_events;

	PollEvent();
	~PollEvent();

	void updateEvents(int fd, int event_flags, int modify, RequestBuff *request_buff, epoll_event *ev);	
	int doPoll();
	int getIndexEventItemFd(int index);
	int getIndexEventItemType(int index);
	RequestBuff* getIndexEventItemData(int index);
};

#endif
