#include "GMath/Polygon.h"
#include "GMath/GMath.h"
#include "GMath/IPoint.h"
#include "GUtils/GConfig.h"
#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"
#include "GGraphics/GTexture.h"
#include "GGraphics/Colors.h"
#include "GGraphics/Camera.h"

namespace Granite
{
    namespace GMath
    {
        Polygon::Polygon() : meshPtr(nullptr)
        {
            transformation.MakeIdentity();
        }

        void Polygon::UniformMove(float scalar)
        {
            vertices[0].x += scalar;
            vertices[0].y += scalar;
            vertices[0].z += scalar;

            vertices[1].x += scalar;
            vertices[1].y += scalar;
            vertices[1].z += scalar;

            vertices[2].x += scalar;
            vertices[2].y += scalar;
            vertices[2].z += scalar;
        }

        void Polygon::UniformScale(float scalar)
        {
            vertices[0].x *= scalar;
            vertices[0].y *= scalar;
            vertices[0].z *= scalar;

            vertices[1].x *= scalar;
            vertices[1].y *= scalar;
            vertices[1].z *= scalar;

            vertices[2].x *= scalar;
            vertices[2].y *= scalar;
            vertices[2].z *= scalar;
        }

        void Polygon::TransformPolygon(const FMatrix4x4& transformMatrix)
        {
            MultiplyMatrixPolygon(const_cast<Polygon&>(*this), transformMatrix);
        }

        void Polygon::RasterizePolygon(Color color) const
        {
            RasterizePolygon(color, nullptr);
        }

        void Polygon::RasterizePolygon(const GTexture* texture) const
        {
            RasterizePolygon(Color::Grey, texture);
        }

        void Polygon::RasterizePolygon(Color color, const GTexture* texture) const
        {
            Polygon triTranslated = *this;
            transformation = transformation * meshPtr->GetWorldSpaceTransform(); // TODO: možda mogu direkt ovo napravit?
            MultiplyMatrixPolygon(triTranslated, transformation);
            OffsetPolygonDepth(triTranslated, 15.f);

            FVector3 normal, line1, line2, cameraToPoint;

            line1 = triTranslated.vertices[1] - triTranslated.vertices[0];
            line2 = triTranslated.vertices[2] - triTranslated.vertices[0];

            normal = line1.CrossProduct(line2);
            normal.Normalize();

            cameraToPoint = triTranslated.vertices[0] - Camera::GetMainCamera()->position;
            cameraToPoint.Normalize();

            float dotProduct = normal.DotProduct(cameraToPoint);

            if (dotProduct < .0f)
            {
                // Convert World Space --> View Space
                MultiplyMatrixPolygon(triTranslated, meshPtr->GetViewSpaceTransform());

                // Project triangles from 3D --> 2D
                MultiplyMatrixPolygon(triTranslated, GMath::GetProjectionMatrix());

                triTranslated.UniformMove(1.0f);
                triTranslated.UniformScale(0.5f * (float)GConfig::WINDOW_WIDTH);

                if (GConfig::WIREFRAME_MODE)
                {
                    GGraphics::DrawLine(
                        GMath::IPoint(triTranslated.vertices[0].x, triTranslated.vertices[0].y),
                        GMath::IPoint(triTranslated.vertices[1].x, triTranslated.vertices[1].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(triTranslated.vertices[1].x, triTranslated.vertices[1].y),
                        GMath::IPoint(triTranslated.vertices[2].x, triTranslated.vertices[2].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(triTranslated.vertices[2].x, triTranslated.vertices[2].y),
                        GMath::IPoint(triTranslated.vertices[0].x, triTranslated.vertices[0].y),
                        Color::Red);
                }

                if (GConfig::RASTERIZE)
                {
                    GGraphics::Pixel newColor(128, 128, 128);
                    float intensity = GUtil::Abs(dotProduct);
                    newColor.SetIntensity(intensity);
                    Granite::GGraphics::RasterizeTriangle(triTranslated, newColor);

                /*    if (texture != nullptr)
                    {
                        GGraphics::RasterizeTriangle(triTranslated, &(*texture));
                    }
                    else
                    {
                        GGraphics::RasterizeTriangle(triTranslated, color);
                    }*/
                }
            }
        }
    }
}