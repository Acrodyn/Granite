#include "GMath/GMath.h"
#include "GMath/FVector3.h"
#include "GMath/FMatrix4x4.h"
#include "GMath/Polygon.h"

namespace Granite
{
	namespace GMath
	{
        // TODO: Ovo SVE treba ici u matrix classu i treba pretty much izbrisat cijeli GMath file

        FMatrix4x4 GetXRotation(float angle)
        {
            FMatrix4x4 rotation;

            rotation.data[0][0] = 1;
            rotation.data[1][1] = cosf(angle);
            rotation.data[1][2] = sinf(angle);
            rotation.data[2][1] = -sinf(angle);
            rotation.data[2][2] = cosf(angle);
            rotation.data[3][3] = 1;

            return rotation;
        }

        FMatrix4x4 GetYRotation(float angle)
        {
            FMatrix4x4 rotation;

            rotation.data[0][0] = cos(angle);
            rotation.data[0][2] = sinf(angle);
            rotation.data[1][1] = 1;
            rotation.data[2][0] = -sinf(angle);
            rotation.data[2][2] = cosf(angle);
            rotation.data[3][3] = 1;

            return rotation;
        }

        FMatrix4x4 GetZRotation(float angle)
        {
            FMatrix4x4 rotation;

            rotation.data[0][0] = cosf(angle);
            rotation.data[0][1] = sinf(angle);
            rotation.data[1][0] = -sinf(angle);
            rotation.data[1][1] = cosf(angle);
            rotation.data[2][2] = 1;
            rotation.data[3][3] = 1;

            return rotation;
        }

        FVector3 GetUpVector()
        {
            return FVector3(0.f, 1.f, 0.f);
        }

        FVector3 GetForwardVector()
        {
            return FVector3(0.f, 0.f, 1.f);
        }

        FVector3 GetRightVector()
        {
            return FVector3(1.f, 0.f, 0.f);
        }

        FVector3 VectorIntersectPlane(const FVector3 &plane_p, const FVector3 &plane_n, FVector3 &lineStart, FVector3 &lineEnd)
        {
            float plane_d = -(plane_n.DotProduct(plane_p));
            float ad = lineStart.DotProduct(plane_n);
            float bd = lineEnd.DotProduct(plane_n);
            float t = (-plane_d - ad) / (bd - ad);
            FVector3 lineStartToEnd = lineEnd - lineStart;
            FVector3 lineToIntersect = lineStartToEnd * t;
            return (lineStart + lineToIntersect);
        }

        int ClipAgainstPlane(const FVector3& plane_p, const FVector3& plane_n, Polygon &inPoly, Polygon** outPolygons)
        {
            auto dist = [&](const FVector3& p)
            {
                return (plane_n.x * p.x + plane_n.y * p.y + plane_n.z * p.z - plane_n.DotProduct(plane_p));
            };

            FVector3* insidePoints[3]{};
            FVector3* outsidePoints[3]{};

            int insidePointCount = 0;
            int outsidePointCount = 0;
            float d0 = dist(inPoly.vertices[0]);
            float d1 = dist(inPoly.vertices[1]);
            float d2 = dist(inPoly.vertices[2]);

            if (d0 >= 0)
            {
                insidePoints[insidePointCount++] = &inPoly.vertices[0];
            }
            else
            {
                outsidePoints[outsidePointCount++] = &inPoly.vertices[0];
            }

            if (d1 > 0)
            {
                insidePoints[insidePointCount++] = &inPoly.vertices[1];
            }
            else
            {
                outsidePoints[outsidePointCount++] = &inPoly.vertices[1];
            }

            if (d2 > 0)
            {
                insidePoints[insidePointCount++] = &inPoly.vertices[2];
            }
            else
            {
                outsidePoints[outsidePointCount++] = &inPoly.vertices[2];
            }

            if (insidePointCount == 0)
            {
                return 0;
            }

            if (insidePointCount == 3)
            {
                outPolygons[0] = &inPoly;
                
                return 1;
            }

            if (insidePointCount == 1 && outsidePointCount == 2)
            {
                //outPoly1.CopyTextureCoordinates(inPoly);
                outPolygons[0] = &inPoly; 

                outPolygons[0]->RepositionVertices(0, *insidePoints[0]);
                outPolygons[0]->RepositionVertices(1, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[0]));
                outPolygons[0]->RepositionVertices(2, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[1]));

                //outPolygons[0]->vertices[0] = *insidePoints[0];
                //outPolygons[0]->vertices[1] = VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[0]);
                //outPolygons[0]->vertices[2] = VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[1]);

                return 1;
            }

            if (insidePointCount == 2 && outsidePointCount == 1)
            {
            /*    outPoly1.CopyTextureCoordinates(inPoly);
                outPoly2.CopyTextureCoordinates(inPoly);*/
                outPolygons[0] = &inPoly;
                outPolygons[1] = new Polygon(inPoly, true);

                outPolygons[0]->RepositionVertices(0, *insidePoints[0]);
                outPolygons[0]->RepositionVertices(1, *insidePoints[1]);
                outPolygons[0]->RepositionVertices(2, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[0]));

             /*   outPolygons[0]->vertices[0] = *insidePoints[0];
                outPolygons[0]->vertices[1] = *insidePoints[1];
                outPolygons[0]->vertices[2] = VectorIntersectPlane(plane_p, plane_n, *insidePoints[0], *outsidePoints[0]);*/

                outPolygons[1]->RepositionVertices(0, *insidePoints[1]);
                outPolygons[1]->RepositionVertices(1, outPolygons[0]->vertices[2]);
                outPolygons[1]->RepositionVertices(2, VectorIntersectPlane(plane_p, plane_n, *insidePoints[1], *outsidePoints[0]));

                /*outPolygons[1]->vertices[0] = *insidePoints[1];
                outPolygons[1]->vertices[1] = outPolygons[0]->vertices[2];
                outPolygons[1]->vertices[2] = VectorIntersectPlane(plane_p, plane_n, *insidePoints[1], *outsidePoints[0]);*/

                return 2;
            }
        }

        FMatrix4x4 GetPointAtMatrix(const FVector3& pos, const FVector3& target, const FVector3& up)
        {
            FVector3 forward = target - pos;
            forward.Normalize();

            FVector3 a = forward * up.DotProduct(forward);
            FVector3 newUp = up - a;
            newUp.Normalize();

            FVector3 newRight = newUp.CrossProduct(forward);

            FMatrix4x4 matrix;

            matrix.data[0][0] = newRight.x; matrix.data[0][1] = newRight.y; matrix.data[0][2] = newRight.z; matrix.data[0][3] = 0.f;
            matrix.data[1][0] = newUp.x; matrix.data[1][1] = newUp.y; matrix.data[1][2] = newUp.z; matrix.data[1][3] = 0.f;
            matrix.data[2][0] = forward.x; matrix.data[2][1] = forward.y; matrix.data[2][2] = forward.z; matrix.data[2][3] = 0.f;
            matrix.data[3][0] = pos.x; matrix.data[3][1] = pos.y; matrix.data[3][2] = pos.z; matrix.data[3][3] = 1.f;

            return matrix;
        }

        FMatrix4x4 GetInverseMatrix(FMatrix4x4 &matrix) // Only for Rotation/Translation Matrices
        {
            FMatrix4x4 invMatrix;

            invMatrix.data[0][0] = matrix.data[0][0]; invMatrix.data[0][1] = matrix.data[1][0]; invMatrix.data[0][2] = matrix.data[2][0]; invMatrix.data[0][3] = 0.0f;
            invMatrix.data[1][0] = matrix.data[0][1]; invMatrix.data[1][1] = matrix.data[1][1]; invMatrix.data[1][2] = matrix.data[2][1]; invMatrix.data[1][3] = 0.0f;
            invMatrix.data[2][0] = matrix.data[0][2]; invMatrix.data[2][1] = matrix.data[1][2]; invMatrix.data[2][2] = matrix.data[2][2]; invMatrix.data[2][3] = 0.0f;
            invMatrix.data[3][0] = -(matrix.data[3][0] * invMatrix.data[0][0] + matrix.data[3][1] * invMatrix.data[1][0] + matrix.data[3][2] * invMatrix.data[2][0]);
            invMatrix.data[3][1] = -(matrix.data[3][0] * invMatrix.data[0][1] + matrix.data[3][1] * invMatrix.data[1][1] + matrix.data[3][2] * invMatrix.data[2][1]);
            invMatrix.data[3][2] = -(matrix.data[3][0] * invMatrix.data[0][2] + matrix.data[3][1] * invMatrix.data[1][2] + matrix.data[3][2] * invMatrix.data[2][2]);
            invMatrix.data[3][3] = 1.0f;

            return invMatrix;
        }

        // TODO: Ovo treba primat parametre i po tome podešavat projection matrix, bez hardkodiranja!
        FMatrix4x4 GetProjectionMatrix()
        {
            GMath::FMatrix4x4 projectionMatrix;

            float near = 0.1f;
            float far = 1000.0f;
            float fFovRad = GMath::AnglesToRadians(90.f);

            float scale = 1 / tan(90.f * 0.5 * M_PI / 180); // alternative

            projectionMatrix.data[0][0] = fFovRad;
            projectionMatrix.data[1][1] = fFovRad;
            projectionMatrix.data[2][2] = far / (far - near);
            projectionMatrix.data[3][2] = -far * near / (far - near);
            projectionMatrix.data[2][3] = 1.f;
            projectionMatrix.data[3][3] = 0.f;

            return projectionMatrix;
        }

        FMatrix4x4 GetTranslationMatrix(float x, float y, float z)
        {
            FMatrix4x4 translatedMatrix;

            translatedMatrix.data[0][0] = 1.f;
            translatedMatrix.data[1][1] = 1.f;
            translatedMatrix.data[2][2] = 1.f;
            translatedMatrix.data[3][3] = 1.f;
            translatedMatrix.data[3][0] = x;
            translatedMatrix.data[3][1] = y;
            translatedMatrix.data[3][2] = z;

            return translatedMatrix;
        }

        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix)
        {
            FVector3 multipliedVector;

            multipliedVector.x = vector.x * matrix.data[0][0] + vector.y * matrix.data[1][0] + vector.z * matrix.data[2][0] + matrix.data[3][0];
            multipliedVector.y = vector.x * matrix.data[0][1] + vector.y * matrix.data[1][1] + vector.z * matrix.data[2][1] + matrix.data[3][1];
            multipliedVector.z = vector.x * matrix.data[0][2] + vector.y * matrix.data[1][2] + vector.z * matrix.data[2][2] + matrix.data[3][2];
            float w = vector.x * matrix.data[0][3] + vector.y * matrix.data[1][3] + vector.z * matrix.data[2][3] + matrix.data[3][3];

            if (w != 0.0f)
            {
                multipliedVector.x /= w;
                multipliedVector.y /= w;
                multipliedVector.z /= w;
            }

            return multipliedVector;
        }

        void MultiplyMatrixPolygon(Polygon& polygon, const FMatrix4x4& matrix)
        {
            for (int i = 0; i < 3; ++i)
            {
                polygon.vertices[i] = MultiplyMatrixVector(polygon.vertices[i], matrix);
                // polygon.normals[i] = MultiplyMatrixVector(polygon.normals[i], matrix);
            }
        }

        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix)
        {
            for (int i = 0; i < 3; ++i)
            {
                oPolygon.vertices[i] = MultiplyMatrixVector(iPolygon.vertices[i], matrix);
                //oPolygon.normals[i] = MultiplyMatrixVector(iPolygon.normals[i], matrix);
            }
        }

        void OffsetPolygonDepth(Polygon& oTriangle, float offset)
        {
            for (int i = 0; i < 3; ++i)
            {
                oTriangle.vertices[i].z = oTriangle.vertices[i].z + offset;
                //oTriangle.normals[i].z = oTriangle.normals[i].z + offset;
            }
        }

        float AnglesToRadians(float angles)
        {
            return angles * PI / 180.f;
        }
	}
}