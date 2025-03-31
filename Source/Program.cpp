#include "Program.h"

#include "Managers/NetworkManager.h"
#include "Managers/WindowManager.h"

Program::Program()
{
	networkManager_ = new NetworkManager();
	windowManager_ = new WindowManager();
}

Program::~Program()
{

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

	while (true)
	{
		windowManager_->PollEvent();
	}
}