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

            mutable FVector3 vertices[3];
            mutable FVector3 textureCoordinates[3];
            mutable FVector3 normals[3];

            void UniformMove(float scalar);
            void UniformScale(float scalar);
            void RasterizePolygon(const FMatrix4x4& transformMatrix, Color color) const;
            void RasterizePolygon(const FMatrix4x4& transformMatrix, const GTexture* texture) const;
            void RasterizePolygon(const FMatrix4x4& transformMatrix, Color color, const GTexture* texture) const;
        };
	}
}