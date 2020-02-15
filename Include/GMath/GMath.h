#pragma once
#include "FVector3.h"

namespace Granite
{
    namespace GMath
    {
        //class FVector3;
        class FMatrix4x4;
        class Polygon;

        FVector3 MultiplyMatrixVector(const FVector3& vector, const FMatrix4x4& matrix);

        FMatrix4x4 GetXRotation(float angle);
        FMatrix4x4 GetYRotation(float angle);
        FMatrix4x4 GetZRotation(float angle);

        FVector3 GetUpVector();
        FVector3 GetForwardVector();
        FVector3 GetRightVector();

        FVector3 VectorIntersectPlane(const FVector3 &plane_p, const FVector3 &plane_n, FVector3 &lineStart, FVector3 &lineEnd);
        int ClipAgainstPlane(const FVector3 &plane_p, const FVector3 &plane_n, Polygon &inPoly, Polygon **outPolygons);

        FMatrix4x4 GetPointAtMatrix(const FVector3 &po, const FVector3 &target, const FVector3 &up);
        FMatrix4x4 GetInverseMatrix(FMatrix4x4& matrix);
        FMatrix4x4 GetProjectionMatrix();
        FMatrix4x4 GetTranslationMatrix(float x, float y, float z);

        void MultiplyMatrixPolygon(Polygon& polygon, const FMatrix4x4& matrix);
        void MultiplyMatrixPolygon(const Polygon& iPolygon, Polygon& oPolygon, const FMatrix4x4& matrix);
        void OffsetPolygonDepth(Polygon& oTriangle, float offset);
        float AnglesToRadians(float angles);

        const float PI = 3.1415926535898f;

        static const FVector3 ZeroVector(0.f, 0.f, 0.f);
        static const FVector3 RightVector(1.f, 0.f, 0.f);
        static const FVector3 LeftVector(-1.f, 0.f, 0.f);
        static const FVector3 UpVector(0.f, 1.f, 0.f);
        static const FVector3 DownVector(0.f, -1.f, 0.f);
        static const FVector3 ForwardVector(0.f, 0.f, 1.f);
        static const FVector3 BackwardVector(0.f, 0.f, -1.f);
    }
}