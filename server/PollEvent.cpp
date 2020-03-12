#include "PollEvent.h"

RequestBuff::RequestBuff() : fd(-1), data_index(-1), storage_index(-1) {}

PollEvent::PollEvent() : max_events(MAXEVENTS)
{
	this->epollfd = epoll_create1(0);
	this->event_items = (epoll_event *)calloc(MAXEVENTS, sizeof(epoll_event));
}

PollEvent::~PollEvent()
{
	free(this->event_items);
}

void PollEvent::updateEvents(int fd, int event_flags, int modify, RequestBuff *request_buff, epoll_event *ev) 
{
	ev->data.ptr = request_buff;			
	if (event_flags == READ)
	{
		ev->events = EPOLLIN | EPOLLET;
	}
	else if (event_flags == WRITE)
	{
		ev->events = EPOLLOUT | EPOLLET;
	}

	int mod;
	if (modify)
	{
		mod = EPOLL_CTL_MOD;
	}
	else
	{
		mod = EPOLL_CTL_ADD;
	}

	epoll_ctl(this->epollfd, mod, fd, ev);
}

int PollEvent::doPoll()
{
	int num = epoll_wait(this->epollfd, this->event_items, this->max_events, -1);
	return num;
}

int PollEvent::getIndexEventItemFd(int index)
{
	return getIndexEventItemData(index)->fd;	
}

int PollEvent::getIndexEventItemType(int index)
{
	int return_type = UNKNOW;
	int flag = (this->event_items + index)->events;

	if (flag & EPOLLIN)
	{
		return_type = READ;
	}
	else if (flag & EPOLLOUT)
	{
		return_type = WRITE;
	}
	return return_type;
}

RequestBuff* PollEvent::getIndexEventItemData(int index)
{
	return (RequestBuff *)(this->event_items + index)->data.ptr;
}
