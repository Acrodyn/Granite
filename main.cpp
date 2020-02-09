#include "SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GUtil.h"
#include "GUtils/GConfig.h"
#include "GGraphics/GModelLoader.h"
#include "GGraphics/GGraphics.h"
#include "GGraphics/GTexture.h"
#include "GGraphics/Camera.h"
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
    mesh.Transform(Granite::GMath::GetXRotation(Granite::GMath::AnglesToRadians(180.f)));

    Granite::Camera camera;
    Granite::Camera::SetMainCamera(camera);

    Granite::GMath::FVector3 lookDir = {0, 0, 1};
    Granite::GMath::FVector3 vUp = { 0, 1, 0 };

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

                if (e.key.keysym.sym == SDLK_UP)
                {
                    Granite::Camera::GetMainCamera()->position.y -= 20.0f * deltaTime;
                }

                if (e.key.keysym.sym == SDLK_DOWN)
                {
                    Granite::Camera::GetMainCamera()->position.y += 20.0f * deltaTime;
                }

                if (e.key.keysym.sym == SDLK_LEFT)
                {
                    Granite::Camera::GetMainCamera()->position.x -= 20.0f * deltaTime;
                }

                if (e.key.keysym.sym == SDLK_RIGHT)
                {
                    Granite::Camera::GetMainCamera()->position.x += 20.0f * deltaTime;
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

        Granite::GMath::FMatrix4x4 transformMatrix;
        transformMatrix.MakeIdentity();
       // transformMatrix = Granite::GMath::GetTranslationMatrix(0.f, 0.f, 5.f); // ovo moze komotno bit u samoj matrici!

        Granite::GMath::FMatrix4x4 matWorld;
        matWorld.MakeIdentity();

        // Set up rotation matrices
        Granite::GMath::FVector3 vTarget = Granite::Camera::GetMainCamera()->position + lookDir;
        Granite::GMath::FMatrix4x4 matCamera = Granite::GMath::GetPointAtMatrix(Granite::Camera::GetMainCamera()->position, vTarget, vUp);
        Granite::GMath::FMatrix4x4 matView = Granite::GMath::GetInverseMatrix(matCamera);

        if (rotateX)
        {
            matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 5.5f);
        }

        if (rotateY)
        {
            matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * 5.3f);
        }

        if (rotateZ)
        {
            matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * 5.4f);
        }

        matWorld = matWorld * transformMatrix;

        mesh.SetWorldSpaceTransform(matWorld);
        mesh.SetViewSpaceTransform(matView);
        mesh.Rasterize();
        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}