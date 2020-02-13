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

            FVector3 operator-(const FVector3& other) const
            {
                return FVector3(x - other.x, y - other.y, z - other.z);
            }

            FVector3 operator+(const FVector3& other) const
            {
                return FVector3(x + other.x, y + other.y, z + other.z);
            }

            FVector3& operator+=(const FVector3& other)
            {
                x += other.x;
                y += other.y;
                z += other.z;

                return *this;
            }

            FVector3& operator-=(const FVector3& other)
            {
                x -= other.x;
                y -= other.y;
                z -= other.z;

                return *this;
            }

            template<typename T>
            FVector3 operator*(T scalar) const
            {
                return FVector3(x * scalar, y * scalar, z * scalar);
            }

            void operator=(const FVector3& vector)
            {
                this->x = vector.x;
                this->y = vector.y;
                this->z = vector.z;
            }

            void SetData(float x, float y, float z)
            {
                this->x = x;
                this->y = y;
                this->z = z;
            }

            void SetData(const FVector3& vector)
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

            FVector3 ReturnNormalized()
            {
                FVector3 normalizedVector = *this;

                float length = GetLength();

                normalizedVector.x /= length;
                normalizedVector.y /= length;
                normalizedVector.z /= length;

                return normalizedVector;
            }

            void Normalize()
            {
                float length = GetLength();

                x /= length;
                y /= length;
                z /= length;
            }

            float DotProduct(const FVector3& vector) const
            {
                return x * vector.x + y * vector.y + z * vector.z;
            }

            FVector3 CrossProduct(const FVector3& vector) const
            {
                return FVector3(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
            }
        };
	}
}