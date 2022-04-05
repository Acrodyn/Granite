#pragma once

#include "SDL2/SDL.h"
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

class GObject
{
public:
    GObject(std::string model, std::string texture = "");
    ~GObject() = default;

    virtual void Update(float deltaTime) = 0;

    /*Granite::GMath::Mesh mesh("Great Fox.obj", "greatFox.png");
    mesh.Transform(Granite::GMath::GetXRotation(Granite::GMath::AnglesToRadians(180.f)));
    mesh.Transform(Granite::GMath::GetYRotation(Granite::GMath::AnglesToRadians(180.f)));*/

protected:
    virtual void SetupView() = 0;

    Granite::GMath::Mesh mesh;
};