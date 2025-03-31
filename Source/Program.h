#pragma once

class NetworkManager;
class WindowManager;

class Program
{
public:
	Program();
	~Program();

	void Init();
	void Run();

protected:

private:
	NetworkManager* networkManager_{ nullptr };
	WindowManager* windowManager_{ nullptr };
};