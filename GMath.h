#pragma once

#include <vector>

namespace Granite
{
    struct IPoint
    {
        int x, y;

        IPoint(int x, int y)
        {
            this->x = x;
            this->y = y;
        }
    };

    class FVector3
    {
    public:
        float x, y, z;

        FVector3()
        {
            this->x = 0;
            this->y = 0;
            this->z = 0;
        }

        FVector3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        FVector3 operator-(FVector3 vector)
        {
            return (FVector3(x - vector.x, y - vector.y, z - vector.z));
        }

        void SetData(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }

        void SetData(FVector3 vector)
        {
            this->x = vector.x;
            this->y = vector.y;
            this->z = vector.z;
        }

        float GetLength()
        {
            return sqrtf(x * x + y * y + z * z);
        }

        float GetLengthSquared()
        {
            return x * x + y * y + z * z;
        }

        void Normalize()
        {
            float length = GetLength();

            x /= length;
            y /= length;
            z /= length;
        }

        float DotProduct(FVector3 vector)
        {
            return x * vector.x + y * vector.y + z * vector.z;
        }

        FVector3 CrossProduct(FVector3 vector)
        {
            return FVector3(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
        }
    };

    class Polygon
    {
    public:

        FVector3 vertices[3];
        FVector3 textureCoordinates[3];
        FVector3 normals[3];

        void UniformMove(float scalar)
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

        void Scale(float scalar)
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
    };

    struct Mesh
    {
        std::vector<Polygon> polygons;

        void AddPolygon(const FVector3 *vertices, const FVector3 *textureCoords, const FVector3 *normals)
        {
            Polygon newPolygon;

            for (int i = 0; i < 3; ++i)
            {
                newPolygon.vertices[i] = vertices[i];
                newPolygon.textureCoordinates[i] = textureCoords[i];
                newPolygon.normals[i] = normals[i];
            }

            polygons.push_back(newPolygon);
        }
    };

    struct FMatrix4x4
    {
        float matrix[4][4] = { 0 };

       /* float &operator ()(int row, int column) const
        {
            return matrix[row][column];
        }*/
    };

    FVector3 MultiplyMatrixVector(const FVector3 &vector, const FMatrix4x4 &matrix)
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
           // polygon.textureCoordinates[i] = MultiplyMatrixVector(polygon.textureCoordinates[i], matrix);
           // polygon.normals[i] = MultiplyMatrixVector(polygon.normals[i], matrix);
        }
    }

    void MultiplyMatrixPolygon(const Polygon &iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix)
    {
        for (int i = 0; i < 3; ++i)
        {
            oPolygon.vertices[i] = MultiplyMatrixVector(iPolygon.vertices[i], matrix);
            //oPolygon.textureCoordinates[i] = MultiplyMatrixVector(iPolygon.textureCoordinates[i], matrix);
            //oPolygon.normals[i] = MultiplyMatrixVector(iPolygon.normals[i], matrix);
        }
    }

    void OffsetPolygonDepth(Polygon& oTriangle, float offset)
    {
        for (int i = 0; i < 3; ++i)
        {
            oTriangle.vertices[i].z = oTriangle.vertices[i].z + offset;
            //oTriangle.textureCoordinates[i].z = oTriangle.textureCoordinates[i].z + offset;
            //oTriangle.normals[i].z = oTriangle.normals[i].z + offset;
        }
    }
}