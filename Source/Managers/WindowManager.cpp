#include "WindowManager.h"

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_surface.h>

#include "Program.h"
#include "Managers/NetworkManager.h"

WindowManager::WindowManager()
{

}

WindowManager::~WindowManager()
{
	SDL_DestroySurface(windowSurface_);
	SDL_DestroyWindow(window_);

	SDL_Quit();
}

void WindowManager::Init()
{
	SDL_Init(SDL_INIT_VIDEO);

	window_ = SDL_CreateWindow("Nest Watcher Client", windowWidth_, windowHeight_, SDL_WINDOW_VULKAN);

	if (!window_)
	{
		std::cerr << "Could not create window." << std::endl;
		return;
	}

	windowSurface_ = SDL_GetWindowSurface(window_);
	if (!windowSurface_)
	{
		std::cerr << "Could not create surface." << std::endl;
		return;
	}
}

void WindowManager::Start()
{

}

void WindowManager::PollEvent()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_EVENT_QUIT)
		{
			Program::GetInstance()->SetIsPendingExit(true);
		}
	}
}

void WindowManager::UpdateCameraImageSurface(char imageRawData[])
{
	constexpr int width = 1920;
	constexpr int height = 1080;
	constexpr int pitch = width * 3;

	SDL_Surface* newSurface = SDL_CreateSurfaceFrom(
		width,
		height,
		SDL_PIXELFORMAT_RGB24,
		imageRawData,
		pitch
	);

	if (!newSurface) {
		std::cerr << "Failed to create image surface: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_BlitSurface(newSurface, nullptr, windowSurface_, nullptr);
	SDL_UpdateWindowSurface(window_);

	SDL_DestroySurface(newSurface);
}