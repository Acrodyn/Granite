#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"
#include "SDL_image.h"

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
            IMG_Init(IMG_INIT_PNG);

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
        _RasterizeTriangle(polygon, _GetConvertedColor(color));
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Color color)
    {
        _RasterizeTriangle(polygon, (Uint32)color);
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, GTexture* texture)
    {
        _RasterizeTriangle(polygon, (Uint32)Color::Grey, texture);
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color)
    {
        _RasterizeTriangle(polygon, color);
    }

    void GGraphics::_RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color, GTexture* texture)
    {
        const GMath::FVector3 *pv0 = &polygon.vertices[0];
        const GMath::FVector3* pv1 = &polygon.vertices[1];
        const GMath::FVector3* pv2 = &polygon.vertices[2];

        const GMath::FVector3* tx0 = &polygon.textureCoordinates[0];
        const GMath::FVector3* tx1 = &polygon.textureCoordinates[1];
        const GMath::FVector3* tx2 = &polygon.textureCoordinates[2];

        if (pv1->y < pv0->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
            GUtil::Swap<const GMath::FVector3>(&tx0, &tx1);
        }
        if (pv2->y < pv1->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv1, &pv2);
            GUtil::Swap<const GMath::FVector3>(&tx1, &tx2);
        }
        if (pv1->y < pv0->y)
        {
            GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
            GUtil::Swap<const GMath::FVector3>(&tx0, &tx1);
        }

        if (pv0->y == pv1->y) // flat top
        {
            if (pv1->x < pv0->x)
            {
                GUtil::Swap<const GMath::FVector3>(&pv0, &pv1);
                GUtil::Swap<const GMath::FVector3>(&tx0, &tx1);
            }

            _RasterizeFlatTopTriangle(pv0, pv1, pv2, tx0, tx1, tx2, color, texture);
        }
        else if (pv1->y == pv2->y) // flat bottom
        {
            if (pv2->x < pv1->x)
            {
                GUtil::Swap<const GMath::FVector3>(&pv1, &pv2);
                GUtil::Swap<const GMath::FVector3>(&tx1, &tx2);
            }

            _RasterizeFlatBottomTriangle(pv0, pv1, pv2, tx0, tx1, tx2, color, texture);
        }
        else // non flat triangle
        {
            const float vertexSlice = (pv1->y - pv0->y) / GUtil::Abs((pv2->y - pv0->y));
            const GMath::FVector3 vi = *pv0 + (*pv2 - *pv0) * vertexSlice;

            const float texelSlice = (tx1->y - tx0->y) / GUtil::Abs((pv2->y - pv0->y));
            const GMath::FVector3 ti = *tx0 + (*tx2 - *tx0) * texelSlice;

            if (pv1->x < vi.x) // major right
            {
                _RasterizeFlatTopTriangle(pv1, &vi, pv2, tx1, &ti, tx2, color, texture);
                _RasterizeFlatBottomTriangle(pv0, pv1, &vi, tx0, tx1, &ti, color, texture);
            }
            else // major left
            {
                _RasterizeFlatTopTriangle(&vi, pv1, pv2, &ti, tx1, tx2, color, texture);
                _RasterizeFlatBottomTriangle(pv0, &vi, pv1, tx0, &ti, tx1, color, texture);
            }
        } 
    }

    void GGraphics::_RasterizeFlatTopTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, GTexture* texture)
    {
        float slope0 = (v2->x - v0->x) / (v2->y - v0->y);
        float slope1 = (v2->x - v1->x) / (v2->y - v1->y);

        const float depthStep = 1.f / (v2->y - v0->y); // modify?
        const float texelStepL = (tx2->y - tx0->y) / (v2->y - v0->y);
        const float texelStepR = (tx2->y - tx1->y) / (v2->y - v1->y);

        // top rule
        int yStartCeil = (int)ceil(v0->y - .5f);
        int yEndCeil = (int)ceil(v2->y - .5f);;
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil > GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT : yEndCeil;

        float zTracker = 0.f;
        float texTrackerL = 0.f;
        float texTrackerR = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0->x + slope0 * (float(y) + .5f - v0->y);
            const float px1 = v1->x + slope1 * (float(y) + .5f - v1->y);

            const float zL = GUtil::GInterpolate(v0->z, v1->z, zTracker);
            const float zR = GUtil::GInterpolate(v1->z, v2->z, zTracker);

            const float tL = GUtil::GInterpolate(tx0->y, tx2->y, texelStepL);
            const float tR = GUtil::GInterpolate(tx1->y, tx2->y, texelStepR);

            _Scanline(y, px0, px1, zL, zR, tL, tR, color, texture);

            zTracker += depthStep;
            texTrackerL += texelStepL;
            texTrackerR += texelStepR;
        }
    }

    void GGraphics::_RasterizeFlatBottomTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, GTexture* texture)
    {
        float slope0 = (v1->x - v0->x) / (v1->y - v0->y);
        float slope1 = (v2->x - v0->x) / (v2->y - v0->y);

        const float stepZ = 1.f / (v1->y - v0->y);

        const float stepTexelL = (tx1->y - tx0->y) / (v1->y - v0->y);
        const float stepTexelR = (tx2->y - tx0->y) / (v2->y - v0->y);

        // top rule
        int yStartCeil = (int)ceil(v0->y - .5f);
        int yEndCeil = (int)ceil(v2->y - .5f);;
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil > GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT : yEndCeil;

        float zTracker = 0.f;
        float texTrackerL = 0.f;
        float texTrackerR = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0->x + slope0 * (float(y) + .5f - v0->y);
            const float px1 = v0->x + slope1 * (float(y) + .5f - v0->y);

            const float zL = GUtil::GInterpolate(v0->z, v1->z, zTracker);
            const float zR = GUtil::GInterpolate(v0->z, v2->z, zTracker);

            const float tL = GUtil::GInterpolate(tx0->y, tx1->y, texTrackerL);
            const float tR = GUtil::GInterpolate(tx0->y, tx2->y, texTrackerR);

            _Scanline(y, px0, px1, zL, zR, tL, tR, color, texture);

            zTracker += stepZ;
            texTrackerL += stepTexelL;
            texTrackerR += stepTexelR;
        }
    }

    void GGraphics::_Scanline(int row, float firstPixel, float lastPixel, float firstPixelZ, float lastPixelZ, float firstTexel, float lastTexel, Uint32 color, GTexture* texture)
    {
        int xStartCeil = (int)ceil(firstPixel - .5f);
        int xEndCeil = (int)ceil(lastPixel - .5f);
        const int xStart = (xStartCeil < 0) ? 0 : xStartCeil;
        const int xEnd = (xEndCeil > GConfig::WINDOW_WIDTH) ? GConfig::WINDOW_WIDTH : xEndCeil;

        float z = 0.f;
        float actualZTracker = 0.f;
        float texTracker = 0.f;

        float stepZRight = 1.f / (lastPixel - firstPixel);
        float stepTexel = (lastTexel - firstTexel) / (lastPixel - firstPixel);

        float tex = 0.f;

        for (int x = xStart; x < xEnd; ++x)
        {
            z = GUtil::GInterpolate(firstPixelZ, lastPixelZ, actualZTracker);

            if (texture != nullptr)
            {
                tex = GUtil::GInterpolate(firstTexel, lastTexel, texTracker);
                int position = tex * texture->textureData->w * texture->textureData->h;

                color = *((Uint32*)texture->textureData->pixels + position);
                texTracker += stepTexel;
            }

            if (z < depthBuffer[row * GConfig::WINDOW_WIDTH + x])
            {
                depthBuffer[row * GConfig::WINDOW_WIDTH + x] = z;
                SetPixel(x, row, color);
            }

            actualZTracker += stepZRight;
        }
    }
}