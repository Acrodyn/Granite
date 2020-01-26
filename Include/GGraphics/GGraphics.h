#pragma once

#include "SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GUtil.h"
#include "GUtils/GConfig.h"
#include "GModelLoader.h"

namespace Granite
{
	class GGraphics
	{
    public:

        enum class Color {
            Red = 0xFF0000,
            Green = 0x00FF00,
            Blue = 0x0000FF,
            White = 0xFFFFFF,
            Black = 0x000000,
        };

        static void InitGraphics();
        static void DestroyGraphics();
        static void UpdateWindow();

        static GMath::FMatrix4x4 GetProjectionMatrix();
        static void ClearScreen(Color clearColor);
        static void SetPixel(int x, int y, Color color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color);

    private:
        static SDL_Surface* surface;
        static SDL_Window* window;
    };
}