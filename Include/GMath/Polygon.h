#pragma once

#include "FVector3.h"
#include "GMath/FMatrix4x4.h"
#include "GGraphics/GGraphics.h"
#include "GGraphics/Colors.h"

namespace Granite
{
    class GTexture;
    class Mesh;

	namespace GMath
	{
        class Polygon
        {
        public:
            Polygon();
            ~Polygon() = default;

            mutable FVector3 vertices[3];
            mutable FVector3 textureCoordinates[3];
            mutable FVector3 normals[3];

            void UniformMove(float scalar);
            void UniformScale(float scalar);
            void TransformPolygon(const FMatrix4x4& transformMatrix);
            void RasterizePolygon(Color color) const;
            void RasterizePolygon(const GTexture* texture) const;
            void RasterizePolygon(Color color, const GTexture* texture) const;

        private:
            Mesh* meshPtr;
            FMatrix4x4 transformation;

            friend Mesh;
        };
	}
}