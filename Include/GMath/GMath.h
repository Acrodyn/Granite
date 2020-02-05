#pragma once

namespace Granite
{
    namespace GMath
    {
        class FVector3;
        class FMatrix4x4;
        class Polygon;


        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix);

        void MultiplyMatrixPolygon(Polygon& polygon, const FMatrix4x4& matrix);
        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix);
        void OffsetPolygonDepth(Polygon& oTriangle, float offset);
    }
}