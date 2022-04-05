#pragma once

#include "SDL2/SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GConfig.h"
#include "SDL2/SDL_image.h"

namespace Granite
{
	class GTexture
	{
	public:
		GTexture() = default;
		GTexture(const char* file)
		{
			LoadTexture(file);
		}
		~GTexture() = default;

		void LoadTexture(const char* file)
		{
			textureData = IMG_Load(file);
		}

		SDL_Surface* textureData;
	private:
	};
}