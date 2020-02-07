#include <vector>
#include <thread>
#include "GGraphics/GTexture.h"
#include "GMath/Mesh.h"
#include "GMath/FVector3.h"
#include "GMath/Polygon.h"
#include "GMath/FMatrix4x4.h"

namespace Granite
{
    namespace GMath
    {
        void Mesh::AddPolygon(const FVector3* vertices, const FVector3* textureCoords, const FVector3* normals)
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


        void Mesh::RasterizePolygons(const Granite::GMath::FMatrix4x4& transformMatrix, const Granite::GTexture& tex)
        {
            const int threadNumber = 8;
            int segmentSize = std::ceil(polygons.size() / threadNumber) + 1;
            std::thread threads[threadNumber];

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i] = std::thread(&Mesh::_RasterizePolygonThread, this, segmentSize * i, segmentSize * (i + 1) , transformMatrix, tex);
            }

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i].join();
            }
        }

        void Mesh::_RasterizePolygonThread(int startingPolygonIndex, int endingPolygonIndex, const Granite::GMath::FMatrix4x4& transformMatrix, const Granite::GTexture& tex)
        {
            if (endingPolygonIndex > polygons.size())
            {
                endingPolygonIndex = polygons.size();
            }

            for (int i = startingPolygonIndex; i < endingPolygonIndex; ++i)
            {
                polygons[i].RasterizePolygon(transformMatrix, &tex);
            }
        }
    }
}