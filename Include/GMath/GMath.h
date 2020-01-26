#pragma once

#include "FVector3.h"
#include "Mesh.h"
#include "FVector3.h"
#include "FMatrix4x4.h"
#include "IPoint.h"
#include "Polygon.h"

namespace Granite
{
    namespace GMath
    {
        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix);

        void MultiplyMatrixPolygon(Polygon& polygon, const FMatrix4x4& matrix);
        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix);
        void OffsetPolygonDepth(Polygon& oTriangle, float offset);
    }
}