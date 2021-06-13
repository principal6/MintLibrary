#pragma once


#ifndef MINT_SHAPE_FONT_RENDERER_CONTEXT_H
#define MINT_SHAPE_FONT_RENDERER_CONTEXT_H


#include <MintRenderingBase/Include/ShapeRendererContext.h>
#include <MintRenderingBase/Include/FontRendererContext.h>


namespace mint
{
    namespace RenderingBase
    {
        class ShapeFontRendererContext final : public ShapeRendererContext
        {
        public:
                                                        ShapeFontRendererContext(mint::RenderingBase::GraphicDevice* const graphicDevice);
            virtual                                     ~ShapeFontRendererContext();

        public:
            virtual void                                initializeShaders() noexcept override final;
            virtual const bool                          hasData() const noexcept override final;
            virtual void                                flush() noexcept override final;
            virtual void                                render() noexcept final;
            virtual void                                renderAndFlush() noexcept final;

        public:
            virtual void                                setViewportIndex(const uint32 viewportIndex) noexcept override final;

        public:
            const bool                                  initializeFontData(const FontRendererContext::FontData& fontData) noexcept;
            const FontRendererContext::FontData&        getFontData() const noexcept;
            void                                        drawDynamicText(const wchar_t* const wideText, const mint::Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                        drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const mint::Float4& position, const FontRenderingOption& fontRenderingOption);
            const float                                 calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                                calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
            void                                        setTextColor(const mint::RenderingBase::Color& textColor) noexcept;

        private:
            mint::RenderingBase::FontRendererContext      _fontRendererContext;
        };
    }
}


#endif // !MINT_SHAPE_FONT_RENDERER_CONTEXT_H
