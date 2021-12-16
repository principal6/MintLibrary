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
            MINT_ASSURE(0 < _precision);

            const uint32 order = controlPoints.size() - 1;
            MINT_ASSURE(0 < order);

            _rendererContext->setColor(color);

            const float t0 = 0.0f;
            const float t1 = 1.0f;
            const float tStep = t1 / static_cast<float>(_precision);
            const uint32 stepCount = _precision;
            for (uint32 stepIndex = 0; stepIndex < stepCount; ++stepIndex)
            {
                const Float2& p0 = getDeCasteljauPoint(controlPoints, t0 + tStep * stepIndex);
                const Float2& p1 = getDeCasteljauPoint(controlPoints, t0 + tStep * (stepIndex + 1));
                _rendererContext->drawLine(p0, p1, thickness);
            }

            if (_debugOptions._drawControlPoints)
            {
                _rendererContext->setColor(_debugOptions._controlPointColor);

                const uint32 controlPointCount = controlPoints.size();
                for (uint32 controlPointIndex = 0; controlPointIndex < controlPointCount; ++controlPointIndex)
                {
                    _rendererContext->setPosition(Float4(controlPoints[controlPointIndex]._x, controlPoints[controlPointIndex]._y, 0.0f, 1.0f));
                    _rendererContext->drawCircle(_debugOptions._controlPointSize);
                }
            }
            return true;
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

        void SplineRenderer::render() noexcept
        {
            _rendererContext->renderAndFlush();
        }
    }
}
