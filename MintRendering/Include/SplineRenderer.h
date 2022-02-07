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
        class ShapeFontRendererContext;
        

        class SplineRenderer
        {
        public:
            struct DebugOptions
            {
            public:
                            DebugOptions();

            public:
                bool        _drawControlPoints;
                Color       _controlPointColor;
                float       _controlPointSize;
            };

        public:
                                                SplineRenderer(ShapeFontRendererContext& rendererContext);
                                                ~SplineRenderer();

        public:
            MINT_INLINE void                    setPrecision(const uint32 precision) noexcept { _precision = precision; }
            MINT_INLINE void                    setDebugOptions(const DebugOptions& debugOptions) noexcept { _debugOptions = debugOptions; }

        public:
            const bool                          drawBezierCurve(const Vector<Float2>& controlPoints, const float thickness, const Color& color = Color::kBlack) noexcept;

        private:
            Float2                              getDeCasteljauPoint(const Vector<Float2>& controlPoints, const float t) const noexcept;
            void                                drawControlPoints(const Vector<Float2>& controlPoints) noexcept;

        public:
            void                                render() noexcept;

        private:
            ShapeFontRendererContext* const     _rendererContext;
            uint32                              _precision;
            DebugOptions                        _debugOptions;
        };
    }
}


#endif // !MINT_RENDERING_SPLINE_RENDERER_H
