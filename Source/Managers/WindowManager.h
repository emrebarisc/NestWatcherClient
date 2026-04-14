#pragma once

#include "IManager.h"

class SDL_Window;
class SDL_Renderer;
class SDL_Texture;

class WindowManager : public IManager
{
public:
	WindowManager();
	~WindowManager();

	void Init() override;
	void Start() override;

	void PollEvent();

	void UpdateCameraImageSurface(unsigned char* imageRawData, int width, int height);

protected:

private:
	SDL_Window* window_{ nullptr };
	SDL_Renderer* renderer_{ nullptr };
	SDL_Texture* cameraTexture_{ nullptr };

	int currentTextureWidth_{ 0 };
	int currentTextureHeight_{ 0 };

	int windowWidth_{ 1600 };
	int windowHeight_{ 900 };
};