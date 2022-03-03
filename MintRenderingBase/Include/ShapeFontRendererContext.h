#pragma once


#ifndef MINT_SHAPE_FONT_RENDERER_CONTEXT_H
#define MINT_SHAPE_FONT_RENDERER_CONTEXT_H


#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/FontRendererContext.h>


namespace mint
{
    namespace Rendering
    {
        struct FontData;


        class ShapeFontRendererContext final : public ShapeRendererContext
        {
        public:
                                ShapeFontRendererContext(GraphicDevice& graphicDevice);
            virtual             ~ShapeFontRendererContext();

        public:
            virtual void        initializeShaders() noexcept override final;
            virtual void        flush() noexcept override final;
            virtual void        render() noexcept final;

        public:
            virtual void        setClipRect(const Rect& clipRect) noexcept override final;

        public:
            const bool          initializeFont(const FontData& fontData) noexcept;
            const FontData&     getFontData() const noexcept;

        public:
            virtual void        testDraw(Float2&& screenOffset) override final;
            virtual void        testDraw(Float2& screenOffset) override final;

        public:
            void                drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            void                drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            void                setTextColor(const Color& textColor) noexcept;

        private:
            FontRendererContext _fontRendererContext;
        };
    }
}


#endif // !MINT_SHAPE_FONT_RENDERER_CONTEXT_H
