#include "GGraphics/GGraphics.h"

namespace Granite
{
	SDL_Surface* GGraphics::surface = nullptr;
	SDL_Window* GGraphics::window = nullptr;

    void GGraphics::InitGraphics()
    {
        static bool initiated;

        if (!initiated)
        {
            window = SDL_CreateWindow("An Asian girl renderer! mmmmm....", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, Granite::GConfig::WINDOW_WIDTH, Granite::GConfig::WINDOW_HEIGHT, 0);

            if (window == NULL) {
                throw "Window can't be created!";
            }

            SDL_Init(0);

            surface = SDL_GetWindowSurface(window);

            initiated = true;
        }
    }

    void GGraphics::DestroyGraphics()
    {
        SDL_DestroyWindow(window);
    }

    void GGraphics::UpdateWindow()
    {
        SDL_UpdateWindowSurface(window);
    }

    GMath::FMatrix4x4 GGraphics::GetProjectionMatrix()
    {
        GMath::FMatrix4x4 projectionMatrix;

        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)GConfig::WINDOW_HEIGHT / (float)GConfig::WINDOW_WIDTH;
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        projectionMatrix.matrix[0][0] = fAspectRatio * fFovRad;
        projectionMatrix.matrix[1][1] = fFovRad;
        projectionMatrix.matrix[2][2] = fFar / (fFar - fNear);
        projectionMatrix.matrix[3][2] = (-fFar * fNear) / (fFar - fNear);
        projectionMatrix.matrix[2][3] = 1.0f;
        projectionMatrix.matrix[3][3] = 0.0f;

        return projectionMatrix;
    }

    void GGraphics::ClearScreen(Color clearColor)
    {
        Uint32* pixels = (Uint32*)surface->pixels;
        for (int i = 0, len = GConfig::WINDOW_WIDTH * GConfig::WINDOW_HEIGHT; i < len; ++i)
        {
            *pixels = (Uint32)clearColor;
            ++pixels;
        }
    }

    void GGraphics::SetPixel(int x, int y, Color color)
    {
        if (x < 0 || x >= GConfig::WINDOW_WIDTH || y < 0 || y >= GConfig::WINDOW_HEIGHT)
        {
            return;
        }

        *((Uint32*)surface->pixels + (y * surface->w) + x) = (Uint32)color;
    }

    // Bresenham’s Line Drawing Algorithm
    void GGraphics::DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color)
    {
        if (startPosition.x == endPosition.x && startPosition.y == endPosition.y)
        {
            SetPixel(startPosition.x, startPosition.y, color);
            return;
        }

        bool steep = false;

        if (GUtil::Abs(startPosition.x - endPosition.x) < GUtil::Abs(startPosition.y - endPosition.y))
        {
            GUtil::Swap(startPosition.x, startPosition.y);
            GUtil::Swap(endPosition.x, endPosition.y);
            steep = true;
        }

        if (startPosition.x > endPosition.x)
        {
            GUtil::Swap(startPosition.x, endPosition.x);
            GUtil::Swap(startPosition.y, endPosition.y);
        }

        int dx = endPosition.x - startPosition.x;
        int dy = endPosition.y - startPosition.y;

        int deltaError = GUtil::Abs(dy) << 2;
        int error = 0;

        int y = startPosition.y;

        for (int x = startPosition.x; x <= endPosition.x; ++x)
        {
            if (steep)
            {
                SetPixel(y, x, color);
            }
            else
            {
                SetPixel(x, y, color);
            }

            error += deltaError;

            if (error > dx)
            {
                y += (endPosition.y > startPosition.y) ? 1 : -1;
                error -= dx << 2;
            }
        }
    }
}