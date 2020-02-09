#include "GMath/FMatrix4x4.h"

namespace Granite
{
    namespace GMath
    {
        FMatrix4x4::FMatrix4x4()
        {
            /*data[0][0] = 1;
            data[1][1] = 1;
            data[2][2] = 1;
            data[3][3] = 1;*/
        }

        FMatrix4x4::~FMatrix4x4()
        {

        }

        void FMatrix4x4::MakeIdentity()
        {
            data[0][0] = 1.f;
            data[1][1] = 1.f;
            data[2][2] = 1.f;
            data[3][3] = 1.f;
        }

        void FMatrix4x4::operator=(const FMatrix4x4& other) const
        {
            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    data[i][j] = other.data[i][j];
                }
            }
        }

        FMatrix4x4 FMatrix4x4::operator*(const FMatrix4x4& other) const
        {
            FMatrix4x4 newMatrix;

            for (int i = 0; i < 4; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    newMatrix.data[i][j] = 0;

                    for (int k = 0; k < 4; k++)
                    {
                        newMatrix.data[i][j] += data[i][k] * other.data[k][j];
                    }
                }
            }

            return newMatrix;
        }
    }
}
