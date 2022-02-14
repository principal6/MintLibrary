#include <stdafx.h>
#include <MintRendering/Include/SplineGenerator.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        SplineGenerator::SplineGenerator()
            : _precision{ 16 }
        {
            __noop;
        }

        SplineGenerator::~SplineGenerator()
        {
            __noop;
        }

        const bool SplineGenerator::setPrecision(const uint32 precision) noexcept
        {
            MINT_ASSURE(_precision > 0);
            
            _precision = precision;
            return true;
        }

        const bool SplineGenerator::generateBezierCurve(const Vector<Float2>& controlPoints, Vector<Float2>& outLinePoints) noexcept
        {
            MINT_ASSURE(controlPoints.size() > 1);

            const uint32 order = controlPoints.size() - 1;
            const float t0 = 0.0f;
            const float t1 = 1.0f;
            const float tStep = t1 / static_cast<float>(_precision);
            const uint32 stepCount = _precision;
            outLinePoints.clear();
            outLinePoints.reserve(stepCount + 1);
            outLinePoints.push_back(getDeCasteljauPoint(controlPoints, t0));
            //outLinePoints.push_back(getBezierPoint(controlPoints, t0));
            for (uint32 stepIndex = 0; stepIndex < stepCount; ++stepIndex)
            {
                outLinePoints.push_back(getDeCasteljauPoint(controlPoints, t0 + tStep * (stepIndex + 1)));
                //outLinePoints.push_back(getBezierPoint(controlPoints, t0 + tStep * (stepIndex + 1)));
            }
            return true;
        }

        float SplineGenerator::computePower(const float base, const uint32 exponent) const noexcept
        {
            float result = 1.0f;
            for (uint32 iter = 0; iter < exponent; iter++)
            {
                result *= base;
            }
            return result;
        }

        uint32 SplineGenerator::computeCombination(const uint32 totalCount, const uint32 selectionCount) const noexcept
        {
            if (selectionCount == 0 || selectionCount == totalCount)
            {
                return 1;
            }

            //    1 1
            //   1 2 1
            //  1 3 3 1
            // 1 4 6 4 1
            return computeCombination(totalCount - 1, selectionCount - 1) + computeCombination(totalCount - 1, selectionCount);
        }

        Float2 SplineGenerator::getBezierPoint(const Vector<Float2>& controlPoints, const float t) const noexcept
        {
            const float s = 1.0f - t;
            Float2 result = Float2::kZero;
            const uint32 controlPointCount = controlPoints.size();
            const uint32 order = controlPointCount - 1;
            for (uint32 controlPointIndex = 0; controlPointIndex < controlPointCount; controlPointIndex++)
            {
                const float coefficient = static_cast<float>(computeCombination(order, controlPointIndex));
                const Float2& controlPoint = controlPoints[controlPointIndex];
                result += (coefficient * computePower(s, order - controlPointIndex) * computePower(t, controlPointIndex) * controlPoint);
            }
            return result;
        }

        Float2 SplineGenerator::getDeCasteljauPoint(const Vector<Float2>& controlPoints, const float t) const noexcept
        {
            // TODO: Stack Vector 구현으로 바꾸면 훨씬 성능에 나을 듯

            if (controlPoints.size() < 2)
            {
                MINT_NEVER;
            }

            if (controlPoints.size() == 2)
            {
                return Math::lerp(controlPoints[0], controlPoints[1], t);
            }

            const uint32 order = controlPoints.size() - 1;
            Vector<Float2> result;
            result.reserve(order - 1);
            for (uint32 orderIter = 0; orderIter < order; ++orderIter)
            {
                result.push_back(Math::lerp(controlPoints[orderIter], controlPoints[orderIter + 1], t));
            }

            return getDeCasteljauPoint(result, t);
        }
    }
}
