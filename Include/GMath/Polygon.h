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
            void CopyTextureCoordinates(const Polygon &other);
            void RasterizePolygon(Color color) const;
            void RasterizePolygon(const GTexture* texture) const;
            void RasterizePolygon(Color color, const GTexture* texture) const;

        private:
            mutable float _intensity;
            Mesh* meshPtr;
            FMatrix4x4 transformation;

            std::vector<Polygon> _GetProjectedPolygons() const;
            std::vector<Polygon> _GetClippedPolygon(const std::vector<Polygon> &polygonsToClip) const;
            void _DrawClippedPolygons(std::vector<Polygon> &polygonsToDraw, Color color, const GTexture* texture) const;

            friend Mesh;
        };
	}
}