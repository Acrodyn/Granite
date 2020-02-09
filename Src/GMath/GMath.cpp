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

        FMatrix4x4 GetPointAtMatrix(FVector3& pos, FVector3& target, FVector3& up)
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

            float fNear = 0.1f;
            float fFar = 1000.0f;
            float fFov = 90.0f;
            float fAspectRatio = (float)GConfig::WINDOW_HEIGHT / (float)GConfig::WINDOW_WIDTH;
            float fFovRad = 1.0f / tanf(fFov * 0.5f / 180.0f * 3.14159f);

            projectionMatrix.data[0][0] = fAspectRatio * fFovRad;
            projectionMatrix.data[1][1] = fFovRad;
            projectionMatrix.data[2][2] = fFar / (fFar - fNear);
            projectionMatrix.data[3][2] = (-fFar * fNear) / (fFar - fNear);
            projectionMatrix.data[2][3] = 1.0f;
            projectionMatrix.data[3][3] = 0.0f;

            return projectionMatrix;
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