#pragma once

namespace Granite
{
    namespace GMath
    {
        class FVector3;
        class FMatrix4x4;
        class Polygon;


        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix);

        FMatrix4x4 GetXRotation(float angle);
        FMatrix4x4 GetYRotation(float angle);
        FMatrix4x4 GetZRotation(float angle);

        FMatrix4x4 GetPointAtMatrix(FVector3 &po, FVector3 &target, FVector3 &up);
        FMatrix4x4 GetInverseMatrix(FMatrix4x4& matrix);
        FMatrix4x4 GetProjectionMatrix();
        FMatrix4x4 GetTranslationMatrix(float x, float y, float z);

        void MultiplyMatrixPolygon(Polygon& polygon, const FMatrix4x4& matrix);
        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix);
        void OffsetPolygonDepth(Polygon& oTriangle, float offset);
        float AnglesToRadians(float angles);

        const float PI = 3.14159265359f;
    }
}