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
#include <memory>

namespace Granite
{
    namespace GMath
    {
        Polygon::Polygon() : meshPtr(nullptr), _fragment(false), _intensity(0.f)
        {
            transformation.MakeIdentity();
        }

        Polygon::Polygon(const Polygon& other, bool isFragment)
        {
            _fragment = isFragment;

            for (int i = 0; i < 3; ++i)
            {
                vertices[i] = other.vertices[i];
                textureCoordinates[i] = other.textureCoordinates[i];
            }

            transformation = other.transformation;
            meshPtr = &(*other.meshPtr);
            _intensity = other._intensity;
        }

        void Polygon::ChangeData(Uint8 index, const FVector3& newVertices, const FVector3& newTextureCoordinates)
        {
            vertices[index] = newVertices;
            textureCoordinates[index] = newTextureCoordinates;
        }

        void Polygon::SliceData(Uint8 index, GMath::FVector3* pt0, GMath::FVector3* pt1, GMath::FVector3* tx0, GMath::FVector3* tx1, const FVector3& newPosition)
        {
            vertices[index] = newPosition;

            const float vertexSliceX = (newPosition.x - pt0->x) / (pt1->x - pt0->x);
            const float vertexSliceY = (newPosition.y - pt0->y) / (pt1->y - pt0->y);

            float lerpX = GUtil::GInterpolate(tx0->x, tx1->x, vertexSliceX);
            float lerpY = GUtil::GInterpolate(tx0->y, tx1->y, vertexSliceY);

            if (!isnan(lerpX))
            {
                textureCoordinates[index].x = lerpX;
            }
            
            if (!isnan(lerpY))
            {
                textureCoordinates[index].y = lerpY;
            }
        }

        void Polygon::Move(Polygon&& other)
        {
            for (int i = 0; i < 3; ++i)
            {
                vertices[i] = other.vertices[i];
                textureCoordinates[i] = other.textureCoordinates[i];
                //normals[i] = other.normals[i];
            }

            _intensity = other._intensity;
            meshPtr = other.meshPtr;
            transformation = other.transformation;
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
            Polygon *projectedPolygons[2]{};
            Polygon *clippedPolygonsStash[16]{}; // max number of clipped polys!

           _GetProjectedPolygons(triTranslated, projectedPolygons);

            if (projectedPolygons[0] == nullptr)
            {
                return;
            }

           _GetClippedPolygon(projectedPolygons, clippedPolygonsStash);
           _DrawClippedPolygons(clippedPolygonsStash, color, texture);

           for (int i = 0; i < 16; ++i)
           {
               if (clippedPolygonsStash[i] == nullptr)
               {
                   break;
               }

               if (clippedPolygonsStash[i]->_fragment)
               {
                   delete clippedPolygonsStash[i];
               }
           }
        }

        void Polygon::_GetProjectedPolygons(Polygon &transformPolygon, Polygon** polygons) const
        {
            transformation = transformation * meshPtr->GetWorldSpaceTransform(); // TODO: možda mogu direkt ovo napravit?
            MultiplyMatrixPolygon(transformPolygon, transformation);
            OffsetPolygonDepth(transformPolygon, 5.f);

            FVector3 normal, line1, line2, cameraToPoint;

            line1 = transformPolygon.vertices[1] - transformPolygon.vertices[0];
            line2 = transformPolygon.vertices[2] - transformPolygon.vertices[0];

            normal = line1.CrossProduct(line2);
            normal.Normalize();

            cameraToPoint = transformPolygon.vertices[0] - Camera::GetMainCamera()->position;
            cameraToPoint.Normalize();

            float dotProduct = normal.DotProduct(cameraToPoint);

            if (dotProduct < .0f)
            {
                _intensity = GUtil::Abs(dotProduct);

                // Convert World Space --> View Space
                MultiplyMatrixPolygon(transformPolygon, meshPtr->GetViewSpaceTransform());

                // Clip front of frustum
                int clippedTriangles = ClipAgainstPlane(FVector3(0.f, 
                    0.f, 0.1f), GMath::GetForwardVector(), transformPolygon, polygons);

                for (int i = 0; i < clippedTriangles; ++i)
                {
                    // Project triangles from 3D --> 2D
                    MultiplyMatrixPolygon(*polygons[i], GMath::GetProjectionMatrix());

                    polygons[i]->UniformMove(1.0f);
                    polygons[i]->UniformScale(0.5f * (float)GConfig::WINDOW_WIDTH);
                }
            }
        }

        void Polygon::_GetClippedPolygon(Polygon** polygons, Polygon** clippedPolygonStash) const
        {
            Polygon *clippedPolygons[2]{};
            int newPolygonCount = 0;

            for (int i = 0; i < 2; ++i)
            {
                if (polygons[i] == nullptr)
                {
                    continue;
                }
                
                clippedPolygonStash[newPolygonCount] = polygons[i];
                ++newPolygonCount;
                int polyFragments = 1;

                for (int i = 0; i < 4; ++i)
                {
                    int polysToAdd = 0;

                    while (polyFragments > 0)
                    {
                        Polygon *testPoly = clippedPolygonStash[newPolygonCount - 1];

                        if (testPoly == nullptr)
                        {
                            continue;
                        }

                        clippedPolygonStash[--newPolygonCount] = nullptr;
                        --polyFragments;

                        switch (i)
                        {
                            case 0:
                            {
                                polysToAdd = ClipAgainstPlane(GMath::ZeroVector, GMath::UpVector, *testPoly, clippedPolygons);
                                break;
                            }
                            case 1:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(0.f, GConfig::WINDOW_HEIGHT - 1.f, 0.f), GMath::DownVector, *testPoly, clippedPolygons);
                                break;
                            }
                            case 2:
                            {
                                polysToAdd = ClipAgainstPlane(GMath::ZeroVector, GMath::RightVector, *testPoly, clippedPolygons);
                                break;
                            }
                            case 3:
                            {
                                polysToAdd = ClipAgainstPlane(FVector3(GConfig::WINDOW_WIDTH - 1.f, 0.f, 0.f), GMath::LeftVector, *testPoly, clippedPolygons);
                                break;
                            }
                        }

                        delete testPoly;
                        testPoly = nullptr;

                        for (int j = 0; j < polysToAdd; ++j)
                        {
                            clippedPolygonStash[newPolygonCount++] = clippedPolygons[j];
                        }

                    }

                    polyFragments = newPolygonCount;
                }
            }
        }

        void Polygon::_DrawClippedPolygons(Polygon** polygonsToDraw, Color color, const GTexture* texture) const
        {
            for (int i = 0; i < 16; ++i)
            {
                if (polygonsToDraw[i] == nullptr)
                {
                    break;
                }

                if (GConfig::WIREFRAME_MODE)
                {
                    GGraphics::DrawLine(
                        GMath::IPoint(polygonsToDraw[i]->vertices[0].x, polygonsToDraw[i]->vertices[0].y),
                        GMath::IPoint(polygonsToDraw[i]->vertices[1].x, polygonsToDraw[i]->vertices[1].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(polygonsToDraw[i]->vertices[1].x, polygonsToDraw[i]->vertices[1].y),
                        GMath::IPoint(polygonsToDraw[i]->vertices[2].x, polygonsToDraw[i]->vertices[2].y),
                        Color::Red);

                    GGraphics::DrawLine(
                        GMath::IPoint(polygonsToDraw[i]->vertices[2].x, polygonsToDraw[i]->vertices[2].y),
                        GMath::IPoint(polygonsToDraw[i]->vertices[0].x, polygonsToDraw[i]->vertices[0].y),
                        Color::Red);
                }

                if (GConfig::RASTERIZE)
                {
                    /* GGraphics::Pixel newColor(128, 128, 128);
                     newColor.SetIntensity(_intensity);
                     Granite::GGraphics::RasterizeTriangle(polyToDraw, newColor);*/

                    if (texture != nullptr)
                    {
                        GGraphics::RasterizeTriangle(*polygonsToDraw[i], &(*texture));
                    }
                    else
                    {
                        GGraphics::RasterizeTriangle(*polygonsToDraw[i], color);
                    }
                }
            }
        }
    }
}