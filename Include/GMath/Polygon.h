#pragma once

#include "FVector3.h"

namespace Granite
{
	namespace GMath
	{
        class Polygon
        {
        public:

            FVector3 vertices[3];
            FVector3 textureCoordinates[3];
            FVector3 normals[3];

            void UniformMove(float scalar);
            void UniformScale(float scalar);
        };
	}
}