#include "NetworkManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>

#include "Network/Command.h"

#include "Network/NetworkInterface.h"

#pragma comment(lib, "ws2_32.lib")

NetworkManager::NetworkManager()
{
	
}

NetworkManager::~NetworkManager()
{
#ifdef PLATFORM_WINDOWS
	WSACleanup();
#endif
}

void NetworkManager::Init()
{
#ifdef PLATFORM_WINDOWS
    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupResult != 0)
    {
        std::cerr << "WSAStartup failed with error: " << wsaStartupResult << std::endl;
    }
#endif
}

void NetworkManager::Start()
{
    int clientSocket = NetworkInterface::CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    int connectResult = NetworkInterface::Connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connectResult == SOCKET_ERROR)
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return;
    }

    CommandMessage* commandMessage = new CommandMessage();
    commandMessage->command = Command::TakeAPhotograph;
    strncpy_s(commandMessage->commandMessage, sizeof(commandMessage->commandMessage), "Hello, Server!", _TRUNCATE);

    int sendResult = NetworkInterface::Send(clientSocket, (char*)commandMessage, sizeof(*commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
    delete commandMessage;

    CommandMessage* takeAPhotoCommandMessage = new CommandMessage();
    takeAPhotoCommandMessage->command = Command::TakeAPhotograph;
    strncpy_s(takeAPhotoCommandMessage->commandMessage, sizeof(takeAPhotoCommandMessage->commandMessage), "TakeAPhotograph", _TRUNCATE);
    int photoCommandSendResult = NetworkInterface::Send(clientSocket, (char*)takeAPhotoCommandMessage, sizeof(*takeAPhotoCommandMessage), 0);
    if (photoCommandSendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
    delete takeAPhotoCommandMessage;

    closesocket(clientSocket);
}