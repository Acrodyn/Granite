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

    struct FVector3
    {
        float x, y, z;

       /* FVector3(float x, float y, float z)
        {
            this->x = x;
            this->y = y;
            this->z = z;
        }*/


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
    };

    struct Triangle
    {
        FVector3 vertices[3];
    };

    struct Mesh
    {
        std::vector<Triangle> polygonVertices;
        std::vector<Triangle> textureCoordinates;
        std::vector<Triangle> vertexNormals;
    };

    struct FMatrix4x4
    {
        mutable float matrix[4][4] = { 0 };

        float &operator ()(int row, int column) const
        {
            return matrix[row][column];
        }
    };

    FVector3 MultiplyMatrixVector(const FVector3 &vector, const FMatrix4x4 &matrix)
    {
        const float inVecData[4] = { vector.x, vector.y, vector.z, 1};
        float outVecData[4] = { 0 };

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                outVecData[i] += (matrix(j, i) * inVecData[j]);
            }
        }

        float w = outVecData[3];

        if (w != 0.0f)
        {
            for (int i = 0; i < 3; ++i)
            {
                outVecData[i] /= w;
            }
        }

        FVector3 multipliedVector;
        multipliedVector.x = outVecData[0];
        multipliedVector.y = outVecData[1];
        multipliedVector.z = outVecData[2];

        return multipliedVector;
    }
}