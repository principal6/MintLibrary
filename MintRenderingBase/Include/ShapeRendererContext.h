#pragma once


#ifndef _MINT_RENDERING_BASE_SHAPE_RENDERER_CONTEXT_H_
#define _MINT_RENDERING_BASE_SHAPE_RENDERER_CONTEXT_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StackVector.h>

#include <MintRenderingBase/Include/IRendererContext.h>
#include <MintRenderingBase/Include/FontLoader.h>


namespace mint
{
	class BitVector;


	namespace Rendering
	{
		struct FontRenderingOption
		{
			FontRenderingOption()
				: FontRenderingOption(TextRenderDirectionHorz::Rightward, TextRenderDirectionVert::Downward, 1.0f)
			{
				__noop;
			}
			FontRenderingOption(const TextRenderDirectionHorz directionHorz, const TextRenderDirectionVert directionVert)
				: FontRenderingOption(directionHorz, directionVert, 1.0f)
			{
				__noop;
			}
			FontRenderingOption(const TextRenderDirectionHorz directionHorz, const TextRenderDirectionVert directionVert, const float scale)
				: _directionHorz{ directionHorz }
				, _directionVert{ directionVert }
				, _scale{ scale }
				, _drawShade{ false }
			{
				__noop;
			}

			TextRenderDirectionHorz _directionHorz;
			TextRenderDirectionVert _directionVert;
			float _scale;
			bool _drawShade;
			Float4x4 _transformMatrix;
		};


		// All draw functions use LowLevelRenderer::PushRenderCommandIndexed()
		class ShapeRendererContext : public IRendererContext
		{
		public:
			struct Split
			{
				Split() : Split(0.0f, Color::kTransparent) { __noop; }
				Split(const float ratio, const Color& color) : _ratio{ ratio }, _color{ color } { __noop; }

				float _ratio;
				Color _color;
			};

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
			virtual ~ShapeRendererContext();

		public:
			virtual void InitializeShaders() noexcept override;
			virtual void Flush() noexcept override;
			virtual void Render() noexcept override;

		protected:
			const char* GetDefaultVertexShaderString() const;
			const char* GetDefaultGeometryShaderString() const;

		public:
			bool InitializeFontData(const FontData& fontData);
			const FontData& GetFontData() const noexcept { return _fontData; }

		public:
			void SetShapeBorderColor(const Color& shapeBorderColor) noexcept;
			void SetTextColor(const Color& textColor) noexcept;

		public:
			virtual void TestDraw(Float2&& screenOffset);
			virtual void TestDraw(Float2& screenOffset);

		public:
			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawLine(const Float2& p0, const Float2& p1, const float thickness);
			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			bool DrawLineStrip(const Vector<Float2>& points, const float thickness);

			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawSolidTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC);
			void DrawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut = false);

			void DrawRectangle(const Float2& size, const float borderThickness, const float rotationAngle);
			void DrawTexturedRectangle(const Float2& size, const float rotationAngle);
			void DrawTaperedRectangle(const Float2& size, const float tapering, const float bias, const float rotationAngle);
			void DrawRoundedRectangle(const Float2& size, const float roundness, const float borderThickness, const float rotationAngle);
			void DrawRoundedRectangleVertSplit(const Float2& size, const float roundnessInPixel, const StackVector<Split, 3>& splits, const float rotationAngle);
			void DrawHalfRoundedRectangle(const Float2& size, const float roundness, const float rotationAngle);

			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawQuadraticBezier(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const bool validate = true);
			void DrawQuarterCircle(const float radius, const float rotationAngle);
			// This function Interprets internal positon as the center of the entire circle (= center root of half circle)
			void DrawHalfCircle(const float radius, const float rotationAngle);
			void DrawCircle(const float radius, const bool insideOut = false);
			void DrawEllipse(const float xRadius, const float yRadius, const float rotationAngle);
			void DrawDoughnut(const float outerRadius, const float innerRadius);
			// arcAngle = [0, +pi]
			void DrawCircularArc(const float radius, const float arcAngle, const float rotationAngle);
			// arcAngle = [0, +pi]
			void DrawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle);

		public:
			// This function is slow...!!!
			void DrawColorPalleteXXX(const float radius);

			// Font
		public:
			void DrawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);

		public:
			float ComputeNormalizedRoundness(const float minSize, const float roundnessInPixel) const;

			// Shape
		protected:
			void DrawLineInternal(const Float2& p0, const Float2& p1, const float thickness);
			void DrawSolidTriangleInternal(const Float2& pointA, const Float2& pointB, const Float2& pointC, const Color& color);
			void DrawRectangleInternal(const Float2& offset, const Float2& halfSize, const Color& color, const ShapeType shapeType);
			void DrawRoundedRectangleInternal(const float radius, const Float2& halfSize, const Color& color);
			void DrawUpperHalfRoundedRectangleInternal(const Float2& offset, const Float2& size, const float roundness, const Color& color);
			void DrawLowerHalfRoundedRectangleInternal(const Float2& offset, const Float2& size, const float roundness, const Color& color);
			void DrawQuadraticBezierInternal(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const Color& color, const bool validate = true);
			void DrawQuarterCircleInternal(const Float2& offset, const float halfRadius, const Color& color);

			// Font
		protected:
			void DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);

		protected:
			void PushShapeTransformToBuffer(const float rotationAngle, const bool applyInternalPosition = true);
			void PushFontTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation);
			float PackInfoAsFloat(const ShapeType shapeType) const noexcept;

		protected:
			GraphicObjectID _inputLayoutID;
			GraphicObjectID _vertexShaderID;
			GraphicObjectID _geometryShaderID;
			GraphicObjectID _pixelShaderID;

			Color _shapeBorderColor;

			Color _textColor;
			FontData _fontData;

			Float2 _uv0 = Float2(0, 0);
			Float2 _uv1 = Float2(1, 1);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_RENDERER_CONTEXT_H_
