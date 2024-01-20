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
		struct Shape;


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

		public:
			ShapeRendererContext(GraphicDevice& graphicDevice);
			virtual ~ShapeRendererContext();

		public:
			virtual void InitializeShaders() noexcept override;
			virtual bool IsEmpty() const noexcept override;
			virtual void Flush() noexcept override;
			virtual void Render() noexcept override;

		protected:
			const char* GetDefaultVertexShaderString() const;
			const char* GetDefaultGeometryShaderString() const;
			const char* GetDefaultPixelShaderString() const;

		public:
			bool InitializeFontData(const FontData& fontData);
			const FontData& GetFontData() const noexcept { return _fontData; }

		public:
			void SetTextColor(const Color& textColor) noexcept;

		public:
			void AddShape(const Shape& shape);

			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawLine(const Float2& p0, const Float2& p1, const float thickness);

			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawLineStrip(const Vector<Float2>& points, const float thickness);

			// Independent from internal position set by SetPosition() call
			// No rotation allowed
			void DrawArrow(const Float2& begin, const Float2& end, const float thickness, float headLengthRatio, float headWidthRatio);

			void DrawTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC);
			void DrawRectangle(const Float2& size, const float borderThickness, const float rotationAngle);
			void DrawCircle(const float radius);

			// Font
		public:
			void DrawDynamicText(const wchar_t* const wideText, const Float2& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);

			// Font
		protected:
			void DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);

		protected:
			uint32 ComputeVertexInfo(uint32 transformIndex, uint8 type) const;
			void PushShapeTransformToBuffer(const float rotationAngle, const bool applyInternalPosition = true);
			void PushFontTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation);

		protected:
			GraphicObjectID _inputLayoutID;
			GraphicObjectID _vertexShaderID;
			GraphicObjectID _geometryShaderID;
			GraphicObjectID _pixelShaderID;

			Color _textColor;
			FontData _fontData;

			Float2 _uv0 = Float2(0, 0);
			Float2 _uv1 = Float2(1, 1);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_RENDERER_CONTEXT_H_
