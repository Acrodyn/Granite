#include "GGame/GObject.h"

GObject::GObject(std::string model, std::string texture) : mesh(model)
{
	mesh.SetTexture(texture);
}

void GObject::Update(float deltaTime, const Uint8* state)
{
	Granite::GMath::FMatrix4x4 matWorld;
	matWorld.MakeIdentity();

    if (state[SDL_SCANCODE_W])
    {
        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * 2.5f);
    }

    if (state[SDL_SCANCODE_S])
    {
        matWorld = matWorld * Granite::GMath::GetXRotation(deltaTime * -2.5f);
    }

    if (state[SDL_SCANCODE_A])
    {
        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * 2.5f);
    }

    if (state[SDL_SCANCODE_D])
    {
        matWorld = matWorld * Granite::GMath::GetYRotation(deltaTime * -2.5f);
    }

    if (state[SDL_SCANCODE_Q])
    {
        matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * -2.5f);
    }

    if (state[SDL_SCANCODE_E])
    {
        matWorld = matWorld * Granite::GMath::GetZRotation(deltaTime * 2.5f);
    }

    mesh.SetWorldSpaceTransform(matWorld);
    mesh.SetViewSpaceTransform(Granite::Camera::matView);
    mesh.Rasterize();
}