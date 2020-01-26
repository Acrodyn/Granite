#pragma once

namespace Granite
{
	namespace GMath
	{
        struct IPoint
        {
            int x, y;

            IPoint(int x, int y)
            {
                this->x = x;
                this->y = y;
            }
        };
	}
}