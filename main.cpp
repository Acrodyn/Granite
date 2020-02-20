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
#include "GGame/MainShip.h"
#include "GGame/EnemyShip.h"

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

    float cameraSpeed = 850.f;
    float fYaw = 0.f;

    // TODO: Vj zelimo ucitat prototipe za odredjeni level i onda radit kopije od njih, ovo bi bilo preskupo
    MainShip ship("Great Fox.obj", "greatFox.png");
    EnemyShip ship2("Great Fox.obj", "greatFox.png");

    Granite::Camera camera;
    Granite::Camera::SetMainCamera(camera);

    const Uint8* state;

    while (true)
    {
        state = SDL_GetKeyboardState(NULL);
        endTime = SDL_GetTicks();
        milisecondsPassed = endTime - startTime;
        deltaTime = milisecondsPassed / 1000.f;
        startTime = endTime;

      /*  if (milisecondsPassed < milisecondsPerFrame)
        {
            SDL_Delay(milisecondsPerFrame - milisecondsPassed);
        }*/

       // printf("FPS is: %f \n", 1000.f / milisecondsPassed);

        Granite::GGraphics::ClearScreen(Granite::Color::Black);
        Granite::GGraphics::ClearDepthBuffer();

        Granite::GMath::FMatrix4x4 matWorld;
        matWorld.MakeIdentity();

        Granite::GMath::FVector3 vForward = Granite::Camera::GetMainCamera()->forward * (cameraSpeed * deltaTime);

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                if (Granite::GConfig::CAMERA_CONTROLS)
                {
                    if (e.key.keysym.sym == SDLK_1)
                    {
                        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 5.5f);
                    }

                    if (e.key.keysym.sym == SDLK_2)
                    {
                        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * 5.3f);
                    }

                    if (e.key.keysym.sym == SDLK_3)
                    {
                        matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * 5.4f);
                    }

                    if (e.key.keysym.sym == SDLK_UP)
                    {
                        Granite::Camera::GetMainCamera()->position.y -= cameraSpeed * deltaTime;
                    }

                    if (e.key.keysym.sym == SDLK_DOWN)
                    {
                        Granite::Camera::GetMainCamera()->position.y += cameraSpeed * deltaTime;
                    }

                    // ovo ne radi dobro zbog pukog x-a
                    if (e.key.keysym.sym == SDLK_LEFT)
                    {
                        Granite::Camera::GetMainCamera()->position.x -= cameraSpeed * deltaTime;
                    }

                    if (e.key.keysym.sym == SDLK_RIGHT)
                    {
                        Granite::Camera::GetMainCamera()->position.x += cameraSpeed * deltaTime;
                    }

                    if (e.key.keysym.sym == SDLK_a)
                    {
                        fYaw += 3.f * deltaTime;
                    }

                    if (e.key.keysym.sym == SDLK_d)
                    {
                        fYaw -= 3.f * deltaTime;
                    }

                    if (e.key.keysym.sym == SDLK_w)
                    {
                        Granite::Camera::GetMainCamera()->position += vForward;
                    }

                    if (e.key.keysym.sym == SDLK_s)
                    {
                        Granite::Camera::GetMainCamera()->position -= vForward;
                    }
                }
                else
                {
                   /* if (e.key.keysym.sym == SDLK_w)
                    {
                        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 18.5f);
                    }

                    if (e.key.keysym.sym == SDLK_s)
                    {
                        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * -18.5f);
                    }

                    if (e.key.keysym.sym == SDLK_a)
                    {
                        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * 8.5f);
                    }

                    if (e.key.keysym.sym == SDLK_d)
                    {
                        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * -8.5f);
                    }*/
                }
            }
        }

        if (state[SDL_SCANCODE_W])
        {
            matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 2.5f);
        }

        if (e.type == SDL_QUIT)
        {
            SDL_Log("Program quit after %i ticks", e.quit.timestamp);
            break;
        }

        Granite::Camera::GetMainCamera()->forward = Granite::GMath::MultiplyMatrixVector(Granite::GMath::GetForwardVector(), Granite::GMath::GetYRotation(fYaw));
        Granite::GMath::FMatrix4x4 matCamera = Granite::GMath::GetPointAtMatrix(Granite::Camera::GetMainCamera()->position, Granite::Camera::GetMainCamera()->GetTarget(), Granite::GMath::GetUpVector());
        Granite::Camera::matView = Granite::GMath::GetInverseMatrix(matCamera);

        ship.Update(deltaTime, state);
        ship2.Update(deltaTime, state);
        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}