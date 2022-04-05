#include "GGame/SimpleGObject.h"
#include "GUtils/KeyHandler.h"

SimpleGObject::SimpleGObject(std::string model, std::string texture) : GObject(model, texture)
{
    this->SetupView();
}

void SimpleGObject::Update(float deltaTime)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    Rotate(matWorld, deltaTime);

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}

void SimpleGObject::SetRotationSpeed(float rotationSpeed)
{
    _rotationSpeed = rotationSpeed;
}

void SimpleGObject::SetupView()
{
    mesh.MeshDepth = 55.f;
    mesh.MeshScale = 0.2f;
    mesh.MeshInitialPosOffset = 2.f;
}

void SimpleGObject::Rotate(Granite::GMath::FMatrix4x4& transform, float deltaTime)
{
    if (KeyHandler::State[SDL_SCANCODE_W])
    {
        transform = transform * Granite::GMath::GetXRotation(deltaTime * _rotationSpeed);
        //mesh.MeshPosY -= _positionSpeed;
    }

    if (KeyHandler::State[SDL_SCANCODE_S])
    {
        transform = transform * Granite::GMath::GetXRotation(deltaTime * -_rotationSpeed);
        //mesh.MeshPosY += _positionSpeed;
    }

    if (KeyHandler::State[SDL_SCANCODE_A])
    {
        transform = transform * Granite::GMath::GetYRotation(deltaTime * _rotationSpeed);
       // mesh.MeshPosX -= _positionSpeed;
    }

    if (KeyHandler::State[SDL_SCANCODE_D])
    {
        transform = transform * Granite::GMath::GetYRotation(deltaTime * -_rotationSpeed);
        //mesh.MeshPosX += _positionSpeed;
    }

    if (KeyHandler::State[SDL_SCANCODE_Q])
    {
        transform = transform * Granite::GMath::GetZRotation(deltaTime * -_rotationSpeed * 1.5f);
    }

    if (KeyHandler::State[SDL_SCANCODE_E])
    {
        transform = transform * Granite::GMath::GetZRotation(deltaTime * _rotationSpeed * 1.5f);
    }
}