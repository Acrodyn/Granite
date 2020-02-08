#pragma once

#include <vector>
#include <string>

namespace Granite
{
    class GTexture;
    class ModelLoader;

	namespace GMath
	{
        class FMatrix4x4;
        class FVector3;
        class Polygon;

        class Mesh
        {
        public:
            Mesh() = default;
            Mesh(std::string modelPath, std::string texturePath = "");
            ~Mesh();

            void RasterizePolygons();
            void TransformPolygons(const Granite::GMath::FMatrix4x4 & transformMatrix);
            void OffsetMesh(float offset);

        private:
            void _AddPolygon(const FVector3* vertices);
            void _AddPolygon(const FVector3* vertices, const FVector3* textureCoords, const FVector3* normals);
            void _RasterizePolygonThread(int startingPolygonIndex, int endingPolygonIndex);
            void _TransformPolygonThread(int startingPolygonIndex, int endingPolygonIndex, const Granite::GMath::FMatrix4x4& transformMatrix);

            std::vector<Polygon> polygons;
            GTexture *meshTexture;

            friend ModelLoader;
        };
	}
}