#include "GGame/EnemyShip.h"

EnemyShip::EnemyShip(std::string model, std::string texture) : GObject(model, texture)
{

}

void EnemyShip::Update(float deltaTime, const Uint8* state)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}