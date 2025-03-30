#pragma once

class NetworkInterface
{
public:
	static int CreateSocket(int af, int type, int protocol);
	static int Bind(int socket, struct sockaddr* socketAddress, int length);
	static int Close(int socket);
	static int Accept(int socketFd, struct sockaddr* address, int* length);
	static int Send(int socketFd, const char* buffer, size_t size, int flags);
	static int Receive(int socketFd, char* buffer, size_t size, int flags);
};