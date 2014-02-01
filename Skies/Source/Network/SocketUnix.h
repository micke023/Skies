#ifndef SOCKETUNIX_H
#define SOCKETUNIX_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include "ISocket.h"

class SocketUnix : ISocket
{
public:
	SocketUnix();
	bool Init();
	bool Open(unsigned short port);
	bool Send(const Address &destination, const void *data, int size);
	int Receive(Address &sender, void *data, int size);
	void Close();
	void Release();
	void Wait(float seconds);
	~SocketUnix();
};
#endif