#include "SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GUtil.h"
#include "GUtils/GConfig.h"
#include "GGraphics/GModelLoader.h"
#include "GGraphics/GGraphics.h"
#include "GGraphics/GTexture.h"
#include "GMath/Mesh.h"
#include "GMath/IPoint.h"
#include "GMath/FMatrix4x4.h"
#include "GMath/FVector3.h"
#include "GMath/Polygon.h"
#include "GGraphics/Colors.h"
#include "GThread/ThreadManager.h"

int main(int argc, char* argv[])
{
    Granite::GGraphics::InitGraphics();

    SDL_Event e;

    float milisecondsPassed = 0;
    float deltaTime = 0;
    float startTime = 0;
    float endTime = 0;
    float milisecondsPerFrame = 16.f;
    float fps = 0;
    bool rotateX, rotateY, rotateZ;

    Granite::GMath::Mesh mesh("teapot.obj");
    mesh.TransformPolygons(Granite::GMath::GetXRotation(Granite::GMath::AnglesToRadians(180.f)));

    while (true)
    {
        rotateX = false;
        rotateY = false;
        rotateZ = false;

        endTime = SDL_GetTicks();
        milisecondsPassed = endTime - startTime;
        deltaTime = milisecondsPassed / 1000.f;
        startTime = endTime;

       /* if (milisecondsPassed < milisecondsPerFrame)
        {
            SDL_Delay(milisecondsPerFrame - milisecondsPassed);
        }*/

        if (milisecondsPassed > milisecondsPerFrame) {
            printf("FPS is: %f \n", 1000.f / milisecondsPassed);
        }

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_q)
                {
                    rotateX = true;
                }

                if (e.key.keysym.sym == SDLK_w)
                {
                    rotateY = true;
                }

                if (e.key.keysym.sym == SDLK_e)
                {
                    rotateZ = true;
                }
            }
        }

        if (e.type == SDL_QUIT)
        {
            SDL_Log("Program quit after %i ticks", e.quit.timestamp);
            break;
        }

        Granite::GGraphics::ClearScreen(Granite::Color::Black);
        Granite::GGraphics::ClearDepthBuffer();

        // Set up rotation matrices
        Granite::GMath::FMatrix4x4 transformMatrix;

        if (rotateX)
        {
            transformMatrix = transformMatrix * Granite::GMath::GetXRotation(deltaTime * 5.5f);
        }

        if (rotateY)
        {
            transformMatrix = transformMatrix * Granite::GMath::GetYRotation(deltaTime * 5.3f);
        }

        if (rotateZ)
        {
            transformMatrix = transformMatrix * Granite::GMath::GetZRotation(deltaTime * 5.4f);
        }

        mesh.TransformPolygons(transformMatrix);
        mesh.RasterizePolygons();
        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}