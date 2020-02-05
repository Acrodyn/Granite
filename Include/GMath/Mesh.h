#pragma once

#include <vector>

namespace Granite
{
	namespace GMath
	{
        class FVector3;
        class Polygon;

        class Mesh
        {
        public:
            std::vector<Polygon> polygons;

            void AddPolygon(const FVector3* vertices, const FVector3* textureCoords, const FVector3* normals);
        };
	}
}