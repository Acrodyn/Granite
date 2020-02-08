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

    float fTheta = .0f;
    Granite::GMath::Mesh mesh("Jess.obj", "JessTex.png");
    Granite::GMath::FMatrix4x4 projMatrix = Granite::GGraphics::GetProjectionMatrix();
    Granite::GMath::FVector3 camera;

    bool rotateX, rotateY, rotateZ;

    float xSpeed = 5.5f;
    float ySpeed = 5.3f;
    float zSpeed = 5.4f;

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
        Granite::GMath::FMatrix4x4 transformMatrix, matRotZ, matRotX, matRotY;
        fTheta += deltaTime;

        // dodaj rotacije u funkcije or something
        // Rotation Z
        matRotZ.data[0][0] = cosf(deltaTime * zSpeed);
        matRotZ.data[0][1] = sinf(deltaTime * zSpeed);
        matRotZ.data[1][0] = -sinf(deltaTime * zSpeed);
        matRotZ.data[1][1] = cosf(deltaTime * zSpeed);
        matRotZ.data[2][2] = 1;
        matRotZ.data[3][3] = 1;

        // Rotation Y
        matRotY.data[0][0] = cos(deltaTime * ySpeed);
        matRotY.data[0][2] = sinf(deltaTime * ySpeed);
        matRotY.data[1][1] = 1;
        matRotY.data[2][0] = -sinf(deltaTime * ySpeed);
        matRotY.data[2][2] = cosf(deltaTime * ySpeed);
        matRotY.data[3][3] = 1;

        // Rotation X
        matRotX.data[0][0] = 1;
        matRotX.data[1][1] = cosf(deltaTime * xSpeed);
        matRotX.data[1][2] = sinf(deltaTime * xSpeed);
        matRotX.data[2][1] = -sinf(deltaTime * xSpeed);
        matRotX.data[2][2] = cosf(deltaTime * xSpeed);
        matRotX.data[3][3] = 1;

        if (rotateZ)
        {
            transformMatrix = transformMatrix * matRotZ;
        }

        if (rotateX)
        {
            transformMatrix = transformMatrix * matRotX;
        }

        if (rotateY)
        {
            transformMatrix = transformMatrix * matRotY;
        }

        mesh.RasterizePolygons(transformMatrix);
        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}