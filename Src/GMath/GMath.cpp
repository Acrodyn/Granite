#include "GMath/GMath.h"

namespace Granite
{
	namespace GMath
	{
        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix)
        {
            FVector3 multipliedVector;

            multipliedVector.x = vector.x * matrix.matrix[0][0] + vector.y * matrix.matrix[1][0] + vector.z * matrix.matrix[2][0] + matrix.matrix[3][0];
            multipliedVector.y = vector.x * matrix.matrix[0][1] + vector.y * matrix.matrix[1][1] + vector.z * matrix.matrix[2][1] + matrix.matrix[3][1];
            multipliedVector.z = vector.x * matrix.matrix[0][2] + vector.y * matrix.matrix[1][2] + vector.z * matrix.matrix[2][2] + matrix.matrix[3][2];
            float w = vector.x * matrix.matrix[0][3] + vector.y * matrix.matrix[1][3] + vector.z * matrix.matrix[2][3] + matrix.matrix[3][3];

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
	}
}