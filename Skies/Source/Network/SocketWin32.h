#ifndef SOCKETWIN32_H
#define SOCKETWIN32_H

#include "ISocket.h"
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

class SocketWin32 : public ISocket
{
public:
	SocketWin32();
	bool Init();
	bool Open(unsigned short port);
	bool Send(const Address &destination, const void *data, int size);
	int Receive(Address &sender, void *data, int size);
	void Close();
	void Release();
	void Wait(float seconds);
	~SocketWin32();
};
#endif