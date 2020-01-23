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
    };

    struct Triangle
    {
        FVector3 vertices[3];
    };

    struct Mesh
    {
        std::vector<Triangle> planes;
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
        FVector3 multipliedVector;

        multipliedVector.x = vector.x * matrix.matrix[0][0] + vector.y * matrix.matrix[1][0] + vector.z * matrix.matrix[2][0] + matrix.matrix[3][0];
        multipliedVector.y = vector.x * matrix.matrix[0][1] + vector.y * matrix.matrix[1][1] + vector.z * matrix.matrix[2][1] + matrix.matrix[3][1];
        multipliedVector.z = vector.x * matrix.matrix[0][2] + vector.y * matrix.matrix[1][2] + vector.z * matrix.matrix[2][2] + matrix.matrix[3][2];
        float w = vector.x * matrix.matrix[0][3] + vector.y * matrix.matrix[1][3] + vector.z * matrix.matrix[2][3] + matrix.matrix[3][3];

        if (w != 0.0f)
        {
            multipliedVector.x /= w; multipliedVector.y /= w; multipliedVector.z /= w;
        }

        return multipliedVector;

        /*const float inVecData[4] = { vector.x, vector.y, vector.z, 1};
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

        return multipliedVector;*/

        //return FVector3(outVecData[0], outVecData[1], outVecData[2]);
    }
}