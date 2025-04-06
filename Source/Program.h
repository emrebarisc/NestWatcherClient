#pragma once

class NetworkManager;
class WindowManager;

class Program
{
public:
	~Program();

	static Program* GetInstance()
	{
		if (instance_ == nullptr)
		{
			instance_ = new Program();
		}

		return instance_;
	}

	void Init();
	void Run();

	void SetIsPendingExit(bool isPendingExit)
	{
		isPendingExit_ = isPendingExit;
	}

	bool GetIsPendingExit() const
	{
		return isPendingExit_;
	}

	NetworkManager* GetNetworkManager() const
	{
		return networkManager_;
	}

	WindowManager* GetWindowManager() const
	{
		return windowManager_;
	}

protected:

private:
	Program();

	static Program* instance_;

	NetworkManager* networkManager_{ nullptr };
	WindowManager* windowManager_{ nullptr };

	bool isPendingExit_{ false };
};