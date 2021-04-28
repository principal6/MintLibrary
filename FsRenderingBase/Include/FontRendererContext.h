#pragma once


#ifndef FS_FONT_RENDERER_CONTEXT_H
#define FS_FONT_RENDERER_CONTEXT_H


#include <FsCommon/Include/CommonDefinitions.h>

#include <FsContainer/Include/Vector.h>

#include <FsMath/Include/Float2.h>

#include <FsRenderingBase/Include/RenderingBaseCommon.h>
#include <FsRenderingBase/Include/IRendererContext.h>
#include <FsRenderingBase/Include/LowLevelRenderer.h>

#include <FsPlatform/Include/BinaryFile.h>


typedef struct FT_Glyph_Metrics_    FT_Glyph_Metrics;
typedef struct FT_LibraryRec_*      FT_Library;
typedef struct FT_FaceRec_*         FT_Face;


namespace fs
{
    namespace RenderingBase
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
            fs::Float2              _uv0;
            fs::Float2              _uv1;
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

            const TextRenderDirectionHorz   _directionHorz;
            const TextRenderDirectionVert   _directionVert;
            const float                     _scale;
            const bool                      _drawShade;
            fs::Float4x4                    _transformMatrix;
        };

        class FontRendererContext final : public IRendererContext
        {
            static constexpr int16                                              kSpaceBottomForVisibility = 1;
            static_assert(kSpaceBottomForVisibility == 1, "kSpaceBottomForVisibility must be 1");
            static constexpr int16                                              kSpaceBottom = 1;
            static_assert(kSpaceBottomForVisibility <= kSpaceBottom, "kSpaceBottom must be greater than or equal to kSpaceBottomForVisibility");
            static constexpr const char* const                                  kFontFileExtension = ".fnt";
            static constexpr const char* const                                  kFontFileMagicNumber = "FNT";

        public:
            struct FontData
            {
                fs::Vector<GlyphInfo>   _glyphInfoArray;
                fs::Vector<uint32>      _charCodeToGlyphIndexMap;
                DxObjectId              _fontTextureId;
            };

        public:
                                                FontRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice);
                                                FontRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice, fs::RenderingBase::LowLevelRenderer<RenderingBase::VS_INPUT_SHAPE>* const triangleRenderer);
            virtual                             ~FontRendererContext();

        public:
            void                                pushGlyphRange(const GlyphRange& glyphRange);

        public:
            const bool                          existsFontData(const char* const fontFileName);

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
            const bool                          bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, fs::Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY);
            void                                completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight);
            void                                writeMetaData(const int16 textureWidth, const int16 textureHeight, fs::BinaryFileWriter& binaryFileWriter) const noexcept;

        public:
            virtual void                        initializeShaders() noexcept override final;
            virtual const bool                  hasData() const noexcept override final;
            virtual void                        flush() noexcept override final;
            virtual void                        render() noexcept final;
            virtual void                        renderAndFlush() noexcept final;

        public:
            void                                drawDynamicText(const wchar_t* const wideText, const fs::Float4& position, const FontRenderingOption& fontRenderingOption);
            void                                drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const fs::Float4& position, const FontRenderingOption& fontRenderingOption);
            const float                         calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32                        calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;
        
        public:
            void                                pushTransformToBuffer(const fs::Float4& position, fs::Float4x4 transformMatrix);
            const DxObjectId&                   getFontTextureId() const noexcept;

        private:
            void                                drawGlyph(const wchar_t wideChar, fs::Float2& glyphPosition, const float scale, const bool drawShade);

        private:
            void                                prepareIndexArray();

        private:
            FT_Library                          _ftLibrary;
            FT_Face                             _ftFace;
            int16                               _fontSize;
            fs::Vector<GlyphRange>              _glyphRangeArray;
        
        private:
            FontData                            _fontData;

        private:
            bool                                _ownTriangleRenderer;
            LowLevelRenderer<VS_INPUT_SHAPE>*   _lowLevelRenderer;
            DxObjectId                          _vertexShaderId;
            DxObjectId                          _geometryShaderId;
            DxObjectId                          _pixelShaderId;
        };
    }
}


#endif // !FS_FONT_RENDERER_CONTEXT_H
