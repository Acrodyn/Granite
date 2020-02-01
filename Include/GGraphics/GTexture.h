#pragma once

#include "SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GConfig.h"
#include "SDL_image.h"

namespace Granite
{
	class GTexture
	{
	public:
		GTexture() = delete;
		GTexture(const char* file)
		{
			textureData = IMG_Load(file);
		}

		~GTexture() = default;

		SDL_Surface* textureData;
	private:
	};
}