#pragma once


#ifndef _MINT_RENDERING_BASE_SHAPE_RENDERER_H_
#define _MINT_RENDERING_BASE_SHAPE_RENDERER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StackVector.h>

#include <MintRenderingBase/Include/IRenderer.h>
#include <MintRenderingBase/Include/FontLoader.h>


namespace mint
{
	class BitVector;
	struct Transform;
	struct Transform2D;

	namespace Rendering
	{
		struct Shape;
	}
}

namespace mint
{
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
		class ShapeRenderer : public IRenderer
		{
			friend GraphicsDevice;

		public:
			struct Split
			{
				Split() : Split(0.0f, Color::kTransparent) { __noop; }
				Split(const float ratio, const Color& color) : _ratio{ ratio }, _color{ color } { __noop; }

				float _ratio;
				Color _color;
			};

		public:
			virtual ~ShapeRenderer();

		public:
			virtual void InitializeShaders() noexcept override;
			void Terminate() noexcept;
			virtual void Render() noexcept override;
			virtual void Flush() noexcept override;

		public:
			MINT_INLINE GraphicsObjectID GetDefaultShaderPipelineID() const noexcept { return _shaderPipelineDefaultID; }

		public:
			bool InitializeFontData(const FontData& fontData);
			void SetTextColor(const Color& textColor) noexcept;
			void SetMaterial(const GraphicsObjectID& materialID) noexcept;

		public:
			void AddShape(const Shape& shape);
			void AddShape(const Shape& shape, const Transform& transform);
			void AddShape(const Shape& shape, const Transform2D& transform2D);

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
			
			void DrawRectangle(const Float3& position, const Float2& size);
			void DrawCircle(const Float3& position, const float radius);

			// Font
		public:
			void DrawDynamicText(const wchar_t* const wideText, const Float2& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);

		public:
			virtual bool IsEmpty() const noexcept override;
			const FontData& GetFontData() const noexcept { return _fontData; }

		protected:
			ShapeRenderer(GraphicsDevice& graphicsDevice);
			ShapeRenderer(const ShapeRenderer& rhs) = delete;
			ShapeRenderer(ShapeRenderer&& rhs) = delete;

			// Font
		protected:
			void DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);

		protected:
			const char* GetDefaultVertexShaderString() const;
			const char* GetDefaultGeometryShaderString() const;
			const char* GetDefaultPixelShaderString() const;
			virtual const char* GetPixelShaderName() const noexcept;
			virtual const char* GetPixelShaderString() const noexcept;
			virtual const char* GetPixelShaderEntryPoint() const noexcept;
			Float3 ComputePostTranslation(const wchar_t* const wideText, const uint32 textLength, const FontRenderingOption& fontRenderingOption) const;
			uint32 ComputeVertexInfo(uint32 transformIndex, uint8 type) const;
			void PushTransformToBuffer(const Transform2D& transform2D);
			void PushTransformToBuffer(const Float2& scale, const float rotationAngle, const Float3& position);
			void PushTransformToBuffer(const Transform& transform);
			// Doesn't apply coordinate space internally!!!
			void PushManualTransformToBuffer(const Float3& preTranslation, const Float4x4& transformMatrix, const Float3& postTranslation);

		private:
			void AddShape_Internal(const Shape& shape);

		protected:
			GraphicsObjectID _shaderPipelineDefaultID;
			GraphicsObjectID _shaderPipelineMultipleViewportID;

			Color _textColor;
			GraphicsObjectID _currentMaterialID;
			FontData _fontData;

			Float2 _uv0 = Float2(0, 0);
			Float2 _uv1 = Float2(1, 1);
		};
	}
}


#endif // !_MINT_RENDERING_BASE_SHAPE_RENDERER_H_
