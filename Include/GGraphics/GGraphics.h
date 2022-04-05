#pragma once

#include "SDL2/SDL.h"
#include "GGraphics/Colors.h"
#include "GMath/GMath.h"
#include "GUtils/GConfig.h"
#include "GModelLoader.h"
#include "GTexture.h"

namespace Granite
{
    namespace GMath
    {
        class Triangle;
        class IPoint;
    }

    class GGraphics
	{
    public:
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

        class TextureCoordinates
        {
        public:
            float u;
            float v;

            TextureCoordinates(float u, float v)
            {
                this->u = u;
                this->v = v;
            }

            TextureCoordinates& operator*=(float value)
            {
                u *= value;
                v *= value;

                return *this;
            }

            TextureCoordinates operator*(float value)
            {
                return TextureCoordinates(u * value, v * value);
            }

            TextureCoordinates& operator+=(float value)
            {
                u += value;
                v += value;

                return *this;
            }

            TextureCoordinates& operator+=(const TextureCoordinates& other)
            {
                u += other.u;
                v += other.v;

                return *this;
            }

            TextureCoordinates& operator-=(float value)
            {
                u -= value;
                v -= value;

                return *this;
            }

            TextureCoordinates operator-(const TextureCoordinates &other)
            {
                return TextureCoordinates(u - other.u, v - other.v);
            }

            TextureCoordinates& operator/=(float value)
            {
                u /= value;
                v /= value;

                return *this;
            }

            TextureCoordinates operator/(float value)
            {
                return TextureCoordinates(u / value, v / value);
            }
        };

        static void InitGraphics();
        static void DestroyGraphics();
        static void UpdateScreen();

        static int InvertSurfaceVertically(SDL_Surface* surface);
        static void ClearScreen(Color clearColor);
        static void ClearDepthBuffer();
        static Uint32 GetPixel(SDL_Surface* textureData, int x, int y);
        static void SetPixel(int x, int y, Pixel color);
        static void SetPixel(int x, int y, Color color);
        static void SetPixel(int x, int y, Uint32 color); // private, maybe?
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Pixel color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color);
        static void DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Uint32 color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Pixel color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Color color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color);
        static void RasterizeTriangle(const GMath::Polygon& polygon, const GTexture* texture = nullptr);

        //static enum Color GetRandomColor()
        //{
        //    static std::vector<Color> colorPool = { Color::Red, Color::Green, Color::Blue, Color::White};
        //    std::shuffle(colorPool.begin(), colorPool.end());
        //    return colorPool.at(0);
        //}

        static Color GetGeneratedColor(unsigned int color)
        {
            return (Color)color;
        }

        static SDL_Surface* surface;
    private:
        static SDL_Window* window;
        static float* depthBuffer;

        static void _RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color = 0, const GTexture* texture = nullptr);
        static void _RasterizeFlatTopTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, const GTexture* texture = nullptr);
        static void _RasterizeFlatBottomTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, const GTexture* texture = nullptr);
        static void _Scanline(int row, float firstPixel, float lastPixel, float firstPixelZ, float lastPixelZ, TextureCoordinates firstTexel, TextureCoordinates lastTexel, Uint32 color, const GTexture* texture = nullptr);
       
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