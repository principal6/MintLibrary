#pragma once


#ifndef FS_SHAPE_FONT_RENDERER_CONTEXT_H
#define FS_SHAPE_FONT_RENDERER_CONTEXT_H


#include <FsRenderingBase/Include/ShapeRendererContext.h>
#include <FsRenderingBase/Include/FontRendererContext.h>


namespace fs
{
	namespace RenderingBase
	{
		class ShapeFontRendererContext final : public ShapeRendererContext
		{
		public:
																			ShapeFontRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice);
			virtual															~ShapeFontRendererContext();

		public:
			virtual void													initializeShaders() noexcept override final;
			virtual void													flushData() noexcept override final;
			virtual const bool												hasData() const noexcept override final;
			virtual void													render() noexcept final;
			virtual void													setViewportIndex(const uint32 viewportIndex) noexcept override final;

		public:
			const bool														initializeFontData(const FontRendererContext::FontData& fontData) noexcept;
			const FontRendererContext::FontData&							getFontData() const noexcept;
			void															drawDynamicText(const wchar_t* const wideText, const fs::Float4& position, const TextRenderDirectionHorz directionHorz = TextRenderDirectionHorz::Rightward, const TextRenderDirectionVert directionVert = TextRenderDirectionVert::Downward, const float scale = 1.0f, const bool drawShade = false);
			void															drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const fs::Float4& position, const TextRenderDirectionHorz directionHorz = TextRenderDirectionHorz::Rightward, const TextRenderDirectionVert directionVert = TextRenderDirectionVert::Downward, const float scale = 1.0f, const bool drawShade = false);
			const float														calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
			const uint32													calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
			void															setTextColor(const fs::RenderingBase::Color& textColor) noexcept;

		private:
			fs::RenderingBase::FontRendererContext						_fontRendererContext;
		};
	}
}


#endif // !FS_SHAPE_FONT_RENDERER_CONTEXT_H
