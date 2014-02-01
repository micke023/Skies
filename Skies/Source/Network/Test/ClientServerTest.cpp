#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "..\Connection.h"

using namespace std;

const int ServerPort = 30000;
const int ClientPort = 30001;
const int ProtocolId = 0x99887766;
const float DeltaTime = 0.25f;
const float SendRate = 0.25f;
const float TimeOut = 10.0f;

int main(int argc, char * argv[])
{
	Connection connection;
	connection.Init(ProtocolId, TimeOut);

	char input;
	cout<<"Server(s) or Client(c)? ";
	cin>>input;

	if(input == 's')
	{
		if (!connection.Start(ServerPort))
		{
			printf("Could not start connection on port %d\n", ServerPort);
			return 1;
		}
	
		connection.Listen();
	
		while (!GetAsyncKeyState(VK_ESCAPE))
		{
			if (connection.IsConnected())
			{
				unsigned char package[] = "Server to client";
				connection.SendPackage(package, sizeof(package));
			}
		
			while (!GetAsyncKeyState(VK_ESCAPE))
			{
				unsigned char package[256];
				int bytes_read = connection.ReceivePacket(package, sizeof(package));
				if (bytes_read == 0)
					break;
				printf("Received packet from client\n");
			}
		
			connection.Update(DeltaTime);
			connection.Wait(DeltaTime);
		}
	}
	else
	{
		if (!connection.Start(ClientPort))
		{
			printf("Could not start connection on port %d\n", ClientPort);
			return 1;
		}
	
		connection.Connect(Address(127,0,0,1,ServerPort));
		bool connected = false;
	
		while (!GetAsyncKeyState(VK_ESCAPE))
		{
			if (!connected && connection.IsConnected())
			{
				printf("Client connected to server\n");
				connected = true;
			}
		
			if (!connected && connection.ConnectFailed())
			{
				printf("Connection failed\n");
				break;
			}
		
			unsigned char package[] = "Client to server";
			connection.SendPackage(package, sizeof(package));
		
			while (!GetAsyncKeyState(VK_ESCAPE))
			{
				unsigned char packet[256];
				int bytes_read = connection.ReceivePacket(packet, sizeof(packet));
				if (bytes_read == 0)
					break;
				printf("Received packet from server\n");
			}
		
			connection.Update(DeltaTime);
			connection.Wait(DeltaTime);
		}
	}
	connection.Release();
	return 0;
}
