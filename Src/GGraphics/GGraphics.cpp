#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"

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

    void GGraphics::UpdateScreen()
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

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Color color)
    {
        const GMath::FVector3 *pv0 = &polygon.vertices[0];
        const GMath::FVector3* pv1 = &polygon.vertices[1];
        const GMath::FVector3* pv2 = &polygon.vertices[2];

        if (pv1->y < pv0->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
        }
        if (pv2->y < pv1->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv1, &pv2);
        }
        if (pv1->y < pv0->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
        }

        if (pv0->y == pv1->y) // flat top
        {
            if (pv1->x < pv0->x)
            {
                GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
            }
            _RasterizeFlatTopTriangle(*pv0, *pv1, *pv2, color);
        }
        else if (pv1->y == pv2->y) // flat bottom
        {
            if (pv2->x < pv1->x)
            {
                GUtil::Swap<const GMath::FVector3>(&pv1, &pv2);
            }

            _RasterizeFlatBottomTriangle(*pv0, *pv1, *pv2, color);
        }
        else // non flat triangle
        {
            const float t = (pv1->y - pv0->y) / (pv2->y - pv0->y);
            const GMath::FVector3 vi = *pv0 + (*pv2 - *pv0) * t;

            if (pv1->x < vi.x) // major right
            {
                _RasterizeFlatBottomTriangle(*pv0, *pv1, vi, color);
                _RasterizeFlatTopTriangle(*pv1, vi, *pv2, color);
            }
            else // major left
            {
                _RasterizeFlatBottomTriangle(*pv0, vi, *pv1, color);
                _RasterizeFlatTopTriangle(vi, *pv1, *pv2, color);
            }
        } 
    }

    void GGraphics::_RasterizeFlatTopTriangle(const GMath::FVector3 &v0, const GMath::FVector3& v1, const GMath::FVector3& v2,  Color color)
    {
        float slope0 = (v2.x - v0.x) / (v2.y - v0.y);
        float slope1 = (v2.x - v1.x) / (v2.y - v1.y);

        // top rule
        const int yStart = (int)ceil(v0.y - .5f);
        const int yEnd = (int)ceil(v2.y - .5f);

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0.x + slope0 * (float(y) + .5f - v0.y);
            const float px1 = v1.x + slope1 * (float(y) + .5f - v1.y);

            // left rule
            const int xStart = (int)ceil(px0 - .5f);
            const int xEnd = (int)ceil(px1 - .5f);

            for (int x = xStart; x < xEnd; ++x)
            {
                SetPixel(x, y, color);
            }
        }
    }

    void GGraphics::_RasterizeFlatBottomTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Color color)
    {
        float slope0 = (v1.x - v0.x) / (v1.y - v0.y);
        float slope1 = (v2.x - v0.x) / (v2.y - v0.y);

        // top rule
        const int yStart = (int)ceil(v0.y - .5f);
        const int yEnd = (int)ceil(v2.y - .5f);

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0.x + slope0 * (float(y) + .5f - v0.y);
            const float px1 = v0.x + slope1 * (float(y) + .5f - v0.y);

            // left rule
            const int xStart = (int)ceil(px0 - .5f);
            const int xEnd = (int)ceil(px1 - .5f);

            for (int x = xStart; x < xEnd; ++x)
            {
                SetPixel(x, y, color);
            }
        }
    }
}