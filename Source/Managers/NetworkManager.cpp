#include "NetworkManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <thread>
#include <memory>

#include "Network/Command.h"
#include "Network/ImageCommand.h"
#include "Network/ImageData.h"

#include "Network/NetworkInterface.h"

#include "Program.h"
#include "Managers/WindowManager.h"

#pragma comment(lib, "ws2_32.lib")

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

NetworkManager::NetworkManager()
{
	
}

NetworkManager::~NetworkManager()
{
    NetworkInterface::Close(clientSocket_);
    NetworkInterface::Close(frameDataSocket_);
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

    CommandMessage commandMessage;
    commandMessage.command = Command::Connect;
    strncpy_s(commandMessage.commandMessage, sizeof(commandMessage.commandMessage), "Hello, Server!", _TRUNCATE);

    int sendResult = NetworkInterface::Send(clientSocket_, (char*)&commandMessage, sizeof(commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    commandMessage.command = Command::TakeAPhotograph;
    strncpy_s(commandMessage.commandMessage, sizeof(commandMessage.commandMessage), "TakeAPhotograph", _TRUNCATE);
    sendResult = NetworkInterface::Send(clientSocket_, (char*)&commandMessage, sizeof(commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    std::thread(&NetworkManager::StartListeningToServerForFrameData, this).detach();
}

void NetworkManager::Cleanup()
{
#ifdef PLATFORM_WINDOWS
    WSACleanup();
#endif
}

constexpr int cameraWidth = 1920;
constexpr int cameraHeight = 1080;
constexpr int colorDepth = 3;
constexpr int sectionCount = 3;
constexpr int sectionSize = cameraWidth * colorDepth / sectionCount;
char buffer[cameraWidth + 2 * sizeof(int)];
char image[cameraWidth * cameraHeight * colorDepth];

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

    sockaddr_in serverAddress;
    socklen_t serverAddressSize = sizeof(serverAddress);

    CommandMessage commandMessage;
    commandMessage.command = Command::StartStream;
    strncpy_s(commandMessage.commandMessage, sizeof(commandMessage.commandMessage), "StartStream", _TRUNCATE);
    int sendResult = NetworkInterface::Send(clientSocket_, (char*)&commandMessage, sizeof(commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }

    memset(image, 0x00, cameraWidth * cameraHeight * colorDepth);

    std::cout << "Listening image data" << std::endl;
    while (true)
    {
        int received = NetworkInterface::ReceiveFrom(frameDataSocket_, buffer, sizeof(buffer), 0, (sockaddr*)&serverAddress, &serverAddressSize);

        if ((sizeof(ImageData::rowIndex) + sizeof(ImageData::sectionIndex) + sectionSize) <= received)
        {
            int rowIndex;
            memcpy(&rowIndex, buffer, sizeof(int));

            int sectionIndex = 0;
            memcpy(&sectionIndex, buffer + sizeof(int), sizeof(int));

            const int rowStart = rowIndex * cameraWidth * colorDepth;
            const int sectionOffset = sectionIndex * sectionSize;
            memcpy(image + rowStart + sectionOffset, &buffer[2 * sizeof(int)], sectionSize);

            if (rowIndex == (cameraHeight - 1) && sectionIndex == (sectionCount - 1))
            {
                Program::GetInstance()->GetWindowManager()->UpdateCameraImageSurface(image);
                //stbi_write_png("C:/Users/Baris/Desktop/Test.png", cameraWidth, cameraHeight, colorDepth, image, cameraWidth * colorDepth);
            }
        }
    }

    NetworkInterface::Close(frameDataSocket_);
}