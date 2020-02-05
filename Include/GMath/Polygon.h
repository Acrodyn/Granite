#pragma once

#include "FVector3.h"
#include "GMath/FMatrix4x4.h"
#include "GGraphics/GGraphics.h"
#include "GGraphics/Colors.h"

namespace Granite
{
    class GTexture;

	namespace GMath
	{
        class Polygon
        {
        public:

            FVector3 vertices[3];
            FVector3 textureCoordinates[3];
            FVector3 normals[3];

            void UniformMove(float scalar);
            void UniformScale(float scalar);
            void RasterizePolygon(const FMatrix4x4& transformMatrix, Color color);
            void RasterizePolygon(const FMatrix4x4& transformMatrix, GTexture* texture);
            void RasterizePolygon(const FMatrix4x4& transformMatrix, Color color, GTexture* texture);
        };
	}
}