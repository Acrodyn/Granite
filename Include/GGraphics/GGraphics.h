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

        // its own class!
        class Pixel
        {
        public:
            Uint8 r = 0;
            Uint8 g = 0;
            Uint8 b = 0;
            Uint8 a = 255;

            Pixel(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255)
            {
                this->r = r;
                this->g = g;
                this->b = b;
                this->a = a;
            }

            void SetIntensity(float intensity)
            {
                // clamp from zero to one
                r *= intensity;
                g *= intensity;
                b *= intensity;
            }
        };

        static void InitGraphics();
        static void DestroyGraphics();
        static void UpdateScreen();

        static GMath::FMatrix4x4 GetProjectionMatrix();
        static void ClearScreen(Color clearColor);
        static void SetPixel(int x, int y, Pixel color);
        static void SetPixel(int x, int y, Color color);
        static void SetPixel(int x, int y, Uint32 color); // private, maybe?
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Pixel color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Uint32 color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Pixel color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Color color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color);

        static enum Color GetRandomColor()
        {
            static std::vector<Color> colorPool = { Color::Red, Color::Green, Color::Blue, Color::White};
            std::random_shuffle(colorPool.begin(), colorPool.end());
            return colorPool.at(0);
        }

        static Color GetGeneratedColor(unsigned int color)
        {
            return (Color)color;
        }

    private:
        static SDL_Surface* surface;
        static SDL_Window* window;

        static void _RasterizeFlatTopTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Uint32 color);
        static void _RasterizeFlatBottomTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Uint32 color);
       
        static Uint32 _GetConvertedColor(Color color)
        {
            return (Uint32)color;
        }

        static Uint32 _GetConvertedColor(Pixel color)
        {
            Uint32 convertedColor = 0x000000;
            convertedColor |= (color.a << 24);
            convertedColor |= (color.r << 16);
            convertedColor |= (color.g << 8);
            convertedColor |= (color.b);

            return convertedColor;
        }
    };
}