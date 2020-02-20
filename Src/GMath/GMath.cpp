#include "GMath/GMath.h"
#include "GMath/FVector3.h"
#include "GMath/FMatrix4x4.h"
#include "GMath/Polygon.h"
#include "GUtils/GUtil.h"

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

            struct PolyData
            {
                FVector3* vertices;
                FVector3* texels;
            };

            PolyData insidePoints[3]{};
            PolyData outsidePoints[3]{};

            GMath::FVector3* pv0 = &inPoly.vertices[0];
            GMath::FVector3* pv1 = &inPoly.vertices[1];
            GMath::FVector3* pv2 = &inPoly.vertices[2];

            GMath::FVector3* tx0 = &inPoly.textureCoordinates[0];
            GMath::FVector3* tx1 = &inPoly.textureCoordinates[1];
            GMath::FVector3* tx2 = &inPoly.textureCoordinates[2];

            int insidePointCount = 0;
            int outsidePointCount = 0;
            float d0 = dist(*pv0);
            float d1 = dist(*pv1);
            float d2 = dist(*pv2);

            if (d0 >= 0)
            {
                insidePoints[insidePointCount].vertices = pv0;
                insidePoints[insidePointCount].texels = tx0;
                ++insidePointCount;
            }
            else
            {
                outsidePoints[outsidePointCount].vertices = pv0;
                outsidePoints[outsidePointCount].texels = tx0;
                ++outsidePointCount;
            }

            if (d1 > 0)
            {
                insidePoints[insidePointCount].vertices = pv1;
                insidePoints[insidePointCount].texels = tx1;
                ++insidePointCount;
            }
            else
            {
                outsidePoints[outsidePointCount].vertices = pv1;
                outsidePoints[outsidePointCount].texels = tx1;
                ++outsidePointCount;
            }

            if (d2 > 0)
            {
                insidePoints[insidePointCount].vertices = pv2;
                insidePoints[insidePointCount].texels = tx2;
                ++insidePointCount;
            }
            else
            {
                outsidePoints[outsidePointCount].vertices = pv2;
                outsidePoints[outsidePointCount].texels = tx2;
                ++outsidePointCount;
            }

            if (insidePointCount == 0)
            {
                return 0;
            }

            if (insidePointCount == 3)
            {
                outPolygons[0] = new Polygon(inPoly, true);
                
                return 1;
            }

            if (insidePointCount == 1 && outsidePointCount == 2)
            {
                outPolygons[0] = new Polygon(inPoly, true);

                outPolygons[0]->ChangeData(0, *insidePoints[0].vertices, *insidePoints[0].texels);
                outPolygons[0]->SliceData(1, insidePoints[0].vertices, outsidePoints[0].vertices, insidePoints[0].texels, outsidePoints[0].texels, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0].vertices, *outsidePoints[0].vertices));
                outPolygons[0]->SliceData(2, insidePoints[0].vertices, outsidePoints[1].vertices, insidePoints[0].texels, outsidePoints[1].texels, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0].vertices, *outsidePoints[1].vertices));

                return 1;
            }

            if (insidePointCount == 2 && outsidePointCount == 1)
            {
                outPolygons[0] = new Polygon(inPoly, true);
                outPolygons[1] = new Polygon(inPoly, true);

                outPolygons[0]->ChangeData(0, *insidePoints[0].vertices, *insidePoints[0].texels); // these ones probably don't need reposition
                outPolygons[0]->ChangeData(1, *insidePoints[1].vertices, *insidePoints[1].texels);
                outPolygons[0]->SliceData(2, insidePoints[0].vertices, outsidePoints[0].vertices, insidePoints[0].texels, outsidePoints[0].texels, VectorIntersectPlane(plane_p, plane_n, *insidePoints[0].vertices, *outsidePoints[0].vertices));

                outPolygons[1]->ChangeData(0, *insidePoints[1].vertices, *insidePoints[1].texels);
                outPolygons[1]->ChangeData(1, outPolygons[0]->vertices[2], outPolygons[0]->textureCoordinates[2]);
                outPolygons[1]->SliceData(2, insidePoints[1].vertices, outsidePoints[0].vertices, insidePoints[1].texels, outsidePoints[0].texels, VectorIntersectPlane(plane_p, plane_n, *insidePoints[1].vertices, *outsidePoints[0].vertices));

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
            float fFovRad = GMath::DegreesToRadians(90.f);

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
            }
        }

        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix)
        {
            for (int i = 0; i < 3; ++i)
            {
                oPolygon.vertices[i] = MultiplyMatrixVector(iPolygon.vertices[i], matrix);
            }
        }

        void OffsetPolygonDepth(Polygon& oTriangle, float offset)
        {
            for (int i = 0; i < 3; ++i)
            {
                oTriangle.vertices[i].z = oTriangle.vertices[i].z + offset;
            }
        }

        float DegreesToRadians(float angles)
        {
            return angles * PI / 180.f;
        }

        float RadiansToDegrees(float radians)
        {
            return radians * 180.f / PI;
        }
	}
}