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

            void SetTexture(std::string texturePath);
            void Rasterize();
            void Transform(const Granite::GMath::FMatrix4x4 &transformMatrix);
            void SetWorldSpaceTransform(const FMatrix4x4 &transformMatrix);
            void SetViewSpaceTransform(const FMatrix4x4& transformMatrix);
            const FMatrix4x4& GetWorldSpaceTransform() const;
            const FMatrix4x4& GetViewSpaceTransform() const;
            void OffsetMesh(float offset);

            float MeshDepth;
            float MeshScale;
            float MeshPosX;
            float MeshPosY;
            float MeshPosZ;
            float MeshInitialPosOffset;
            FMatrix4x4 *Transformation;

        private:
            void _AddPolygon(const FVector3* vertices);
            void _AddPolygon(const FVector3* vertices, const FVector3* textureCoords);
            void _RasterizePolygonThread(int startingPolygonIndex, int endingPolygonIndex);
            void _TransformPolygonThread(int startingPolygonIndex, int endingPolygonIndex, const Granite::GMath::FMatrix4x4& transformMatrix);

            std::vector<Polygon> polygons;
            GTexture *meshTexture;
            const FMatrix4x4 *worldSpaceTransform;
            const FMatrix4x4* viewSpaceTransform;
            

            friend ModelLoader;
        };
	}
}