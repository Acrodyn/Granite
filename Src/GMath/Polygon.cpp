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
                std::vector<Polygon> clippedPolygons;
                _intensity = GUtil::Abs(dotProduct);
                   
                // Convert World Space --> View Space
                MultiplyMatrixPolygon(triTranslated, meshPtr->GetViewSpaceTransform());

                Polygon clipped[2];
                int clippedTriangles = ClipAgainstPlane(FVector3(0.f, 0.f, 0.1f), FVector3(0.f, 0.f, 1.f), triTranslated, clipped[0], clipped[1]);

                for (int i = 0; i < clippedTriangles; ++i)
                {
                    // Project triangles from 3D --> 2D
                    MultiplyMatrixPolygon(clipped[i], GMath::GetProjectionMatrix());

                    clipped[i].UniformMove(1.0f);
                    clipped[i].UniformScale(0.5f * (float)GConfig::WINDOW_WIDTH);

                    clippedPolygons.push_back(clipped[i]);
                }

                _DrawClippedPolygon(clippedPolygons);
            }
        }

        void Polygon::_DrawClippedPolygon(const std::vector<Polygon> &clippedPolygons) const
        {
            for (auto &poly : clippedPolygons)
            {
                Polygon clipped[2];
                std::list<Polygon> metaClipPolys;

                metaClipPolys.push_back(poly);
                int newTriangles = 1;

                for (int i = 0; i < 5; ++i)
                {
                    int polysToAdd = 0;

                    while (newTriangles > 0)
                    {
                        Polygon testPoly = metaClipPolys.front();
                        metaClipPolys.pop_front();
                        --newTriangles;

                        switch (i)
                        {
                            case 0:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, 0.f, 0.f), FVector3(0.f, 1.f, 0.f), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 1:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, GConfig::WINDOW_HEIGHT - 1.f, 0.f), FVector3(0.f, -1.f, 0.f), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 2:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, 0.f, 0.f), FVector3(1.f, 0.f, 0.f), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 3:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(GConfig::WINDOW_WIDTH - 1.f, 0.f, 0.f), FVector3(-1.f, 0.f, 0.f), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                            case 4:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, 0.f, 0.1f), FVector3(0.f, 0.f, 1.f), testPoly, clipped[0], clipped[1]);
                                break;
                            }
                        }

                        for (int j = 0; j < polysToAdd; ++j)
                        {
                            metaClipPolys.push_back(clipped[j]);
                        }
                    }

                    newTriangles = metaClipPolys.size();
                }

                for (auto &polyToDraw : metaClipPolys)
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
                        GGraphics::Pixel newColor(128, 128, 128);
                        newColor.SetIntensity(_intensity);
                        Granite::GGraphics::RasterizeTriangle(polyToDraw, newColor);

                        /* if (texture != nullptr)
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
}