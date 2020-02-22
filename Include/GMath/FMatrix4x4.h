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

            mutable float data[4][4] = { 0 };

            void MakeIdentity();
            void operator=(const FMatrix4x4& other) const;
            FMatrix4x4 operator*(const FMatrix4x4& other) const;
            void GetRotation(float &Yaw, float &Pitch, float &Roll) const;
            bool IsIdentity() const;

            /* float &operator ()(int row, int column) const
             {
                 return matrix[row][column];
             }*/
        };
	}
}