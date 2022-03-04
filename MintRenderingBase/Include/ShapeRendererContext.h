#pragma once


#ifndef MINT_SHAPE_RENDERER_CONTEXT_H
#define MINT_SHAPE_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/IRendererContext.h>

#include <MintMath/Include/Int2.h>


namespace mint
{
    namespace Rendering
    {
        // All draw functions use LowLevelRenderer::pushRenderCommandIndexed()
        class ShapeRendererContext : public IRendererContext
        {
        protected:
            enum class ShapeType : uint8
            {
                QuadraticBezierTriangle,
                SolidTriangle,
                Circular,
                DoubleCircular,
                TexturedTriangle, // Assumes four channels are used
                FontTriangle, // Assumes single channel is used
            };

        public:
                            ShapeRendererContext(GraphicDevice& graphicDevice);
            virtual         ~ShapeRendererContext();

        public:
            virtual void    initializeShaders() noexcept override;
            virtual void    flush() noexcept override;
            virtual void    render() noexcept;

        public:
            void            setBorderColor(const Color& borderColor) noexcept;

        public:
            virtual void    testDraw(Float2&& screenOffset);
            virtual void    testDraw(Float2& screenOffset);

        public:
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void            drawQuadraticBezier(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const bool validate = true);
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void            drawSolidTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC);
            void            drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
            void            drawQuarterCircle(const float radius, const float rotationAngle);
            // This function Interprets internal positon as the center of the entire circle (= center root of half circle)
            void            drawHalfCircle(const float radius, const float rotationAngle);
            void            drawCircle(const float radius, const bool insideOut = false);
            void            drawDoughnut(const float outerRadius, const float innerRadius);
            // arcAngle = [0, +pi]
            void            drawCircularArc(const float radius, const float arcAngle, const float rotationAngle);
            // arcAngle = [0, +pi]
            void            drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);
            void            drawRectangle(const Float2& size, const float borderThickness, const float rotationAngle);
            void            drawTexturedRectangle(const Float2& size, const float rotationAngle);
            void            drawTaperedRectangle(const Float2& size, const float tapering, const float bias, const float rotationAngle);
            void            drawRoundedRectangle(const Float2& size, const float roundness, const float borderThickness, const float rotationAngle);
            void            drawHalfRoundedRectangle(const Float2& size, const float roundness, const float rotationAngle);
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void            drawLine(const Float2& p0, const Float2& p1, const float thickness);
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            const bool      drawLineStrip(const Vector<Float2>& points, const float thickness);

        public:
            // This function is slow...!!!
            void            drawColorPalleteXXX(const float radius);

        protected:
            void            drawQuadraticBezierInternal(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const Color& color, const bool validate = true);
            void            drawSolidTriangleInternal(const Float2& pointA, const Float2& pointB, const Float2& pointC, const Color& color);
            void            drawQuarterCircleInternal(const Float2& offset, const float halfRadius, const Color& color);
            void            drawRectangleInternal(const Float2& offset, const Float2& halfSize, const Color& color, const ShapeType shapeType = ShapeType::SolidTriangle);
            void            drawRoundedRectangleInternal(const float radius, const Float2& halfSize, const float roundness, const Color& color);
            void            drawHalfRoundedRectangleInternal(const float radius, const Float2& halfSize, const float roundness, const Color& color);
            void            drawLineInternal(const Float2& p0, const Float2& p1, const float thickness);
        
        protected:
            const float     packInfoAsFloat(const ShapeType shapeType) const noexcept;
            void            pushTransformToBuffer(const float rotationAngle, const bool applyInternalPosition = true);

        protected:
            DxObjectID      _vertexShaderID;
            DxObjectID      _geometryShaderID;
            DxObjectID      _pixelShaderID;
            Color           _borderColor;
        };
    }
}


#endif // !MINT_SHAPE_RENDERER_CONTEXT_H
