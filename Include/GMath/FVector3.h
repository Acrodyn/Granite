#pragma once
#include <math.h>

namespace Granite
{
	namespace GMath
	{
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
	}
}