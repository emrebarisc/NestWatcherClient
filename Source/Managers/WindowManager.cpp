#include "WindowManager.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_video.h>

WindowManager::WindowManager()
{

}

WindowManager::~WindowManager()
{
	SDL_DestroyWindow(window_);

	SDL_Quit();
}

void WindowManager::Init()
{
	SDL_Init(SDL_INIT_VIDEO);

	window_ = SDL_CreateWindow("Nest Watcher Client", 1600, 900, SDL_WINDOW_VULKAN);
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

		}
	}
}
