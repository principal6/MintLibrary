#include <stdafx.h>
#include <MintRendering/Include/SplineRenderer.h>

#include <MintRenderingBase/Include/ShapeFontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        MINT_INLINE Float2 lerp(const Float2& a, const Float2& b, const float t) noexcept
        {
            return a * (1.0f - t) + b * t;
        }

        
        SplineRenderer::DebugOptions::DebugOptions()
            : _drawControlPoints{ false }
            , _controlPointColor{ Color::kBlue }
            , _controlPointSize{ 2.0f }
        {
            __noop;
        }

        SplineRenderer::SplineRenderer(ShapeFontRendererContext& rendererContext)
            : _rendererContext{ &rendererContext }
            , _precision{ 16 }
        {
            __noop;
        }

        SplineRenderer::~SplineRenderer()
        {
            __noop;
        }

        const bool SplineRenderer::drawBezierCurve(const Vector<Float2>& controlPoints, const float thickness, const Color& color) noexcept
        {
            MINT_ASSURE(_precision > 0);

            const uint32 order = controlPoints.size() - 1;
            MINT_ASSURE(order > 0);

            _rendererContext->setColor(color);

            const float t0 = 0.0f;
            const float t1 = 1.0f;
            const float tStep = t1 / static_cast<float>(_precision);
            const uint32 stepCount = _precision;
            for (uint32 stepIndex = 0; stepIndex < stepCount; ++stepIndex)
            {
                //const Float2& p0 = getDeCasteljauPoint(controlPoints, t0 + tStep * stepIndex);
                //const Float2& p1 = getDeCasteljauPoint(controlPoints, t0 + tStep * (stepIndex + 1));
                const Float2& p0 = getBezierPoint(controlPoints, t0 + tStep * stepIndex);
                const Float2& p1 = getBezierPoint(controlPoints, t0 + tStep * (stepIndex + 1));
                _rendererContext->drawLine(p0, p1, thickness);
            }

            drawControlPoints(controlPoints);
            return true;
        }

        float SplineRenderer::computePower(const float base, const uint32 exponent) const noexcept
        {
            float result = 1.0f;
            for (uint32 iter = 0; iter < exponent; iter++)
            {
                result *= base;
            }
            return result;
        }

        uint32 SplineRenderer::computeCombination(const uint32 totalCount, const uint32 selectionCount) const noexcept
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

        Float2 SplineRenderer::getBezierPoint(const Vector<Float2>& controlPoints, const float t) const noexcept
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

        Float2 SplineRenderer::getDeCasteljauPoint(const Vector<Float2>& controlPoints, const float t) const noexcept
        {
            // TODO: Stack Vector 구현으로 바꾸면 훨씬 성능에 나을 듯

            if (controlPoints.size() < 2)
            {
                MINT_NEVER;
            }

            if (controlPoints.size() == 2)
            {
                return lerp(controlPoints[0], controlPoints[1], t);
            }

            const uint32 order = controlPoints.size() - 1;
            Vector<Float2> result;
            result.reserve(order - 1);
            for (uint32 orderIter = 0; orderIter < order; ++orderIter)
            {
                result.push_back(lerp(controlPoints[orderIter], controlPoints[orderIter + 1], t));
            }

            return getDeCasteljauPoint(result, t);
        }

        void SplineRenderer::drawControlPoints(const Vector<Float2>& controlPoints) noexcept
        {
            if (_debugOptions._drawControlPoints == false)
            {
                return;  
            }

            _rendererContext->setColor(_debugOptions._controlPointColor);

            const uint32 controlPointCount = controlPoints.size();
            for (uint32 controlPointIndex = 0; controlPointIndex < controlPointCount; ++controlPointIndex)
            {
                _rendererContext->setPosition(Float4(controlPoints[controlPointIndex]._x, controlPoints[controlPointIndex]._y, 0.0f, 1.0f));
                _rendererContext->drawCircle(_debugOptions._controlPointSize);
            }
        }

        void SplineRenderer::render() noexcept
        {
            _rendererContext->render();
            _rendererContext->flush();
        }
    }
}
