#include "GGame/EnemyShip.h"

EnemyShip::EnemyShip(std::string model, std::string texture) : GObject(model, texture)
{
    this->SetupView();
}

void EnemyShip::Update(float deltaTime)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}

void EnemyShip::SetupView()
{
    mesh.MeshDepth = 25.f;
    mesh.MeshScale = 0.2f;
    mesh.MeshInitialPosOffset = 1.f;
}