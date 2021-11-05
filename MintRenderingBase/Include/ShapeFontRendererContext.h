#pragma once


#ifndef MINT_SHAPE_FONT_RENDERER_CONTEXT_H
#define MINT_SHAPE_FONT_RENDERER_CONTEXT_H


#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/FontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        class ShapeFontRendererContext final : public ShapeRendererContext
        {
        public:
                                                    ShapeFontRendererContext(GraphicDevice* const graphicDevice);
            virtual                                 ~ShapeFontRendererContext();

        public:
            virtual void                            initializeShaders() noexcept override final;
            virtual const bool                      hasData() const noexcept override final;
            virtual void                            flush() noexcept override final;
            virtual void                            render() noexcept final;
            virtual void                            renderAndFlush() noexcept final;

        public:
            virtual void                            setClipRect(const Rect& clipRect) noexcept override final;

        public:
            const bool                              initializeFontData(const FontRendererContext::FontData& fontData) noexcept;
            const bool                              initializeFontData(const char* const fontFileName) noexcept;
            const bool                              existsFontData(const char* const fontFileName) const noexcept;
            void                                    pushGlyphRange(const GlyphRange& glyphRange) noexcept;
            const bool                              bakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop);
            const FontRendererContext::FontData&    getFontData() const noexcept;

        public:
            void                                    drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                    drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                    drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            void                                    drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            const float                             calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                            calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
            void                                    setTextColor(const Color& textColor) noexcept;

        private:
            FontRendererContext                     _fontRendererContext;
        };
    }
}


#endif // !MINT_SHAPE_FONT_RENDERER_CONTEXT_H
