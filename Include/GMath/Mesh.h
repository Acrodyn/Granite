#pragma once

#include <vector>
#include "FVector3.h"
#include "Polygon.h"

namespace Granite
{
	namespace GMath
	{
        struct Mesh
        {
            std::vector<Polygon> polygons;

            void AddPolygon(const FVector3* vertices, const FVector3* textureCoords, const FVector3* normals)
            {
                Polygon newPolygon;

                for (int i = 0; i < 3; ++i)
                {
                    newPolygon.vertices[i] = vertices[i];
                    newPolygon.textureCoordinates[i] = textureCoords[i];
                    newPolygon.normals[i] = normals[i];
                }

                polygons.push_back(newPolygon);
            }
        };
	}
}