#pragma once


#ifndef MINT_FONT_RENDERER_CONTEXT_H
#define MINT_FONT_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/IRendererContext.h>
#include <MintRenderingBase/Include/FontLoader.h>


namespace mint
{
#pragma region Forward declaration
    class BitVector;
#pragma endregion


    namespace Rendering
    {
        class GraphicDevice;


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

            TextRenderDirectionHorz     _directionHorz;
            TextRenderDirectionVert     _directionVert;
            float                       _scale;
            bool                        _drawShade;
            Float4x4                    _transformMatrix;
        };

        // All draw functions use LowLevelRenderer::pushRenderCommandIndexed()
        class FontRendererContext final : public IRendererContext
        {
            static constexpr int32  kVertexCountPerGlyph = 4;
            static constexpr int32  kIndexCountPerGlyph = 6;

        public:
                                    FontRendererContext(GraphicDevice& graphicDevice);
                                    FontRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer);
            virtual                 ~FontRendererContext();

        public:
            const bool              initialize(const FontData& fontData);
            const FontData&         getFontData() const noexcept;

        public:
            virtual void            initializeShaders() noexcept override final;
            virtual void            flush() noexcept override final;
            virtual void            render() noexcept final;

        public:
            void                    drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                    drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                    drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            void                    drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            const float             computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32            computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;

        public:
            void                    pushTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation);

        private:
            void                    drawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);

        private:
            void                    prepareIndexArray();

        private:
            FontData                _fontData;

        private:
            DxObjectID              _vertexShaderID;
            DxObjectID              _geometryShaderID;
            DxObjectID              _pixelShaderID;
        };
    }
}


#endif // !MINT_FONT_RENDERER_CONTEXT_H
