#include "SocketUnix.h"

SocketUnix::SocketUnix()
{

}

bool SocketUnix::Init()
{

}

bool SocketUnix::Open(unsigned short port)
{
	//Create socket
	mSocket = ::socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if(mSocket <= 0)
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
		
	if(bind(mSocket, (const sockaddr*)&address, sizeof(sockaddr_in)) < 0)
	{
		printf("Failed to bind socket\n");
		Close();
		return false;
	}

	//Set non-blocking io
	int nonBlocking = 1;
	if (fcntl(socket, F_SETFL, O_NONBLOCK, nonBlocking) == -1)
	{
		printf("Failed to set non-blocking socket\n");
		Close();
		return false;
	}

	return true;
}

void SocketUnix::Close()
{
	if(socket != 0)
	{
		close(mSocket);
		mSocket = 0;
	}
}

bool SocketUnix::Send(const Address &destination, const void *data, int size)
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

int SocketUnix::Receive(Address &sender, void *data, int size)
{
	if(mSocket == 0)
		return false;

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

void SocketUnix::Release()
{

}

void SocketUnix::Wait(float seconds)
{
	usleep((int)(seconds * 1000000.0f));
}

SocketUnix::~SocketUnix()
{
	Close();
}

