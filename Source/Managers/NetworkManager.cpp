#define NOMINMAX // Fixes the std::max compiler error caused by winsock2.h

#include "NetworkManager.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#include <iostream>
#include <thread>
#include <memory>
#include <map>
#include <vector>
#include <algorithm>

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

void NetworkManager::SetServerIP(const std::string& ip)
{
    serverIP_ = ip;
}

void NetworkManager::Start()
{
    clientSocket_ = NetworkInterface::CreateSocket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (clientSocket_ == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed..." << std::endl;
        Cleanup();
        return;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, serverIP_.c_str(), &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported: " << serverIP_ << std::endl;
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

    std::cout << "Listening image data" << std::endl;

    std::map<uint32_t, std::vector<ImageData>> frameBufferMap;
    uint32_t latestFrameId = 0;
    ImageData buffer;

    while (true)
    {
        int received = NetworkInterface::ReceiveFrom(frameDataSocket_, (char*)&buffer, sizeof(ImageData), 0, (sockaddr*)&serverAddress, &serverAddressSize);

        if (received > 0)
        {
            if (buffer.frameId < latestFrameId && (latestFrameId - buffer.frameId) > 10)
            {
                continue;
            }

            latestFrameId = std::max(latestFrameId, buffer.frameId);
            frameBufferMap[buffer.frameId].push_back(buffer);

            if (frameBufferMap[buffer.frameId].size() == buffer.totalChunks)
            {
                auto& chunks = frameBufferMap[buffer.frameId];

                std::sort(chunks.begin(), chunks.end(), [](const ImageData& a, const ImageData& b)
                    {
                        return a.chunkIndex < b.chunkIndex;
                    });

                std::vector<uint8_t> jpegData;
                jpegData.reserve(buffer.totalChunks * 1024);
                for (const auto& chunk : chunks)
                {
                    jpegData.insert(jpegData.end(), chunk.data, chunk.data + chunk.dataSize);
                }

                int w, h, channels;
                unsigned char* pixels = stbi_load_from_memory(jpegData.data(), jpegData.size(), &w, &h, &channels, 3);

                if (pixels)
                {
                    Program::GetInstance()->GetWindowManager()->UpdateCameraImageSurface(pixels, w, h);
                    stbi_image_free(pixels);
                }

                for (auto it = frameBufferMap.begin(); it != frameBufferMap.end(); )
                {
                    if (it->first <= buffer.frameId)
                    {
                        it = frameBufferMap.erase(it);
                    }
                    else
                    {
                        ++it;
                    }
                }
            }
        }
    }

    NetworkInterface::Close(frameDataSocket_);
}