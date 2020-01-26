#pragma once

#include "SDL.h"
#include "GMath/GMath.h"
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
        static void UpdateScreen();

        static GMath::FMatrix4x4 GetProjectionMatrix();
        static void ClearScreen(Color clearColor);
        static void SetPixel(int x, int y, Color color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Color color);

        static enum Color GetRandomColor()
        {
            static std::vector<Color> colorPool = { Color::Red, Color::Green, Color::Blue, Color::White};
            std::random_shuffle(colorPool.begin(), colorPool.end());
            return colorPool.at(0);
        }

    private:
        static SDL_Surface* surface;
        static SDL_Window* window;

        static void _RasterizeFlatTopTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Color color);
        static void _RasterizeFlatBottomTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Color color);
    };
}