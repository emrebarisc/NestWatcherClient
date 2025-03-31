#pragma once

#include "IManager.h"

#define SERVER_PORT 1000
#define SERVER_IP "192.168.31.154"

class NetworkManager : public IManager
{
public:
	NetworkManager();
	~NetworkManager();

	void Init() override;
	void Start() override;

protected:
	void Cleanup();

private:
};