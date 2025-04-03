#include "NetworkManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <thread>

#include "Network/Command.h"
#include "Network/ImageCommand.h"
#include "Network/ImageData.h"

#include "Network/NetworkInterface.h"

#pragma comment(lib, "ws2_32.lib")

NetworkManager::NetworkManager()
{
	
}

NetworkManager::~NetworkManager()
{
    Cleanup();
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
    clientSocket_ = NetworkInterface::CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket_ == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        Cleanup();
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported." << std::endl;
        NetworkInterface::Close(clientSocket_);
        Cleanup();
        return;
    }

    int connectResult = NetworkInterface::Connect(clientSocket_, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connectResult == SOCKET_ERROR)
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        NetworkInterface::Close(clientSocket_);
        Cleanup();
        return;
    }

    CommandMessage* commandMessage = new CommandMessage();
    commandMessage->command = Command::Connect;
    strncpy_s(commandMessage->commandMessage, sizeof(commandMessage->commandMessage), "Hello, Server!", _TRUNCATE);

    int sendResult = NetworkInterface::Send(clientSocket_, (char*)commandMessage, sizeof(*commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    commandMessage->command = Command::TakeAPhotograph;
    strncpy_s(commandMessage->commandMessage, sizeof(commandMessage->commandMessage), "TakeAPhotograph", _TRUNCATE);
    sendResult = NetworkInterface::Send(clientSocket_, (char*)commandMessage, sizeof(*commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    commandMessage->command = Command::StartStream;
    strncpy_s(commandMessage->commandMessage, sizeof(commandMessage->commandMessage), "StartStream", _TRUNCATE);
    sendResult = NetworkInterface::Send(clientSocket_, (char*)commandMessage, sizeof(*commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    delete commandMessage;

    StartListeningToServerForFrameData();

    NetworkInterface::Close(clientSocket_);
}

void NetworkManager::Cleanup()
{
#ifdef PLATFORM_WINDOWS
    WSACleanup();
#endif
}

void NetworkManager::StartListeningToServerForFrameData()
{
    frameDataSocket_ = NetworkInterface::CreateSocket(AF_INET, SOCK_DGRAM, 0);

    frameDataAddress_.sin_family = AF_INET;
    frameDataAddress_.sin_port = htons(FRAME_DATA_PORT);
    frameDataAddress_.sin_addr.s_addr = INADDR_ANY;

    int frameDataAddressBindResult = NetworkInterface::Bind(frameDataSocket_, (sockaddr*)&frameDataAddress_, sizeof(frameDataAddress_));
    if (frameDataAddressBindResult != 0)
    {
        std::cerr << "Failed to bind frame data address with error: " << WSAGetLastError() << std::endl;
        NetworkInterface::Close(frameDataSocket_);
    
        return;
    }

    std::cout << "Data server started." << std::endl;

    ImageData imageData;
    imageData.row = new uint8_t[1920];

    sockaddr_in serverAddress;
    socklen_t serverAddressSize = sizeof(serverAddress);

    while (true)
    {
        CommandMessage commandMessage;
        commandMessage.command = Command::StartStream;
        strncpy_s(commandMessage.commandMessage, sizeof(commandMessage.commandMessage), "StartStream", _TRUNCATE);
        int sendResult = NetworkInterface::Send(clientSocket_, (char*)&commandMessage, sizeof(commandMessage), 0);
        if (sendResult == SOCKET_ERROR)
        {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
        }

        while (0 < NetworkInterface::ReceiveFrom(frameDataSocket_, (char*)&imageData, sizeof(imageData), 0, (sockaddr*)&serverAddress, &serverAddressSize))
        {
            std::cout << "Row index " << imageData.rowIndex << " received from " << inet_ntoa(serverAddress.sin_addr) << std::endl;
        }

        NetworkInterface::Close(frameDataSocket_);
    }
}