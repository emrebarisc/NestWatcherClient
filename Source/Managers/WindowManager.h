#pragma once

#include "IManager.h"

class SDL_Surface;
class SDL_Window;

class WindowManager : public IManager
{
public:
	WindowManager();
	~WindowManager();

	void Init() override;
	void Start() override;

	void PollEvent();

	void UpdateCameraImageSurface(char imageRawData[]);

protected:

private:
	SDL_Surface* windowSurface_{ nullptr };
	SDL_Window* window_{ nullptr };

	int windowWidth_{ 1600 };
	int windowHeight_{ 900 };
};