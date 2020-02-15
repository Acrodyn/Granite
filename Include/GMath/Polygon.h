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
            Polygon(const Polygon &other, bool isFragment = false);
            ~Polygon() = default;

            void Move(Polygon &&other);

            mutable FVector3 vertices[3];
            mutable FVector3 textureCoordinates[3];
            //mutable FVector3 normals[3];

            void RepositionVertices(Uint8 index, const FVector3 &newPosition);
            void UniformMove(float scalar);
            void UniformScale(float scalar);
            void TransformPolygon(const FMatrix4x4& transformMatrix);
            void CopyTextureCoordinates(const Polygon &other);
            void RasterizePolygon(Color color) const;
            void RasterizePolygon(const GTexture* texture) const;
            void RasterizePolygon(Color color, const GTexture* texture) const;

        private:
            bool _fragment;
            mutable float _intensity;
            Mesh* meshPtr;
            FMatrix4x4 transformation;

            void _GetProjectedPolygons(Polygon&, Polygon** polygons) const;
            void _GetClippedPolygon(Polygon** polygons, Polygon** clippedPolygonStash) const;
            void _DrawClippedPolygons(Polygon** polygonsToDraw, Color color, const GTexture* texture) const;

            friend Mesh;
        };
	}
}