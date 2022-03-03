#include <stdafx.h>
#include <MintRenderingBase/Include/ShapeFontRendererContext.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
    namespace Rendering
    {
        ShapeFontRendererContext::ShapeFontRendererContext(GraphicDevice& graphicDevice)
            : ShapeRendererContext(graphicDevice)
            , _fontRendererContext{ graphicDevice }
        {
            __noop;
        }

        ShapeFontRendererContext::~ShapeFontRendererContext()
        {
            __noop;
        }

        void ShapeFontRendererContext::initializeShaders() noexcept
        {
            __super::initializeShaders();

            _fontRendererContext.initializeShaders();
        }

        void ShapeFontRendererContext::flush() noexcept
        {
            __super::flush();

            _fontRendererContext.flush();
        }

        void ShapeFontRendererContext::render() noexcept
        {
            __super::render();

            _fontRendererContext.render();
        }

        void ShapeFontRendererContext::setClipRect(const Rect& clipRect) noexcept
        {
            __super::setClipRect(clipRect);

            _fontRendererContext.setClipRect(clipRect);
        }

        const bool ShapeFontRendererContext::initializeFont(const FontData& fontData) noexcept
        {
            return _fontRendererContext.initialize(fontData);
        }

        const FontData& ShapeFontRendererContext::getFontData() const noexcept
        {
            return _fontRendererContext.getFontData();
        }

        void ShapeFontRendererContext::testDraw(Float2&& screenOffset)
        {
            testDraw(screenOffset);
        }

        void ShapeFontRendererContext::testDraw(Float2& screenOffset)
        {
            __super::testDraw(screenOffset);

            setTextColor(Color(0, 40, 80));

            FontRenderingOption fontRenderingOption;
            drawDynamicText(L"Testing`!@#$%^&*()_+ °Ë»ç Áß...", Float4(screenOffset), fontRenderingOption);
        }

        void ShapeFontRendererContext::drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption)
        {
            _fontRendererContext.drawDynamicText(wideText, position, fontRenderingOption);
        }

        void ShapeFontRendererContext::drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption)
        {
            _fontRendererContext.drawDynamicText(wideText, textLength, position, fontRenderingOption);
        }

        void ShapeFontRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
        {
            _fontRendererContext.drawDynamicTextBitFlagged(wideText, position, fontRenderingOption, bitFlags);
        }

        void ShapeFontRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
        {
            _fontRendererContext.drawDynamicTextBitFlagged(wideText, textLength, position, fontRenderingOption, bitFlags);
        }

        const float ShapeFontRendererContext::computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _fontRendererContext.computeTextWidth(wideText, textLength);
        }

        const uint32 ShapeFontRendererContext::computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _fontRendererContext.computeIndexFromPositionInText(wideText, textLength, positionInText);
        }

        void ShapeFontRendererContext::setTextColor(const Color& textColor) noexcept
        {
            _fontRendererContext.setColor(textColor);
        }

    }
}
