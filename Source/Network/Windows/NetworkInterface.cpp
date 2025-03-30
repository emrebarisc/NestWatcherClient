#include "Network/NetworkInterface.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

int NetworkInterface::CreateSocket(int af, int type, int protocol)
{
	return socket(af, type, protocol);
}

int NetworkInterface::Connect(int socketFd, sockaddr* address, int length)
{
	return connect(socketFd, address, length);
}

int NetworkInterface::Bind(int socketFd, sockaddr* address, int length)
{
	return bind(socketFd, address, length); 
}

int NetworkInterface::Close(int socketFd)
{
	return closesocket(socketFd);
}

int NetworkInterface::Accept(int socketFd, sockaddr* address, int* length)
{
	return accept(socketFd, address, length);
}

int NetworkInterface::Send(int socketFd, const char* buffer, size_t size, int flags)
{
	return send(socketFd, buffer, size, flags);
}

int NetworkInterface::Receive(int socketFd, char* buffer, size_t size, int flags)
{
	return recv(socketFd, buffer, size, flags);
}