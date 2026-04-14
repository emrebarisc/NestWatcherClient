#include "WindowManager.h"

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_render.h>

#include "Program.h"

WindowManager::WindowManager()
{

}

WindowManager::~WindowManager()
{
	if (cameraTexture_) SDL_DestroyTexture(cameraTexture_);
	if (renderer_) SDL_DestroyRenderer(renderer_);
	if (window_) SDL_DestroyWindow(window_);

	SDL_Quit();
}

void WindowManager::Init()
{
	SDL_Init(SDL_INIT_VIDEO);

	// Remove SDL_WINDOW_VULKAN unless you are natively writing Vulkan code.
	// We use SDL_WINDOW_RESIZABLE so you can dynamically scale the camera view.
	window_ = SDL_CreateWindow("Nest Watcher Client", windowWidth_, windowHeight_, SDL_WINDOW_RESIZABLE);

	if (!window_)
	{
		std::cerr << "Could not create window: " << SDL_GetError() << std::endl;
		return;
	}

	// Create hardware-accelerated renderer
	renderer_ = SDL_CreateRenderer(window_, nullptr);
	if (!renderer_)
	{
		std::cerr << "Could not create renderer: " << SDL_GetError() << std::endl;
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

void WindowManager::UpdateCameraImageSurface(unsigned char* imageRawData, int width, int height)
{
	if (!renderer_)
	{
		return;
	}

	if (!cameraTexture_ || currentTextureWidth_ != width || currentTextureHeight_ != height)
	{
		if (cameraTexture_)
		{
			SDL_DestroyTexture(cameraTexture_);
		}

		cameraTexture_ = SDL_CreateTexture(
			renderer_,
			SDL_PIXELFORMAT_RGB24,
			SDL_TEXTUREACCESS_STREAMING,
			width,
			height
		);

		currentTextureWidth_ = width;
		currentTextureHeight_ = height;
	}

	if (cameraTexture_)
	{
		SDL_UpdateTexture(cameraTexture_, nullptr, imageRawData, width * 3);

		SDL_RenderClear(renderer_);
		SDL_RenderTexture(renderer_, cameraTexture_, nullptr, nullptr);
		SDL_RenderPresent(renderer_);
	}
}