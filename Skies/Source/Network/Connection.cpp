#include "Connection.h"

Connection::Connection()
{

}

void Connection::Init(unsigned int protocolId, float timeout)
{
#if defined(_WIN32)
	mSocket = new SocketWin32();
#else
	mSocket = new SocketUnix();
#endif
	mProtocolId = protocolId;
	mTimeout = timeout;
	mMode = None;
	mRunning = false;
	ClearData();
}

bool Connection::Start(int port)
{
	mSocket->Init();
	printf( "Start connection on port %d\n", port );
	if ( !mSocket->Open( port ) )
		return false;
	mRunning = true;
	return true;
}

void Connection::Stop()
{
	printf( "Stop connection\n" );
	ClearData();
	mSocket->Close();
	mRunning = false;
}

void Connection::Listen()
{
	printf( "Server listening for connection\n" );
	ClearData();
	mMode = Server;
	mState = Listening;
}

void Connection::Connect(const Address & address)
{
	printf( "Client connecting to %d.%d.%d.%d:%d\n", 
		address.GetA(), address.GetB(), address.GetC(), address.GetD(), address.GetPort() );
	ClearData();
	mMode = Client;
	mState = Connecting;
	mAddress = address;
}

bool Connection::IsConnecting() const
{
	return mState == Connecting;
}

bool Connection::ConnectFailed() const
{
	return mState == ConnectFail;
}

bool Connection::IsConnected() const
{
	return mState == Connected;
}

bool Connection::IsListening() const
{
	return mState == Listening;
}

Mode Connection::GetMode() const
{
	return mMode;
}

void Connection::Update(float deltaTime)
{
	mTimeoutAccumulator += deltaTime;
	if (mTimeoutAccumulator > mTimeout)
	{
		if (mState == Connecting)
		{
			printf("Connect timed out\n");
			ClearData();
			mState = ConnectFail;
		}
		else if (mState == Connected)
		{
			printf("Connection timed out\n");
			ClearData();
			if (mState == Connecting)
				mState = ConnectFail;
		}
	}
}

bool Connection::SendPackage(const unsigned char data[], int size)
{
	if (mAddress.GetAddress() == 0)
		return false;
	unsigned char *packet = new unsigned char[size+4];
	packet[0] = (unsigned char)(mProtocolId >> 24);
	packet[1] = (unsigned char)((mProtocolId >> 16) & 0xFF);
	packet[2] = (unsigned char)((mProtocolId >> 8) & 0xFF);
	packet[3] = (unsigned char)((mProtocolId) & 0xFF);
	memcpy( &packet[4], data, size );
	bool ret = mSocket->Send( mAddress, packet, size + 4 );
	delete[] packet;
	return ret;
}

int Connection::ReceivePacket(unsigned char data[], int size)
{
	unsigned char *packet = new unsigned char[size+4];
	Address sender;
	int bytes_read = mSocket->Receive(sender, packet, size + 4);
	if (bytes_read == 0)
		return 0;
	if (bytes_read <= 4)
		return 0;
	if (packet[0] != (unsigned char)(mProtocolId >> 24) || 
			packet[1] != (unsigned char)((mProtocolId >> 16) & 0xFF ) ||
			packet[2] != (unsigned char)((mProtocolId >> 8) & 0xFF ) ||
			packet[3] != (unsigned char)(mProtocolId & 0xFF))
		return 0;
	if (mMode == Server && !IsConnected())
	{
		printf("Server accepts connection from client %d.%d.%d.%d:%d\n", 
			sender.GetA(), sender.GetB(), sender.GetC(), sender.GetD(), sender.GetPort());
		mState = Connected;
		mAddress = sender;
	}
	if (sender == mAddress)
	{
		if (mMode == Client && mState == Connecting)
		{
			printf("client completes connection with server\n");
			mState = Connected;
		}
		mTimeoutAccumulator = 0.0f;
		memcpy(data, &packet[4], size - 4);
		delete[] packet;
		printf("The data: %s ", data);
		return size - 4;
	}
	return 0;
}

void Connection::ClearData()
{
	mState = Disconnected;
	mTimeoutAccumulator = 0.0f;
	mAddress = Address();
}

void Connection::Wait(float seconds)
{
	mSocket->Wait(seconds);
}

void Connection::Release()
{
	mSocket->Release();
}

Connection::~Connection()
{
	delete mSocket;
}