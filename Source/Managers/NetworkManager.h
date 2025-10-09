#pragma once

#include "IManager.h"

#include <ws2tcpip.h>

#define SERVER_IP "192.168.31.154"

constexpr int CAMERA_WIDTH = 1920;
constexpr int CAMERA_HEIGHT = 1080;
constexpr int COLOR_DEPTH = 3;
constexpr int SECTION_COUNT = 3;
constexpr int SECTION_SIZE = CAMERA_WIDTH * COLOR_DEPTH / SECTION_COUNT;

class NetworkManager : public IManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Init() override;
	void Start() override;

protected:
	void Cleanup();

	void StartListeningToServerForFrameData();

private:
	static constexpr int SERVER_PORT{ 1000 };
	static constexpr int FRAME_DATA_PORT{ 1001 };
	static constexpr int CAMERA_IMAGE_COMMAND_PORT{ 1002 };

	sockaddr_in frameDataAddress_{ 0 };

	int clientSocket_{ 0 };
	int frameDataSocket_{ 0 };
};