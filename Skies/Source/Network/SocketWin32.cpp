#include "SocketWin32.h"

SocketWin32::SocketWin32()
{

}

bool SocketWin32::Init()
{
	WSADATA WsaData;
	return WSAStartup(WINSOCK_VERSION, &WsaData) == NO_ERROR;
}

bool SocketWin32::Open(unsigned short port)
{
	//Create socket
	mSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(mSocket == INVALID_SOCKET)
	{
		printf("Failed to create socket\n");
		mSocket = 0;
		return false;
	}

	//Bind to port
	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons((unsigned short)port);
		
	if(bind(mSocket, (const sockaddr*)&address, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		printf("Failed to bind socket\n");
		Close();
		return false;
	}

	//Set non-blocking io
	DWORD nonBlocking = 1;
	if(ioctlsocket(mSocket, FIONBIO, &nonBlocking) == SOCKET_ERROR)
	{
		printf("Failed to set non-blocking socket\n");
		Close();
		return false;
	}

	return true;
}

void SocketWin32::Close()
{
	if(socket != 0)
	{
		closesocket(mSocket);
		mSocket = 0;
	}
}

bool SocketWin32::Send(const Address &destination, const void *data, int size)
{
	if(socket == 0)
		return false;

	sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(destination.GetAddress());
	address.sin_port = htons((unsigned short)destination.GetPort());
	int sent_bytes = sendto(mSocket, (const char*)data, size, 0, (sockaddr*)&address, sizeof(sockaddr_in));

	return sent_bytes == size;
}

int SocketWin32::Receive(Address &sender, void *data, int size)
{
	if(mSocket == 0)
		return false;
			
	typedef int socklen_t;
	sockaddr_in from;
	socklen_t fromLength = sizeof(from);
	int received_bytes = recvfrom(mSocket, (char*)data, size, 0, (sockaddr*)&from, &fromLength);

	if(received_bytes <= 0)
		return 0;

	unsigned int address = ntohl(from.sin_addr.s_addr);
	unsigned short port = ntohs(from.sin_port);
	sender = Address(address, port);

	return received_bytes;
}

void SocketWin32::Release()
{
	WSACleanup();
}

void SocketWin32::Wait(float seconds)
{
	Sleep((int)(seconds * 1000.0f));
}

SocketWin32::~SocketWin32()
{
	Close();
}

