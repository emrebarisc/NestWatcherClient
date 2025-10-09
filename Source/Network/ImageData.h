#pragma once

#include "Managers/NetworkManager.h"

class alignas(sizeof(uint8_t*)) ImageData
{
public:
	ImageData() = default;
	~ImageData() = default;

	int rowIndex{ 0 };
	int sectionIndex{ 0 };
	uint8_t row[CAMERA_WIDTH]{ };
};
