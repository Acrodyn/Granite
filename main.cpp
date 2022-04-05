#include "SDL2/SDL.h"
#include "GMath/GMath.h"
#include "GUtils/GUtil.h"
#include "GUtils/GConfig.h"
#include "GUtils/KeyHandler.h"
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
#include "GGame/SimpleGObject.h"

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

    float cameraSpeed = 1.0f;
    float fYaw = 0.f;

    Granite::Camera camera;
    Granite::Camera::SetMainCamera(camera);

    SimpleGObject cottage("Cat/Cat.obj", "Cat/Cat_diffuse.jpg");
    cottage.SetRotationSpeed(1.f);

    while (true)
    {
        KeyHandler::State = SDL_GetKeyboardState(NULL);
        endTime = SDL_GetTicks();
        milisecondsPassed = endTime - startTime;
        deltaTime = milisecondsPassed / 1000.f;
        startTime = endTime;

        if (milisecondsPassed < milisecondsPerFrame)
        {
            SDL_Delay(milisecondsPerFrame - milisecondsPassed);
        }

       printf("FPS is: %f \n", 1000.f / milisecondsPassed);

        Granite::GGraphics::ClearScreen(Granite::Color::Black);
        Granite::GGraphics::ClearDepthBuffer();

        Granite::GMath::FVector3 vForward = Granite::Camera::GetMainCamera()->forward * (cameraSpeed * deltaTime);


        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_KEYDOWN)
            {
                if (Granite::GConfig::CAMERA_CONTROLS)
                {
                    //if (e.key.keysym.sym == SDLK_1)
                    //{
                    //    matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 5.5f);
                    //}

                    //if (e.key.keysym.sym == SDLK_2)
                    //{
                    //    matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * 5.3f);
                    //}

                    //if (e.key.keysym.sym == SDLK_3)
                    //{
                    //    matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * 5.4f);
                    //}

                    //if (e.key.keysym.sym == SDLK_UP)
                    //{
                    //    Granite::Camera::GetMainCamera()->position.y -= cameraSpeed * deltaTime;
                    //}

                    //if (e.key.keysym.sym == SDLK_DOWN)
                    //{
                    //    Granite::Camera::GetMainCamera()->position.y += cameraSpeed * deltaTime;
                    //}

                    //// ovo ne radi dobro zbog pukog x-a
                    //if (e.key.keysym.sym == SDLK_LEFT)
                    //{
                    //    Granite::Camera::GetMainCamera()->position.x -= cameraSpeed * deltaTime;
                    //}

                    //if (e.key.keysym.sym == SDLK_RIGHT)
                    //{
                    //    Granite::Camera::GetMainCamera()->position.x += cameraSpeed * deltaTime;
                    //}

                    //if (e.key.keysym.sym == SDLK_a)
                    //{
                    //    fYaw += 3.f * deltaTime;
                    //}

                    //if (e.key.keysym.sym == SDLK_d)
                    //{
                    //    fYaw -= 3.f * deltaTime;
                    //}

                    //if (e.key.keysym.sym == SDLK_w)
                    //{
                    //    Granite::Camera::GetMainCamera()->position += vForward;
                    //}

                    //if (e.key.keysym.sym == SDLK_s)
                    //{
                    //    Granite::Camera::GetMainCamera()->position -= vForward;
                    //}
                }
            }
        }

        if (e.type == SDL_QUIT)
        {
            SDL_Log("Program quit after %i ticks", e.quit.timestamp);
            break;
        }

        Granite::Camera::GetMainCamera()->forward = Granite::GMath::MultiplyMatrixVector(Granite::GMath::GetForwardVector(), Granite::GMath::GetYRotation(fYaw));
        Granite::GMath::FMatrix4x4 matCamera = Granite::GMath::GetPointAtMatrix(Granite::Camera::GetMainCamera()->position, Granite::Camera::GetMainCamera()->GetTarget(), Granite::GMath::GetUpVector());
        Granite::Camera::matView = Granite::GMath::GetInverseMatrix(matCamera);

        cottage.Update(deltaTime);
        Granite::GGraphics::UpdateScreen();
    }

    Granite::GGraphics::DestroyGraphics();
    return 0;
}