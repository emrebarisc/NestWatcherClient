#pragma once

struct sockaddr;

namespace NetworkInterface
{
	extern int CreateSocket(int af, int type, int protocol);
	extern int Connect(int socket, sockaddr* socketAddress, int length);
	extern int Bind(int socket, sockaddr* socketAddress, int length);
	extern int Close(int socket);
	extern int Accept(int socketFd, struct sockaddr* address, int* length);
	extern int Send(int socketFd, const char* buffer, size_t size, int flags);
	extern int Receive(int socketFd, char* buffer, size_t size, int flags);
}