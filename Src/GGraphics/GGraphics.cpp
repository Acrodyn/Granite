#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"
#include "GMath/Polygon.h"
#include "GMath/IPoint.h"
#include "SDL_image.h"

#define SDL_LOCKIFMUST(s) (SDL_MUSTLOCK(s) ? SDL_LockSurface(s) : 0)
#define SDL_UNLOCKIFMUST(s) { if(SDL_MUSTLOCK(s)) SDL_UnlockSurface(s); }

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
            IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

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

    // TODO: rewrite this or find the alternative
    int GGraphics::InvertSurfaceVertically(SDL_Surface* surface)
    {
        Uint8* t;
        register Uint8* a, * b;
        Uint8* last;
        register Uint16 pitch;

        if (SDL_LOCKIFMUST(surface) < 0)
            return -2;

        /* do nothing unless at least two lines */
        if (surface->h < 2) {
            SDL_UNLOCKIFMUST(surface);
            return 0;
        }

        /* get a place to store a line */
        pitch = surface->pitch;
        t = (Uint8*)malloc(pitch);

        if (t == NULL) {
            SDL_UNLOCKIFMUST(surface);
            return -2;
        }

        /* get first line; it's about to be trampled */
        memcpy(t, surface->pixels, pitch);

        /* now, shuffle the rest so it's almost correct */
        a = (Uint8*)surface->pixels;
        last = a + pitch * (surface->h - 1);
        b = last;

        while (a < b) {
            memcpy(a, b, pitch);
            a += pitch;
            memcpy(b, a, pitch);
            b -= pitch;
        }

        /* in this shuffled state, the bottom slice is too far down */
        memmove(b, b + pitch, last - b);

        /* now we can put back that first row--in the last place */
        memcpy(last, t, pitch);

        /* everything is in the right place; close up. */
        free(t);
        SDL_UNLOCKIFMUST(surface);

        return 0;
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

    Uint32 GGraphics::GetPixel(SDL_Surface* textureData, int x, int y)
    {
        int bpp = textureData->format->BytesPerPixel;
        Uint8* p = (Uint8*)textureData->pixels + y * textureData->pitch + x * bpp;

        switch (bpp) 
        {
        case 1:
            return *p;
        case 2:
            return *(Uint16*)p;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return p[0] << 16 | p[1] << 8 | p[2];
            else
                return p[0] | p[1] << 8 | p[2] << 16;
        case 4:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
                return *(Uint32*)p;
            else
                return *(Uint32*)p;
        default:
            return 0;
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

        int bpp = surface->format->BytesPerPixel;
        Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

        switch (bpp) 
        {
        case 1:
            *p = color;
            break;
        case 2:
            *(Uint16*)p = color;
            break;
        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                p[0] = (color >> 16) & 0xff;
                p[1] = (color >> 8) & 0xff;
                p[2] = color & 0xff;
            }
            else {
                p[0] = color & 0xff;
                p[1] = (color >> 8) & 0xff;
                p[2] = (color >> 16) & 0xff;
            }
            break;
        case 4:
            *(Uint32*)p = color;
            break;
        }
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

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, const GTexture* texture)
    {
        _RasterizeTriangle(polygon, (Uint32)Color::Grey, texture);
    }

    void GGraphics::RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color)
    {
        _RasterizeTriangle(polygon, color);
    }

    void GGraphics::_RasterizeTriangle(const GMath::Polygon& polygon, Uint32 color, const GTexture* texture)
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
            const float vertexSlice = (pv1->y - pv0->y) / (pv2->y - pv0->y);
            const GMath::FVector3 vi = *pv0 + (*pv2 - *pv0) * vertexSlice;

            //const float texelSlice = (tx1->y - tx0->y) / (pv2->y - pv0->y);
            const GMath::FVector3 ti = *tx0 + (*tx2 - *tx0) * vertexSlice;

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

    void GGraphics::_RasterizeFlatTopTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, const GTexture* texture)
    {
        // top rule
        int yStartCeil = (int)ceil(v0->y - .5f);
        int yEndCeil = (int)ceil(v2->y - .5f);
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil >= GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT - 1 : yEndCeil;

        float slope0 = (v2->x - v0->x) / (v2->y - v0->y);
        float slope1 = (v2->x - v1->x) / (v2->y - v1->y);

        const float depthStep = 1.f / (v2->y - v0->y); // modify?

        TextureCoordinates tL(tx0->x, tx0->y);
        TextureCoordinates tR(tx1->x, tx1->y);
        TextureCoordinates tB(tx2->x, tx2->y);

        TextureCoordinates texelStepL = (tB - tL) / (v2->y - v0->y);
        TextureCoordinates texelStepR = (tB - tR) / (v2->y - v1->y);

        // prestep
        float prestep = float(yStart) + .5f - v1->y;
        tL += (texelStepL * prestep);
        tR += (texelStepR * prestep);

        float zTracker = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0->x + slope0 * (float(y) + .5f - v0->y);
            const float px1 = v1->x + slope1 * (float(y) + .5f - v1->y);

            const float zL = GUtil::GInterpolate(v0->z, v1->z, zTracker);
            const float zR = GUtil::GInterpolate(v1->z, v2->z, zTracker);

            _Scanline(y, px0, px1, zL, zR, tL, tR, color, texture);

            zTracker += depthStep;

            tL += texelStepL;
            tR += texelStepR;
        }
    }

    void GGraphics::_RasterizeFlatBottomTriangle(const GMath::FVector3* v0, const GMath::FVector3* v1, const GMath::FVector3* v2, const GMath::FVector3* tx0, const GMath::FVector3* tx1, const GMath::FVector3* tx2, Uint32 color, const GTexture* texture)
    {
        // top rule
        int yStartCeil = (int)ceil(v0->y - .5f);
        int yEndCeil = (int)ceil(v2->y - .5f);
        const int yStart = (yStartCeil < 0) ? 0 : yStartCeil;
        const int yEnd = (yEndCeil >= GConfig::WINDOW_HEIGHT) ? GConfig::WINDOW_HEIGHT - 1: yEndCeil;

        float slope0 = (v1->x - v0->x) / (v1->y - v0->y);
        float slope1 = (v2->x - v0->x) / (v2->y - v0->y);

        const float stepZ = 1.f / (v1->y - v0->y);

        TextureCoordinates tTL(tx0->x, tx0->y);
        TextureCoordinates tTR(tx0->x, tx0->y);
        TextureCoordinates tBL(tx1->x, tx1->y);
        TextureCoordinates tBR(tx2->x, tx2->y);

        TextureCoordinates texelStepL = (tBL - tTL) / (v1->y - v0->y);
        TextureCoordinates texelStepR = (tBR - tTR) / (v2->y - v0->y);

        // prestep
        float prestep = float(yStart) + .5f - v0->y;
        tTL += (texelStepL * prestep);
        tTR += (texelStepR * prestep);

        float zTracker = 0.f;
        float texTrackerL = 0.f;
        float texTrackerR = 0.f;

        for (int y = yStart; y < yEnd; ++y)
        {
            const float px0 = v0->x + slope0 * (float(y) + .5f - v0->y);
            const float px1 = v0->x + slope1 * (float(y) + .5f - v0->y);

            const float zL = GUtil::GInterpolate(v0->z, v1->z, zTracker);
            const float zR = GUtil::GInterpolate(v0->z, v2->z, zTracker);

            /*const float tL = GUtil::GInterpolate(tx0->y, tx1->y, texTrackerL);
            const float tR = GUtil::GInterpolate(tx0->y, tx2->y, texTrackerR);*/

            _Scanline(y, px0, px1, zL, zR, tTL, tTR, color, texture);

            zTracker += stepZ;
            tTL += texelStepL;
            tTR += texelStepR;
        }
    }

    void GGraphics::_Scanline(int row, float firstPixel, float lastPixel, float firstPixelZ, float lastPixelZ, TextureCoordinates firstTexel, TextureCoordinates lastTexel, Uint32 color, const GTexture* texture)
    {
        int xStartCeil = (int)ceil(firstPixel - .5f);
        int xEndCeil = (int)ceil(lastPixel - .5f);
        const int xStart = (xStartCeil < 0) ? 0 : xStartCeil;
        const int xEnd = (xEndCeil >= GConfig::WINDOW_WIDTH) ? GConfig::WINDOW_WIDTH - 1: xEndCeil;

        float z = 0.f;
        float actualZTracker = 0.f;

        float stepZRight = 1.f / (xEnd - xStart);
        TextureCoordinates stepTexel = (lastTexel - firstTexel) / (xEnd - xStart);

        // prestep
        firstTexel += (stepTexel * ((float)xStart + .5f - xStart));

        for (int x = xStart; x < xEnd; ++x)
        {
            z = GUtil::GInterpolate(firstPixelZ, lastPixelZ, actualZTracker);

            if (z < depthBuffer[row * GConfig::WINDOW_WIDTH + x])
            {
                if (texture != nullptr)
                {
                    int texelPositionX = GUtil::Min((int)(firstTexel.u * texture->textureData->w), texture->textureData->w - 1);
                    int texelPositionY = GUtil::Min((int)(firstTexel.v * texture->textureData->h), texture->textureData->h - 1);
                    int texelPosition = (texelPositionY * texture->textureData->w) + texelPositionX;
                    Uint32 pixelData = GetPixel(texture->textureData, texelPositionX, texelPositionY);
                    Pixel pixel((Uint8)(pixelData), (Uint8)(pixelData >> 8), (Uint8)(pixelData >> 16));
                    color = _GetConvertedColor(pixel);
                    firstTexel += stepTexel;
                }
                depthBuffer[row * GConfig::WINDOW_WIDTH + x] = z;
                SetPixel(x, row, color);
            }

            actualZTracker += stepZRight;
        }
    }
}