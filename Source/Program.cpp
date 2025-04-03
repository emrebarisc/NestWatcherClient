#include "Program.h"

#include "Managers/NetworkManager.h"
#include "Managers/WindowManager.h"

Program* Program::instance_ = nullptr;

Program::Program()
{
	networkManager_ = new NetworkManager();
	windowManager_ = new WindowManager();
}

Program::~Program()
{
	delete windowManager_;
	delete networkManager_;
}

void Program::Init()
{
	networkManager_->Init();
	windowManager_->Init();
}

void Program::Run()
{
	networkManager_->Start();
	windowManager_->Start();

	while (!isPendingExit_)
	{
		windowManager_->PollEvent();
	}
}