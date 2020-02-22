#include "GGame/MainShip.h"
#include "GMath/GMath.h"
#include "GUtils/KeyHandler.h"

MainShip::MainShip(std::string model, std::string texture) : GObject(model, texture), 
        _moveSpeed(2.f), 
        _yawCap(10.f), 
        _pitchCap(10.f),
        _positionSpeed(3.1f),
        _adjustmentSpeed(5.f)
{
    this->SetupView();
    defaultState.MakeIdentity();
}

void MainShip::Update(float deltaTime)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    if (!_UpdateBasicMovement(matWorld, deltaTime))
    {
        if (!mesh.Transformation->IsIdentity());
        {
            _StabilizeShip(matWorld, deltaTime);
        }
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
    mesh.MeshScale = 0.4f;
    mesh.MeshInitialPosOffset = 1.2f;
}

bool MainShip::_UpdateBasicMovement(Granite::GMath::FMatrix4x4& transform, float deltaTime)
{
    bool updatedMovement = false;

    float Yaw, Pitch, Roll;
    mesh.Transformation->GetRotation(Yaw, Pitch, Roll);
    Yaw = Granite::GMath::RadiansToDegrees(Yaw);
    Pitch = Granite::GMath::RadiansToDegrees(Pitch);
    Roll = Granite::GMath::RadiansToDegrees(Roll);

    if (KeyHandler::State[SDL_SCANCODE_W])
    {
        if (Pitch >= -_pitchCap)
        {
            transform = transform * Granite::GMath::GetXRotation(deltaTime * _moveSpeed);
        }

        mesh.MeshPosY -= _positionSpeed;
        updatedMovement = true;
    }

    if (KeyHandler::State[SDL_SCANCODE_S])
    {
        if (Pitch <= _pitchCap)
        {
            transform = transform * Granite::GMath::GetXRotation(deltaTime * -_moveSpeed);
        }
        mesh.MeshPosY += _positionSpeed;
        updatedMovement = true;
    }

    if (KeyHandler::State[SDL_SCANCODE_A])
    {
        if (Yaw <= _yawCap)
        {
            transform = transform * Granite::GMath::GetYRotation(deltaTime * _moveSpeed);
        }

        mesh.MeshPosX -= _positionSpeed;
        updatedMovement = true;
    }

    if (KeyHandler::State[SDL_SCANCODE_D])
    {
        if (Yaw >= -_yawCap)
        {
            transform = transform * Granite::GMath::GetYRotation(deltaTime * -_moveSpeed);
        }

        mesh.MeshPosX += _positionSpeed;
        updatedMovement = true;
    }

    if (KeyHandler::State[SDL_SCANCODE_Q])
    {
        transform = transform * Granite::GMath::GetZRotation(deltaTime * -_moveSpeed * 1.5f);
        updatedMovement = true;
    }

    if (KeyHandler::State[SDL_SCANCODE_E])
    {
        transform = transform * Granite::GMath::GetZRotation(deltaTime * _moveSpeed * 1.5f);
        updatedMovement = true;
    }

    return updatedMovement;
}

void MainShip::_StabilizeShip(Granite::GMath::FMatrix4x4& transform, float deltaTime)
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (i == j)
            {
                mesh.Transformation->data[i][j] = Granite::GUtil::GInterpolate(mesh.Transformation->data[i][j], 1.f, deltaTime * _adjustmentSpeed);
            }
            else
            {
                mesh.Transformation->data[i][j] = Granite::GUtil::GInterpolate(mesh.Transformation->data[i][j], 0.f, deltaTime * _adjustmentSpeed);
            }
        }
    }
}