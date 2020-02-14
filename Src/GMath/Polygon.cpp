#include <list>
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
        Polygon::Polygon() : meshPtr(nullptr), _intensity(0.f)
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

        void Polygon::CopyTextureCoordinates(const Polygon& other)
        {
            textureCoordinates[0] = other.textureCoordinates[0];
            textureCoordinates[1] = other.textureCoordinates[1];
            textureCoordinates[2] = other.textureCoordinates[2];
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
            std::vector<Polygon> projectedPolygons = _GetProjectedPolygons();
            std::vector<Polygon> clippedPolygons = _GetClippedPolygon(projectedPolygons);
            _DrawClippedPolygons(clippedPolygons, color, texture);
        }

        std::vector<Polygon> Polygon::_GetProjectedPolygons() const
        {
            Polygon triTranslated = *this;
            transformation = transformation * meshPtr->GetWorldSpaceTransform(); // TODO: možda mogu direkt ovo napravit?
            MultiplyMatrixPolygon(triTranslated, transformation);
            OffsetPolygonDepth(triTranslated, 1800.f);

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
                std::vector<Polygon> frustumClippedPolygons;
                _intensity = GUtil::Abs(dotProduct);

                // Convert World Space --> View Space
                MultiplyMatrixPolygon(triTranslated, meshPtr->GetViewSpaceTransform());

                // Clip front of frustum
                Polygon clipped[2];
                int clippedTriangles = ClipAgainstPlane(FVector3(0.f, 0.f, 0.1f), GMath::GetForwardVector(), triTranslated, clipped[0], clipped[1]);

                for (int i = 0; i < clippedTriangles; ++i)
                {
                    // Project triangles from 3D --> 2D
                    MultiplyMatrixPolygon(clipped[i], GMath::GetProjectionMatrix());

                    clipped[i].UniformMove(1.0f);
                    clipped[i].UniformScale(0.5f * (float)GConfig::WINDOW_WIDTH);

                    frustumClippedPolygons.push_back(clipped[i]);
                }

                return frustumClippedPolygons;
            }
        }

        std::vector<Polygon> Polygon::_GetClippedPolygon(const std::vector<Polygon> &polygonsToClip) const
        {
            std::vector<Polygon> clippedPolygons;

            for (auto &polygon : polygonsToClip)
            {
                Polygon clipped[2];
                clippedPolygons.push_back(polygon);
                int newTriangles = 1;

                for (int i = 0; i < 4; ++i)
                {
                    int polysToAdd = 0;

                    while (newTriangles > 0)
                    {
                        Polygon testPoly = clippedPolygons.back();
                        clippedPolygons.pop_back();
                        --newTriangles;

                        switch (i)
                        {
                            case 0:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(), GMath::GetUpVector(), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 1:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, GConfig::WINDOW_HEIGHT - 1.f, 0.f), GMath::GetUpVector() * -1, testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 2:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, 0.f, 0.f), GMath::GetRightVector(), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 3:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(GConfig::WINDOW_WIDTH - 1.f, 0.f, 0.f), GMath::GetRightVector() * -1, testPoly, clipped[0], clipped[1]);
                                break;
                            }
                        }

                        for (int j = 0; j < polysToAdd; ++j)
                        {
                            clippedPolygons.push_back(clipped[j]);
                        }
                    }

                    newTriangles = clippedPolygons.size();
                }
            }

            return clippedPolygons;
        }

        void Polygon::_DrawClippedPolygons(std::vector<Polygon> &polygonsToDraw, Color color, const GTexture* texture) const
        {
            for (auto& polyToDraw : polygonsToDraw)
            {
                if (GConfig::WIREFRAME_MODE)
                {
                    GGraphics::DrawLine(
                        GMath::IPoint(polyToDraw.vertices[0].x, polyToDraw.vertices[0].y),
                        GMath::IPoint(polyToDraw.vertices[1].x, polyToDraw.vertices[1].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(polyToDraw.vertices[1].x, polyToDraw.vertices[1].y),
                        GMath::IPoint(polyToDraw.vertices[2].x, polyToDraw.vertices[2].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(polyToDraw.vertices[2].x, polyToDraw.vertices[2].y),
                        GMath::IPoint(polyToDraw.vertices[0].x, polyToDraw.vertices[0].y),
                        Color::Red);
                }

                if (GConfig::RASTERIZE)
                {
                    /* GGraphics::Pixel newColor(128, 128, 128);
                     newColor.SetIntensity(_intensity);
                     Granite::GGraphics::RasterizeTriangle(polyToDraw, newColor);*/

                    if (texture != nullptr)
                    {
                        GGraphics::RasterizeTriangle(polyToDraw, &(*texture));
                    }
                    else
                    {
                        GGraphics::RasterizeTriangle(polyToDraw, color);
                    }
                }
            }
        }
    }
}