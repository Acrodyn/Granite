#include "SDL.h"
#include "GMath.h"
#include "Util.h"
#include "ModelLoader.h"

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

namespace Granite
{
    enum class Color {
        Red = 0xFF0000,
        Green = 0x00FF00,
        Blue = 0x0000FF,
        White = 0xFFFFFF,
        Black = 0x000000,
    };

    FMatrix4x4 GetProjectionMatrix()
    {
        FMatrix4x4 projectionMatrix;

        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f;
        float fAspectRatio = (float)WINDOW_HEIGHT / (float)WINDOW_WIDTH;
        float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

        projectionMatrix(0, 0) = fAspectRatio * fFovRad;
        projectionMatrix(1, 1) = fFovRad;
        projectionMatrix(2, 2) = fFar / (fFar - fNear);
        projectionMatrix(3, 2) = (-fFar * fNear) / (fFar - fNear);
        projectionMatrix(2, 3) = 1.0f;
        projectionMatrix(3, 3) = 0.0f;

        return projectionMatrix;
    }

    Mesh GetMesh()
    {
        Mesh mesh;

        mesh.polygonVertices = {

            // SOUTH
            { 0.0f, 0.0f, 0.0f,    0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 0.0f, 0.0f },

            // EAST                                                      
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 0.0f, 1.0f },

            // NORTH                                                     
            { 1.0f, 0.0f, 1.0f,    1.0f, 1.0f, 1.0f,    0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 0.0f, 1.0f },

            // WEST                                                      
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 1.0f,    0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,    0.0f, 1.0f, 0.0f,    0.0f, 0.0f, 0.0f },

            // TOP                                                       
            { 0.0f, 1.0f, 0.0f,    0.0f, 1.0f, 1.0f,    1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,    1.0f, 1.0f, 1.0f,    1.0f, 1.0f, 0.0f },

            // BOTTOM                                                    
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,    0.0f, 0.0f, 0.0f,    1.0f, 0.0f, 0.0f },

        };

        return mesh;
    }

    void ClearScreen(SDL_Surface *surface, Color clearColor)
    {
        Uint32* pixels = (Uint32*)surface->pixels;
        for (int i = 0, len = WINDOW_WIDTH * WINDOW_HEIGHT; i < len; ++i)
        {
            *pixels = (Uint32)clearColor;
            ++pixels;
        }
    }

    void SetPixel(SDL_Surface* surface, int x, int y, Color color)
    {
        if (x < 0 || x >= WINDOW_WIDTH || y < 0 || y >= WINDOW_HEIGHT)
            return;
        *((Uint32*)surface->pixels + (y * surface->w) + x) = (Uint32)color;
    }

    // Bresenham’s Line Drawing Algorithm
    void DrawLine(SDL_Surface* surface, IPoint startPosition, IPoint endPosition, Color color)
    {
        if (startPosition.x == endPosition.x && startPosition.y == endPosition.y)
        {
            SetPixel(surface, startPosition.x, startPosition.y, color);
            return;
        }

        bool steep = false;

        if (Abs(startPosition.x - endPosition.x) < Abs(startPosition.y - endPosition.y))
        {
            Swap(startPosition.x, startPosition.y);
            Swap(endPosition.x, endPosition.y);
            steep = true;
        }

        if (startPosition.x > endPosition.x)
        {
            Swap(startPosition.x, endPosition.x);
            Swap(startPosition.y, endPosition.y);
        }

        int dx = endPosition.x - startPosition.x;
        int dy = endPosition.y - startPosition.y;

        int deltaError = Abs(dy) << 2;
        int error = 0;

        int y = startPosition.y;

        for (int x = startPosition.x; x <= endPosition.x; ++x)
        {
            if (steep)
            {
                SetPixel(surface, y, x, color);
            }
            else
            {
                SetPixel(surface, x, y, color);
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

int main(int argc, char* argv[])
{
    SDL_Window* window = SDL_CreateWindow("An SDL2 window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Init(0);

    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_Event e;

    float milisecondsPassed = 0;
    float deltaTime = 0;
    float startTime = 0;
    float endTime = 0;
    float milisecondsPerFrame = 16.f;
    float fps = 0;

    float fTheta = .0f;
    Granite::Mesh test;
    Granite::Mesh cube = Granite::GetMesh();
    Granite::FMatrix4x4 projMatrix = Granite::GetProjectionMatrix();

    test = Granite::ModelLoader::Load();

    while (true)
    {
        endTime = SDL_GetTicks();
        milisecondsPassed = endTime - startTime;
        deltaTime = milisecondsPassed / 1000.f;
        startTime = endTime;

        if (milisecondsPassed < milisecondsPerFrame)
        {
            SDL_Delay(milisecondsPerFrame - milisecondsPassed);
        }

        if (milisecondsPassed > milisecondsPerFrame) {
            //printf("FPS is: %f \n", 1000.f / milisecondsPassed);
        }

        SDL_PollEvent(&e);
        if (e.type == SDL_QUIT) 
        {
            SDL_Log("Program quit after %i ticks", e.quit.timestamp);
            break;
        }

        Granite::ClearScreen(surface, Granite::Color::Black);

        // Set up rotation matrices
        Granite::FMatrix4x4 matRotZ, matRotX;
        fTheta += 1.0f * deltaTime;

        // Rotation Z
        matRotZ.matrix[0][0] = cosf(fTheta);
        matRotZ.matrix[0][1] = sinf(fTheta);
        matRotZ.matrix[1][0] = -sinf(fTheta);
        matRotZ.matrix[1][1] = cosf(fTheta);
        matRotZ.matrix[2][2] = 1;
        matRotZ.matrix[3][3] = 1;

        // Rotation X
        matRotX.matrix[0][0] = 1;
        matRotX.matrix[1][1] = cosf(fTheta * 0.5f);
        matRotX.matrix[1][2] = sinf(fTheta * 0.5f);
        matRotX.matrix[2][1] = -sinf(fTheta * 0.5f);
        matRotX.matrix[2][2] = cosf(fTheta * 0.5f);
        matRotX.matrix[3][3] = 1;

        for (auto tri : test.polygonVertices)
        {
            Granite::Triangle triProjected, triTranslated, triRotatedZ, triRotatedZX;

            // Rotate in Z-Axis
            triRotatedZ.vertices[0] = MultiplyMatrixVector(tri.vertices[0], matRotZ);
            triRotatedZ.vertices[1] = MultiplyMatrixVector(tri.vertices[1], matRotZ);
            triRotatedZ.vertices[2] = MultiplyMatrixVector(tri.vertices[2], matRotZ);

             // Rotate in X-Axis
            triRotatedZX.vertices[0] = MultiplyMatrixVector(triRotatedZ.vertices[0], matRotX);
            triRotatedZX.vertices[1] = MultiplyMatrixVector(triRotatedZ.vertices[1], matRotX);
            triRotatedZX.vertices[2] = MultiplyMatrixVector(triRotatedZ.vertices[2], matRotX);

            // Offset into the screen
            triTranslated = triRotatedZX;
            triTranslated.vertices[0].z = triRotatedZX.vertices[0].z + 2000.f;
            triTranslated.vertices[1].z = triRotatedZX.vertices[1].z + 2000.f;
            triTranslated.vertices[2].z = triRotatedZX.vertices[2].z + 2000.f;

            // Project triangles from 3D --> 2D
            triProjected.vertices[0] = MultiplyMatrixVector(triTranslated.vertices[0], projMatrix);
            triProjected.vertices[1] = MultiplyMatrixVector(triTranslated.vertices[1], projMatrix);
            triProjected.vertices[2] = MultiplyMatrixVector(triTranslated.vertices[2], projMatrix);

            // Scale into view
            triProjected.vertices[0].x += 1.0f; 
            triProjected.vertices[0].y += 1.0f;
            triProjected.vertices[1].x += 1.0f; 
            triProjected.vertices[1].y += 1.0f;
            triProjected.vertices[2].x += 1.0f; 
            triProjected.vertices[2].y += 1.0f;
            triProjected.vertices[0].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.vertices[0].y *= 0.5f * (float)WINDOW_HEIGHT;
            triProjected.vertices[1].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.vertices[1].y *= 0.5f * (float)WINDOW_HEIGHT;
            triProjected.vertices[2].x *= 0.5f * (float)WINDOW_WIDTH;
            triProjected.vertices[2].y *= 0.5f * (float)WINDOW_HEIGHT;

            Granite::DrawLine(surface, 
                Granite::IPoint(triProjected.vertices[0].x, triProjected.vertices[0].y),
                Granite::IPoint(triProjected.vertices[1].x, triProjected.vertices[1].y),
                Granite::Color::Red);

            Granite::DrawLine(surface,
                Granite::IPoint(triProjected.vertices[1].x, triProjected.vertices[1].y),
                Granite::IPoint(triProjected.vertices[2].x, triProjected.vertices[2].y),
                Granite::Color::Red);

            Granite::DrawLine(surface,
                Granite::IPoint(triProjected.vertices[2].x, triProjected.vertices[2].y),
                Granite::IPoint(triProjected.vertices[0].x, triProjected.vertices[0].y),
                Granite::Color::Red);
        }

        SDL_UpdateWindowSurface(window);
    }

    // Close and destroy the window
    SDL_DestroyWindow(window);
    return 0;
}