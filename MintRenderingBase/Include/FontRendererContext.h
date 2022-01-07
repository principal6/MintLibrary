#pragma once


#ifndef MINT_FONT_RENDERER_CONTEXT_H
#define MINT_FONT_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>

#include <MintRenderingBase/Include/RenderingBaseCommon.h>
#include <MintRenderingBase/Include/IRendererContext.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>


typedef struct FT_Glyph_Metrics_    FT_Glyph_Metrics;
typedef struct FT_LibraryRec_*      FT_Library;
typedef struct FT_FaceRec_*         FT_Face;


namespace mint
{
#pragma region Forward declaration
    class BitVector;
    class BinaryFileWriter;

    template <typename T>
    class Vector;
#pragma endregion


    namespace Rendering
    {
        class GraphicDevice;
        class FontRendererContext;

        using GlyphMetricType = int8;
        class GlyphInfo
        {
            friend FontRendererContext;

        public:
                                    GlyphInfo();
                                    GlyphInfo(const wchar_t charCode, const FT_Glyph_Metrics* const ftGlyphMetrics);

        private:
            wchar_t                 _charCode;
            GlyphMetricType         _width;
            GlyphMetricType         _height;
            GlyphMetricType         _horiBearingX;
            GlyphMetricType         _horiBearingY;
            GlyphMetricType         _horiAdvance;
            Float2                  _uv0;
            Float2                  _uv1;
        };

        class GlyphRange
        {
            friend FontRendererContext;

        public:
                                    GlyphRange();
                                    GlyphRange(const wchar_t startWchar, const wchar_t endWchar);

        public:
            const bool              operator<(const GlyphRange& rhs) const noexcept;

        private:
            wchar_t                 _startWchar;
            wchar_t                 _endWchar;
        };

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

        class FontRendererContext final : public IRendererContext
        {
            static constexpr int16                                              kSpaceBottomForVisibility = 1;
            static_assert(kSpaceBottomForVisibility == 1, "kSpaceBottomForVisibility must be 1");
            static constexpr int16                                              kSpaceBottom = 1;
            static_assert(kSpaceBottomForVisibility <= kSpaceBottom, "kSpaceBottom must be greater than or equal to kSpaceBottomForVisibility");
            static constexpr const char* const                                  kFontFileExtension = ".fnt";
            static constexpr const char* const                                  kFontFileMagicNumber = "FNT";
            static constexpr int32                                              kVertexCountPerGlyph = 4;
            static constexpr int32                                              kIndexCountPerGlyph = 6;

        public:
            struct FontData
            {
                const uint32                getSafeGlyphIndex(const wchar_t wideChar) const noexcept;

                Vector<GlyphInfo>           _glyphInfoArray;
                Vector<uint32>              _charCodeToGlyphIndexMap;
                DxObjectID                  _fontTextureID;
            };

        public:
                                                FontRendererContext(GraphicDevice& graphicDevice);
                                                FontRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const triangleRenderer);
            virtual                             ~FontRendererContext();

        public:
            void                                pushGlyphRange(const GlyphRange& glyphRange) noexcept;

        public:
            const bool                          existsFontData(const char* const fontFileName) const noexcept;

        private:
            const std::string                   getFontDataFileNameWithExtension(const char* const fontFileName) const noexcept;
            const bool                          existsFontDataInternal(const char* const fontFileNameWithExtension) const noexcept;

        public:
            const bool                          loadFontData(const char* const fontFileName);
            const bool                          loadFontData(const FontData& fontData);
            const bool                          bakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop);
            const FontData&                     getFontData() const noexcept;
            const int16                         getFontSize() const noexcept;

        private:
            const bool                          initializeFreeType(const char* const fontFaceFileName, const int16 fontSize);
            const bool                          deinitializeFreeType();
        
        private:
            const bool                          bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY);
            void                                completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight);
            void                                writeMetaData(const int16 textureWidth, const int16 textureHeight, BinaryFileWriter& binaryFileWriter) const noexcept;

        public:
            virtual void                        initializeShaders() noexcept override final;
            virtual const bool                  hasData() const noexcept override final;
            virtual void                        flush() noexcept override final;
            virtual void                        render() noexcept final;
            virtual void                        renderAndFlush() noexcept final;

        public:
            void                                drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            void                                drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags);
            const float                         computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                        computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
        
        public:
            void                                pushTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation);
            const DxObjectID&                   getFontTextureID() const noexcept;

        private:
            void                                drawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace);

        private:
            void                                prepareIndexArray();

        private:
            FT_Library                          _ftLibrary;
            FT_Face                             _ftFace;
            int16                               _fontSize;
            Vector<GlyphRange>                  _glyphRangeArray;
        
        private:
            FontData                            _fontData;

        private:
            bool                                _ownTriangleRenderer;
            LowLevelRenderer<VS_INPUT_SHAPE>*   _lowLevelRenderer;
            DxObjectID                          _vertexShaderID;
            DxObjectID                          _geometryShaderID;
            DxObjectID                          _pixelShaderID;
        };
    }
}


#endif // !MINT_FONT_RENDERER_CONTEXT_H
