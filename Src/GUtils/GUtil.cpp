#include "GUtils/GUtil.h"

namespace Granite
{
    namespace GUtil
    {
        float GInterpolate(float start, float end, float t)
        {
            return start * (1.f - t) + end * t;
        }
    }
}