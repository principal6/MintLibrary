#pragma once


#ifndef MINT_RENDERING_SPLINE_RENDERER_H
#define MINT_RENDERING_SPLINE_RENDERER_H


#include <MintContainer/Include/Vector.h>

#include <MintMath/Include/Float2.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
    namespace Rendering
    {
        class SplineGenerator
        {
        public:
                                                SplineGenerator();
                                                ~SplineGenerator();

        public:
            const bool                          setPrecision(const uint32 precision) noexcept;

        public:
            const bool                          generateBezierCurve(const Vector<Float2>& controlPoints, Vector<Float2>& outLinePoints) noexcept;

        private:
            float                               computePower(const float base, const uint32 exponent) const noexcept;
            uint32                              computeCombination(const uint32 totalCount, const uint32 selectionCount) const noexcept;
            Float2                              getBezierPoint(const Vector<Float2>& controlPoints, const float t) const noexcept;
            Float2                              getDeCasteljauPoint(const Vector<Float2>& controlPoints, const float t) const noexcept;

        private:
            uint32                              _precision;
        };
    }
}


#endif // !MINT_RENDERING_SPLINE_RENDERER_H
