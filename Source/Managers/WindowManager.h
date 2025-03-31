#pragma once

#include "IManager.h"

class SDL_Window;

class WindowManager : public IManager
{
public:
	WindowManager();
	~WindowManager();

	void Init() override;
	void Start() override;

	void PollEvent();

protected:

private:
	SDL_Window* window_{ nullptr };
};