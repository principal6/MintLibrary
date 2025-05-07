#pragma once


#ifndef _MINT_RENDERING_BASE_FONT_RENDERER_H_
#define _MINT_RENDERING_BASE_FONT_RENDERER_H_


#include <MintRenderingBase/Include/IRenderer.h>
#include <MintRenderingBase/Include/FontLoader.h>


namespace mint
{
	class BitVector;
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

		class FontRenderer final : public IRenderer
		{
			friend GraphicsDevice;

		public:
			virtual ~FontRenderer();

		public:
			virtual void InitializeShaders() noexcept override;
			void Terminate() noexcept;
			virtual void Render() noexcept override;
			virtual void Flush() noexcept override;

		public:
			bool InitializeFontData(const FontData& fontData);
			void SetTextColor(const Color& textColor) noexcept;

		public:
			void DrawDynamicText(const wchar_t* const wideText, const Float2& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
			void DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
		
		public:
			virtual bool IsEmpty() const noexcept override;
			const FontData& GetFontData() const noexcept { return _fontData; }

		private:
			FontRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData);
			FontRenderer(const FontRenderer& rhs) = delete;
			FontRenderer(FontRenderer&& rhs) = delete;

		private:
			Float3 ComputePostTranslation(const wchar_t* const wideText, const uint32 textLength, const FontRenderingOption& fontRenderingOption) const;
			void DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);
			uint32 ComputeVertexInfo(uint32 transformIndex, uint8 type) const;
			// Doesn't apply coordinate space internally!!!
			void PushManualTransformToBuffer(const Float3& preTranslation, const Float4x4& transformMatrix, const Float3& postTranslation);
			const char* GetDefaultVertexShaderString() const;
			const char* GetDefaultGeometryShaderString() const;
			const char* GetDefaultPixelShaderString() const;

		private:
			GraphicsObjectID _shaderPipelineMultipleViewportID;
			GraphicsObjectID _defaultMaterialID;
			FontData _fontData;
			Color _textColor;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_FONT_RENDERER_H_
