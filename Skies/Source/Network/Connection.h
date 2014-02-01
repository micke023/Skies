#ifndef CONNECTION_H
#define CONNECTION_H

#if defined(_WIN32)
	#include "SocketWin32.h"
#else
	#include "SocketUnix.h"
#endif

enum Mode
{
	None,	
	Client,
	Server
};

#include "Address.h"

class Connection
{
public:
	Connection();
	void Init(unsigned int protocolId, float timeout);
	bool Start(int port);
	void Stop();
	void Listen();
	void Connect(const Address & address);
	bool IsConnecting() const;
	bool ConnectFailed() const;
	bool IsConnected() const;
	bool IsListening() const;
	Mode GetMode() const;
	void Update(float deltaTime);
	bool SendPackage(const unsigned char data[], int size);
	int ReceivePacket(unsigned char data[], int size);
	void Wait(float seconds);
	void Release();
	~Connection();
protected:
	void ClearData();
private:
	enum State
	{
		Disconnected,
		Listening,
		Connecting,
		ConnectFail,
		Connected
	};
	unsigned int mProtocolId;
	float mTimeout;
	bool mRunning;
	Mode mMode;
	State mState;
	ISocket *mSocket;
	float mTimeoutAccumulator;
	Address mAddress;
};
#endif