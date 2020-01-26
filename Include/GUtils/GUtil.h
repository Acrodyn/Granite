#pragma once

namespace Granite
{
    namespace GUtil
    {
        template <typename T>
        int Abs(T number)
        {
            return (number >= 0 ? number : -number);
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