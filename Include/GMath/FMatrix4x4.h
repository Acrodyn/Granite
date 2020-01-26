#pragma once

namespace Granite
{
	namespace GMath
	{
        struct FMatrix4x4
        {
            float matrix[4][4] = { 0 };

            /* float &operator ()(int row, int column) const
             {
                 return matrix[row][column];
             }*/
        };
	}
}