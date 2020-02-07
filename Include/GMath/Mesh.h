#pragma once

#include <vector>

namespace Granite
{
    class GTexture;

	namespace GMath
	{
        class FMatrix4x4;
        class FVector3;
        class Polygon;

        class Mesh
        {
        public:
            std::vector<Polygon> polygons;

            void AddPolygon(const FVector3* vertices, const FVector3* textureCoords, const FVector3* normals);
            void RasterizePolygons(const Granite::GMath::FMatrix4x4& transformMatrix, const Granite::GTexture& tex);

        private:
            //const Granite::GMath::FMatrix4x4 & transformMatrix, const Granite::GTexture& tex
            void _RasterizePolygonThread(int startingPolygonIndex, int endingPolygonIndex, const Granite::GMath::FMatrix4x4& transformMatrix, const Granite::GTexture& tex);
        };
	}
}