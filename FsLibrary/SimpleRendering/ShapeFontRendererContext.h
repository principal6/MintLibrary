#pragma once


#ifndef FS_SHAPE_FONT_RENDERER_CONTEXT_H
#define FS_SHAPE_FONT_RENDERER_CONTEXT_H


#include <FsLibrary/SimpleRendering/ShapeRendererContext.h>
#include <FsLibrary/SimpleRendering/FontRendererContext.h>


namespace fs
{
	namespace SimpleRendering
	{
		class ShapeFontRendererContext final : public ShapeRendererContext
		{
		public:
																			ShapeFontRendererContext(fs::SimpleRendering::GraphicDevice* const graphicDevice);
			virtual															~ShapeFontRendererContext();

		public:
			virtual void													initializeShaders() noexcept override final;
			virtual void													flushData() noexcept override final;
			virtual const bool												hasData() const noexcept override final;
			virtual void													render() noexcept final;
			virtual void													setViewportIndex(const uint32 viewportIndex) noexcept override final;

		public:
			const bool														initializeFont(const char* const fontFileName, const std::vector<fs::SimpleRendering::GlyphRange>& glyphRangeArray) noexcept;
			void															drawDynamicText(const wchar_t* const wideText, const fs::Float4& position, const TextRenderDirectionHorz directionHorz = TextRenderDirectionHorz::Rightward, const TextRenderDirectionVert directionVert = TextRenderDirectionVert::Downward, const float scale = 1.0f, const bool drawShade = false);
			const float														calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
			void															setTextColor(const fs::SimpleRendering::Color& textColor) noexcept;

		private:
			fs::SimpleRendering::FontRendererContext						_fontRendererContext;
		};
	}
}


#endif // !FS_SHAPE_FONT_RENDERER_CONTEXT_H
