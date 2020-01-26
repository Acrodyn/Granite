#pragma once

namespace Granite
{
    namespace Util
    {
        int Abs(int number)
        {
            return (number >= 0 ? number : -number);
        }

        void Swap(int& a, int& b)
        {
            int temp = a;
            a = b;
            b = temp;
        }
    }
}