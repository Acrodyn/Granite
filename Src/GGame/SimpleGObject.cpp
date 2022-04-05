#include "GGame/SimpleGObject.h"

SimpleGObject::SimpleGObject(std::string model, std::string texture) : GObject(model, texture)
{
    this->SetupView();
}

void SimpleGObject::Update(float deltaTime)
{
    Granite::GMath::FMatrix4x4 matWorld;
    matWorld.MakeIdentity();

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}

void SimpleGObject::SetupView()
{
    mesh.MeshDepth = 25.f;
    mesh.MeshScale = 0.2f;
    mesh.MeshInitialPosOffset = 1.f;
}