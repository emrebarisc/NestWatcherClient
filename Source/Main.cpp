#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 1000

enum class Command : unsigned char
{
    None = 0,
    Connect,
    Disconnect,
    StartCamera,
    StopCamera,
    SetResolution,
    SetFPS,
    SetPixelFormat,
    StartRecording,
    PauseRecording,
    StopRecording,
    TakeAPhotograph,
    StartStream,
    StopStream,
    Shutdown
};

class CommandMessage
{
public:
    Command command;
    char commandMessage[64];
};

int main()
{
    WSADATA wsaData;
    int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaStartupResult != 0)
    {
        std::cerr << "WSAStartup failed with error: " << wsaStartupResult << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);

    const char* serverIP = "192.168.31.154";
    if (InetPton(AF_INET, serverIP, &serverAddress.sin_addr) <= 0)
    {
        std::cerr << "Invalid address or address not supported." << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    int connectResult = connect(clientSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
    if (connectResult == SOCKET_ERROR)
    {
        std::cerr << "Connection failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    CommandMessage* commandMessage = new CommandMessage();
    commandMessage->command = Command::TakeAPhotograph;
    strncpy_s(commandMessage->commandMessage, sizeof(commandMessage->commandMessage), "Hello, Server!", _TRUNCATE);

    int sendResult = send(clientSocket, (char*)commandMessage, sizeof(*commandMessage), 0);
    if (sendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
    delete commandMessage;

    CommandMessage* takeAPhotoCommandMessage = new CommandMessage();
    takeAPhotoCommandMessage->command = Command::TakeAPhotograph;
    strncpy_s(takeAPhotoCommandMessage->commandMessage, sizeof(takeAPhotoCommandMessage->commandMessage), "TakeAPhotograph", _TRUNCATE);
    int photoCommandSendResult = send(clientSocket, (char*)takeAPhotoCommandMessage, sizeof(*takeAPhotoCommandMessage), 0);
    if (photoCommandSendResult == SOCKET_ERROR)
    {
        std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
    }
    delete takeAPhotoCommandMessage;

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}