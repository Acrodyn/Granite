#include "GMath/Polygon.h"
#include "GMath/GMath.h"
#include "GMath/IPoint.h"
#include "GUtils/GConfig.h"
#include "GGraphics/GGraphics.h"
#include "GUtils/GUtil.h"
#include "GGraphics/GTexture.h"
#include "GGraphics/Colors.h"

namespace Granite
{
    namespace GMath
    {
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

        void Polygon::RasterizePolygon(const FMatrix4x4& transformMatrix, Color color) const
        {
            RasterizePolygon(transformMatrix, color, nullptr);
        }

        void Polygon::RasterizePolygon(const FMatrix4x4& transformMatrix, const GTexture* texture) const
        {
            RasterizePolygon(transformMatrix, Color::Grey, texture);
        }

        void Polygon::RasterizePolygon(const FMatrix4x4& transformMatrix, Color color, const GTexture* texture) const
        {
            FVector3 camera; // TEMP!
            Polygon triTranslated;

            MultiplyMatrixPolygon(const_cast<Polygon&>(*this), transformMatrix);
            triTranslated = *this;
            OffsetPolygonDepth(triTranslated, 1900.f);

            FVector3 normal, line1, line2, cameraToPoint;

            line1 = triTranslated.vertices[1] - triTranslated.vertices[0];
            line2 = triTranslated.vertices[2] - triTranslated.vertices[0];

            normal = line1.CrossProduct(line2);
            normal.Normalize();

            cameraToPoint = triTranslated.vertices[0] - camera;
            cameraToPoint.Normalize();

            float dotProduct = normal.DotProduct(cameraToPoint);

            if (dotProduct < .0f)
            {
                // Project triangles from 3D --> 2D
               MultiplyMatrixPolygon(triTranslated, GGraphics::GetProjectionMatrix());

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
                   /* GGraphics::Pixel newColor(128, 128, 128);
                    float intensity = GUtil::Abs(dotProduct);
                    newColor.SetIntensity(intensity);*/
                    //Granite::GGraphics::RasterizeTriangle(triTranslated, newColor);

                    if (texture != nullptr)
                    {
                        GGraphics::RasterizeTriangle(triTranslated, &(*texture));
                    }
                    else
                    {
                        GGraphics::RasterizeTriangle(triTranslated, color);
                    }
                }
            }
        }
    }
}