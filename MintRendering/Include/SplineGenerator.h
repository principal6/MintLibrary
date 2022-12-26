#pragma once


#ifndef _MINT_RENDERING_SPLINE_GENERATOR_H_
#define _MINT_RENDERING_SPLINE_GENERATOR_H_


#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
    namespace Rendering
    {
        // TODO
        // Hermite splines
        // Catmull-Rom splines
        // Cardinal splines (tension)
        // Kochanek-Bartels splines (bias, continuity)
        class SplineGenerator
        {
        public:
                        SplineGenerator();
                        ~SplineGenerator();

        public:
            bool        setPrecision(const uint32 precision) noexcept;

        public:
            bool        generateBezierCurve(const Vector<Float2>& controlPoints, Vector<Float2>& outLinePoints) noexcept;
            bool        generateBSpline(const uint32 order, const Vector<Float2>& controlPoints, const Vector<float>& knotVector, Vector<Float2>& outLinePoints) noexcept;

        public:
            Float2      computeBezierPoint(const Vector<Float2>& controlPoints, const float t) const noexcept;
            Float2      computeDeCasteljauPoint(const Vector<Float2>& controlPoints, const float t) const noexcept;
            Float2      computeBSplinePoint(const uint32 order, const Vector<Float2>& controlPoints, const Vector<float>& knotVector, const float t) const noexcept;

        private:
            float       computePower(const float base, const uint32 exponent) const noexcept;
            uint32      computeCombination(const uint32 totalCount, const uint32 selectionCount) const noexcept;
            // B-Spline Basis Function = N_i_j(t)
            //  i = control point index
            //  j = order index
            float       evaluateBSplineBasisFunction(const uint32 i, const uint32 j, const Vector<float>& knotVector, const float t) const noexcept;

        private:
            uint32      _precision;
        };
    }
}


#endif // !_MINT_RENDERING_SPLINE_GENERATOR_H_
