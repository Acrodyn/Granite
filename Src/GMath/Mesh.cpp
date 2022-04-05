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
        Mesh::Mesh(std::string modelPath, std::string texturePath) : meshTexture(nullptr), worldSpaceTransform(nullptr), viewSpaceTransform(nullptr)
        {
            ModelLoader::LoadModel(*this, "/Assets/" + modelPath);
            SetTexture(texturePath);

            Transformation = new FMatrix4x4();
            Transformation->MakeIdentity();
        }

        Mesh::~Mesh()
        {
            if (!meshTexture)
            {
                delete meshTexture;
                meshTexture = nullptr;
            }

            delete Transformation;
        }

        void Mesh::SetTexture(std::string texturePath)
        {
            if (texturePath != "")
            {
                std::filesystem::path currentPath = std::filesystem::current_path();
                std::string fullPath = currentPath.generic_string() + "/Assets/" + texturePath;

                meshTexture = new GTexture(fullPath.c_str());
                Granite::GGraphics::InvertSurfaceVertically(meshTexture->textureData);
            }
        }

        void Mesh::Rasterize()
        {
            *Transformation = *Transformation * GetWorldSpaceTransform(); // TODO: možda mogu direkt ovo napravit?

            const int threadNumber = 1;
            int segmentSize = std::ceil(polygons.size() / threadNumber);
            std::thread threads[threadNumber];

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i] = std::thread(&Mesh::_RasterizePolygonThread, this, segmentSize * i, segmentSize * (i + 1));
            }

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i].join();
            }

            // TODO: possible mutex usage for separate rasterization and drawing?
        }

        void Mesh::Transform(const Granite::GMath::FMatrix4x4& transformMatrix)
        {
            const int threadNumber = 1;
            int segmentSize = std::ceil(polygons.size() / threadNumber);
            std::thread threads[threadNumber];

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i] = std::thread(&Mesh::_TransformPolygonThread, this, segmentSize * i, segmentSize * (i + 1), transformMatrix);
            }

            for (int i = 0; i < threadNumber; ++i)
            {
                threads[i].join();
            }
        }

        void Mesh::SetWorldSpaceTransform(const FMatrix4x4& transformMatrix)
        {
            worldSpaceTransform = &transformMatrix;
        }

        void Mesh::SetViewSpaceTransform(const FMatrix4x4& transformMatrix)
        {
            viewSpaceTransform = &transformMatrix;
        }

        const FMatrix4x4& Mesh::GetWorldSpaceTransform() const
        {
            return *worldSpaceTransform;
        }

        const FMatrix4x4& Mesh::GetViewSpaceTransform() const
        {
            return *viewSpaceTransform;
        }

        void Mesh::OffsetMesh(float offset)
        {
            for (auto& poly : polygons)
            {
                poly.OffsetPolygonDepth(offset);
            }
        }

        void Mesh::_RasterizePolygonThread(int startingPolygonIndex, int endingPolygonIndex)
        {
            if (endingPolygonIndex > polygons.size())
            {
                endingPolygonIndex = polygons.size();
            }

            for (int i = startingPolygonIndex; i < endingPolygonIndex; ++i)
            {
                polygons[i].RasterizePolygon(&(*meshTexture));
            }
        }

        void Mesh::_TransformPolygonThread(int startingPolygonIndex, int endingPolygonIndex, const Granite::GMath::FMatrix4x4& transformMatrix)
        {
            if (endingPolygonIndex > polygons.size())
            {
                endingPolygonIndex = polygons.size();
            }

            for (int i = startingPolygonIndex; i < endingPolygonIndex; ++i)
            {
                polygons[i].TransformPolygon(transformMatrix);
            }
        }

        void Mesh::_AddPolygon(const FVector3* vertices)
        {
            Polygon newPolygon;

            for (int i = 0; i < 3; ++i)
            {
                newPolygon.vertices[i] = vertices[i];
            }

            newPolygon.meshPtr = &(*this);
            polygons.push_back(newPolygon);
        }

        void Mesh::_AddPolygon(const FVector3* vertices, const FVector3* textureCoords)
        {
            Polygon newPolygon;

            for (int i = 0; i < 3; ++i)
            {
                newPolygon.vertices[i] = vertices[i];
                newPolygon.textureCoordinates[i] = textureCoords[i];
            }

            newPolygon.meshPtr = &(*this);
            polygons.push_back(newPolygon);
        }
    }
}