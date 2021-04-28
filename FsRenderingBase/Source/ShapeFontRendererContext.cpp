#include <stdafx.h>
#include <FsRenderingBase/Include/ShapeFontRendererContext.h>

#include <FsContainer/Include/Vector.hpp>
#include <FsContainer/Include/StringUtil.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/LowLevelRenderer.hpp>


namespace fs
{
    namespace RenderingBase
    {
        ShapeFontRendererContext::ShapeFontRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice)
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

        const bool ShapeFontRendererContext::hasData() const noexcept
        {
            return (__super::hasData() || _fontRendererContext.hasData());
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

        void ShapeFontRendererContext::renderAndFlush() noexcept
        {
            render();

            flush();
        }

        void ShapeFontRendererContext::setViewportIndex(const uint32 viewportIndex) noexcept
        {
            __super::setViewportIndex(viewportIndex);

            _fontRendererContext.setViewportIndex(viewportIndex);
        }

        const bool ShapeFontRendererContext::initializeFontData(const FontRendererContext::FontData& fontData) noexcept
        {
            return _fontRendererContext.loadFontData(fontData);
        }

        const FontRendererContext::FontData& ShapeFontRendererContext::getFontData() const noexcept
        {
            return _fontRendererContext.getFontData();
        }

        void ShapeFontRendererContext::drawDynamicText(const wchar_t* const wideText, const fs::Float4& position, const FontRenderingOption& fontRenderingOption)
        {
            _fontRendererContext.drawDynamicText(wideText, position, fontRenderingOption);
        }

        void ShapeFontRendererContext::drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const fs::Float4& position, const FontRenderingOption& fontRenderingOption)
        {
            _fontRendererContext.drawDynamicText(wideText, textLength, position, fontRenderingOption);
        }

        const float ShapeFontRendererContext::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
        {
            return _fontRendererContext.calculateTextWidth(wideText, textLength);
        }

        const uint32 ShapeFontRendererContext::calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
        {
            return _fontRendererContext.calculateIndexFromPositionInText(wideText, textLength, positionInText);
        }

        void ShapeFontRendererContext::setTextColor(const fs::RenderingBase::Color& textColor) noexcept
        {
            _fontRendererContext.setColor(textColor);
        }

    }
}
