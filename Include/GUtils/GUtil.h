#pragma once

namespace Granite
{
    namespace GUtil
    {
        const float MAX_FLOAT = 0x0 | 0x7F800000;

        template <typename T>
        T Abs(T number)
        {
            return ((float)number >= 0.f ? number : -number);
        }

        template <typename T>
        T Min(T number1, T number2)
        {
            return (number1 < number2) ? number1 : number2;
        }

        template <typename T>
        void Swap(T &a, T &b)
        {
            T temp = a;
            a = b;
            b = temp;
        }

        template <typename T>
        void Swap(T **a, T **b)
        {
            T *temp = *a;
            *a = *b;
            *b = temp;
        }

        template <typename T>
        T Clamp(T value, T min, T max)
        {
            if (value < min)
            {
                return min;
            }

            if (value > max)
            {
                return max;
            }

            return value;
        }

        float GInterpolate(float start, float end, float t);
    }
}