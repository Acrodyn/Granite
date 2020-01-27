#pragma once

namespace Granite
{
    namespace GUtil
    {
        template <typename T>
        T Abs(T number)
        {
            return ((float)number >= 0.f ? number : -number);
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
    }
}