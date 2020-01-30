#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"

namespace Granite
{
	SDL_Window* GGraphics::window = nullptr;
	SDL_Surface* GGraphics::surface = nullptr;
    float* GGraphics::depthBuffer = nullptr;

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

    void GGraphics::ClearDepthBuffer()
    {
        if (depthBuffer != nullptr)
        {
            delete [] depthBuffer;
            depthBuffer = nullptr;
        }

        int size = GConfig::WINDOW_WIDTH * GConfig::WINDOW_HEIGHT;
        depthBuffer = new float[size];

        for (int i = 0; i < size; ++i)
        {
            depthBuffer[i] = GUtil::MAX_FLOAT;
        }
    }

    void GGraphics::SetPixel(int x, int y, Color color)
    {
        SetPixel(x, y, (Uint32)color);
    }

    void GGraphics::SetPixel(int x, int y, Pixel color)
    {
        SetPixel(x, y, _GetConvertedColor(color));
    }

    void GGraphics::SetPixel(int x, int y, Uint32 color)
    {
        if (x < 0 || x >= GConfig::WINDOW_WIDTH || y < 0 || y >= GConfig::WINDOW_HEIGHT)
        {
            return;
        }

        // Uint8 alpha = ((color & 0xFF000000) >> 24); not currently relevant

        *((Uint32*)surface->pixels + (y * surface->w) + x) = (Uint32)color;
    }

    // Bresenham’s Line Drawing Algorithm
    void GGraphics::DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Pixel color)
    {
        DrawLine(startPosition, endPosition, _GetConvertedColor(color));
    }

    void GGraphics::DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Color color)
    {
        DrawLine(startPosition, endPosition, (Uint32)color);
    }

    void GGraphics::DrawLine(GMath::IPoint startPosition, GMath::IPoint endPosition, Uint32 color)
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

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Pixel color)
    {
        RasterizeTriangle(polygon, _GetConvertedColor(color));
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Color color)
    {
        RasterizeTriangle(polygon, (Uint32)color);
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color)
    {
        const GMath::FVector3 *pv0 = &polygon.vertices[0];
        const GMath::FVector3* pv1 = &polygon.vertices[1];
        const GMath::FVector3* pv2 = &polygon.vertices[2];

        // kad vec imash polygon, nadji teksturu umjesto colora ;) 

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

    void GGraphics::_RasterizeFlatTopTriangle(const GMath::FVector3 &v0, const GMath::FVector3& v1, const GMath::FVector3& v2,  Uint32 color)
    {
        float slope0 = (v2.x - v0.x) / (v2.y - v0.y);
        float slope1 = (v2.x - v1.x) / (v2.y - v1.y);

        const float stepZLeft = 1.f / (v2.y - v0.y);
        const float stepZRight = 1.f / (v2.y - v1.y);

        // top rule
        int yStartCeil = (int)ceil(v0.y - .5f);
        int yEndCeil = (int)ceil(v2.y - .5f);;
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil > GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT : yEndCeil;

        float zTrackerL = 0.f;
        float zTrackerR = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0.x + slope0 * (float(y) + .5f - v0.y);
            const float px1 = v1.x + slope1 * (float(y) + .5f - v1.y);

            const float zL = v0.z * (1.f - zTrackerL) + v1.z * zTrackerL;
            const float zR = v1.z * (1.f - zTrackerL) + v2.z * zTrackerL;

            // left rule
            int xStartCeil = (int)ceil(px0 - .5f);
            int xEndCeil = (int)ceil(px1 - .5f);
            const int xStart = (xStartCeil < 0) ? 0 : xStartCeil;
            const int xEnd = (xEndCeil > GConfig::WINDOW_WIDTH) ? GConfig::WINDOW_WIDTH : xEndCeil;

            float z = 0;
            float actualZTracker = 0.f;
            float stepZRight = 1.f / (px1 - px0);

            for (int x = xStart; x < xEnd; ++x)
            {
                z = zL * (1.f - actualZTracker) + zR * actualZTracker;

                if (z < depthBuffer[y * GConfig::WINDOW_WIDTH + x])
                {
                    depthBuffer[y * GConfig::WINDOW_WIDTH + x] = z;
                    SetPixel(x, y, color);
                }

                actualZTracker += stepZRight;
            }

            zTrackerL += stepZLeft;
            zTrackerR += stepZRight;
        }
    }

    void GGraphics::_RasterizeFlatBottomTriangle(const GMath::FVector3& v0, const GMath::FVector3& v1, const GMath::FVector3& v2, Uint32 color)
    {
        float slope0 = (v1.x - v0.x) / (v1.y - v0.y);
        float slope1 = (v2.x - v0.x) / (v2.y - v0.y);

        const float stepZLeft = 1.f / (v1.y - v0.y);
        const float stepZRight = 1.f / (v2.y - v0.y);

        // top rule
        int yStartCeil = (int)ceil(v0.y - .5f);
        int yEndCeil = (int)ceil(v2.y - .5f);;
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil > GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT : yEndCeil;

        float zTrackerL = 0.f;
        float zTrackerR = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0.x + slope0 * (float(y) + .5f - v0.y);
            const float px1 = v0.x + slope1 * (float(y) + .5f - v0.y);

            const float zL = v0.z * (1.f - zTrackerL) + v1.z * zTrackerL;
            const float zR = v0.z * (1.f - zTrackerL) + v2.z * zTrackerL;

            // left rule
            int xStartCeil = (int)ceil(px0 - .5f);
            int xEndCeil = (int)ceil(px1 - .5f);
            const int xStart = (xStartCeil < 0) ? 0 : xStartCeil;
            const int xEnd = (xEndCeil > GConfig::WINDOW_WIDTH) ? GConfig::WINDOW_WIDTH : xEndCeil;

            float z = 0;
            float actualZTracker = 0.f;
            float stepZRight = 1.f / (px1 - px0);

            for (int x = xStart; x < xEnd; ++x)
            {
                z = zL * (1.f - actualZTracker) + zR * actualZTracker;

                if (z < depthBuffer[y * GConfig::WINDOW_WIDTH + x])
                {
                    depthBuffer[y * GConfig::WINDOW_WIDTH + x] = z;
                    SetPixel(x, y, color);
                }
               
                actualZTracker += stepZRight;
            }

            zTrackerL += stepZLeft;
            zTrackerR += stepZRight;
        }
    }
}