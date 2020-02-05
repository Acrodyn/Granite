#pragma once

namespace Granite
{
	namespace GMath
	{
        class FMatrix4x4
        {
        public:
            FMatrix4x4();
            ~FMatrix4x4();

            float data[4][4] = { 0 };

            FMatrix4x4 operator*(const FMatrix4x4& other);

            /* float &operator ()(int row, int column) const
             {
                 return matrix[row][column];
             }*/
        };
	}
}