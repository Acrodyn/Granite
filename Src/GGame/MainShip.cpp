#include "GGame/MainShip.h"
#include "GMath/GMath.h"

MainShip::MainShip(std::string model, std::string texture) : GObject(model, texture), _moveSpeed(3.f)
{
    this->SetupView();
}

void MainShip::Update(float deltaTime, const Uint8* state)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    float Yaw, Pitch, Roll;
    mesh.Transformation->GetRotation(Yaw, Pitch, Roll);
    Yaw = Granite::GMath::RadiansToDegrees(Yaw);
    Pitch = Granite::GMath::RadiansToDegrees(Pitch);
    Roll = Granite::GMath::RadiansToDegrees(Roll);

    std::printf("%f %f %f \n", Yaw, Pitch, Roll);

    if (state[SDL_SCANCODE_W])
    {
        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * _moveSpeed);
    }

    if (state[SDL_SCANCODE_S])
    {
        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * -_moveSpeed);
    }

    if (state[SDL_SCANCODE_A])
    {
        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * _moveSpeed);
    }

    if (state[SDL_SCANCODE_D])
    {
        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * -_moveSpeed);
    }

    if (state[SDL_SCANCODE_Q])
    {
        matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * -_moveSpeed);
    }

    if (state[SDL_SCANCODE_E])
    {
        matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * _moveSpeed);
    }

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}

void MainShip::SetupView()
{
    mesh.Transform(Granite::GMath::GetXRotation(Granite::GMath::DegreesToRadians(180.f)));
    mesh.Transform(Granite::GMath::GetYRotation(Granite::GMath::DegreesToRadians(180.f)));

    mesh.MeshDepth = 5.f;
    mesh.MeshScale = 0.5f;
    mesh.MeshInitialPosOffset = 1.f;
}