#ifndef ISOCKET_H
#define ISOCKET_H

#include <assert.h>
#include <vector>
#include <map>
#include <stack>
#include "Address.h"

class ISocket
{
public:
	ISocket()
	{
		mSocket = 0;
	}
	bool IsOpen() const
	{
		return mSocket != 0;
	}
	virtual bool Init()=0;
	virtual bool Open(unsigned short port)=0;
	virtual void Close()=0;
	virtual bool Send(const Address &destination, const void *data, int size)=0;
	virtual int Receive(Address &sender, void *data, int size)=0;
	virtual void Release()=0;
	virtual void Wait(float seconds)=0;
	virtual ~ISocket(){}
protected:
	int mSocket;
};
#endif