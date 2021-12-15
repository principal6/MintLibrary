#pragma once


#ifndef MINT_SHAPE_RENDERER_CONTEXT_H
#define MINT_SHAPE_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <MintRenderingBase/Include/IRendererContext.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintMath/Include/Int2.h>


namespace mint
{
    namespace Rendering
    {
        class ShapeRendererContext : public IRendererContext
        {
            enum class ShapeType : uint8
            {
                QuadraticBezierTriangle,
                SolidTriangle,
                Circular,
                Circle,
                TexturedTriangle,
            };

        public:
            static constexpr float              kRoundnessAbsoluteBase  = 2.0f;

        protected:
            static constexpr uint8              kInfoSolid              = 1;
            static constexpr uint8              kInfoCircular           = 2;

        public:
                                                ShapeRendererContext(GraphicDevice* const graphicDevice);
            virtual                             ~ShapeRendererContext();

        public:
            virtual void                        initializeShaders() noexcept override;
            virtual const bool                  hasData() const noexcept override;
            virtual void                        flush() noexcept override;
            virtual void                        render() noexcept;
            virtual void                        renderAndFlush() noexcept;

        public:
            void                                setBorderColor(const Color& borderColor) noexcept;

        public:
            virtual void                        testDraw(const Float2& screenOffset);

        public:
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void                                drawQuadraticBezier(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const bool validate = true);

        protected:
            void                                drawQuadraticBezierInternal(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const Color& color, const bool validate = true);
            
        public:
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void                                drawSolidTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC);

        protected:
            void                                drawSolidTriangleInternal(const Float2& pointA, const Float2& pointB, const Float2& pointC, const Color& color);

        public:
            void                                drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);
            void                                drawQuarterCircle(const float radius, const float rotationAngle);
    
        protected:
            void                                drawQuarterCircleInternal(const Float2& offset, const float halfRadius, const Color& color);

        public:
            // This function Interprets internal positon as the center of the entire circle (= center root of half circle)
            void                                drawHalfCircle(const float radius, const float rotationAngle);

        public:
            void                                drawCircle(const float radius, const bool insideOut = false);

            // arcAngle = [0, +pi]
            void                                drawCircularArc(const float radius, const float arcAngle, const float rotationAngle);

            // arcAngle = [0, +pi]
            void                                drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

        public:
            void                                drawRectangle(const Float2& size, const float borderThickness, const float rotationAngle);
            void                                drawTexturedRectangle(const Float2& size, const float rotationAngle);

        protected:
            void                                drawRectangleInternal(const Float2& offset, const Float2& halfSize, const Color& color, const ShapeType shapeType = ShapeType::SolidTriangle);

        public:
            void                                drawTaperedRectangle(const Float2& size, const float tapering, const float bias, const float rotationAngle);
            void                                drawRoundedRectangle(const Float2& size, const float roundness, const float borderThickness, const float rotationAngle);
            void                                drawHalfRoundedRectangle(const Float2& size, const float roundness, const float rotationAngle);

        protected:
            void                                drawRoundedRectangleInternal(const float radius, const Float2& halfSize, const float roundness, const Color& color);
            void                                drawHalfRoundedRectangleInternal(const float radius, const Float2& halfSize, const float roundness, const Color& color);

        public:
            // Independent from internal position set by setPosition() call
            // No rotation allowed
            void                                drawLine(const Float2& p0, const Float2& p1, const float thickness);

        protected:
            const float                         packShapeTypeAndTransformDataIndexAsFloat(const ShapeType shapeType) const noexcept;
            void                                pushTransformToBuffer(const float rotationAngle, const bool applyInternalPosition = true);

        public:
            // This function is slow...!!!
            void                                drawColorPalleteXXX(const float radius);

        protected:
            LowLevelRenderer<VS_INPUT_SHAPE>*   _lowLevelRenderer;
            DxObjectId                          _vertexShaderId;
            DxObjectId                          _geometryShaderId;
            DxObjectId                          _pixelShaderId;

        protected:
            Color                               _borderColor;
        };
    }
}


#endif // !MINT_SHAPE_RENDERER_CONTEXT_H
