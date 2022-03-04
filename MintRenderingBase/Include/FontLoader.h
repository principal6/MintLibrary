#pragma once


#ifndef _MINT_FONT_LOADER_H_
#define _MINT_FONT_LOADER_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/Float2.h>


typedef struct FT_Glyph_Metrics_    FT_Glyph_Metrics;
typedef struct FT_LibraryRec_*      FT_Library;
typedef struct FT_FaceRec_*         FT_Face;


namespace mint
{
    class BinaryFileWriter;


    namespace Rendering
    {
        class GraphicDevice;
        class FontLoader;
        class FontRendererContext;
        struct FontData;


        using GlyphMetricType = int8;


        class GlyphInfo
        {
            friend FontLoader;
            friend FontRendererContext;
            friend FontData;

        public:
                                GlyphInfo();
                                GlyphInfo(const wchar_t charCode, const FT_Glyph_Metrics* const ftGlyphMetrics);

        private:
            wchar_t             _charCode;
            GlyphMetricType     _width;
            GlyphMetricType     _height;
            GlyphMetricType     _horiBearingX;
            GlyphMetricType     _horiBearingY;
            GlyphMetricType     _horiAdvance;
            Float2              _uv0;
            Float2              _uv1;
        };

        class GlyphRange
        {
            friend FontLoader;

        public:
                            GlyphRange();
                            GlyphRange(const wchar_t startWchar, const wchar_t endWchar);

        public:
            const bool      operator<(const GlyphRange& rhs) const noexcept;

        private:
            wchar_t         _startWchar;
            wchar_t         _endWchar;
        };

        struct FontData
        {
            friend FontLoader;

            const uint32        getSafeGlyphIndex(const wchar_t wideChar) const noexcept;
            const float         computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
            const uint32        computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;

            Vector<GlyphInfo>   _glyphInfoArray;
            DxObjectID          _fontTextureID;
            int16               _fontSize;

        private:
            Vector<uint32>      _charCodeToGlyphIndexMap;
        };


        class FontLoader
        {
            static constexpr int16              kSpaceBottomForVisibility = 1;
            static_assert(kSpaceBottomForVisibility == 1, "kSpaceBottomForVisibility must be 1");
            static constexpr int16              kSpaceBottom = 1;
            static_assert(kSpaceBottomForVisibility <= kSpaceBottom, "kSpaceBottom must be greater than or equal to kSpaceBottomForVisibility");
            static constexpr const char* const  kFontFileExtension = ".fnt";
            static constexpr const char* const  kFontFileMagicNumber = "FNT";

        public:
                                FontLoader();
                                ~FontLoader();

        public:
            static const bool   doesExistFont(const char* const fontFileNameRaw);
            static std::string  getFontFileNameWithExtension(const char* const fontFileName) noexcept;
        
        public:
            void                pushGlyphRange(const GlyphRange& glyphRange) noexcept;
        
        public:
            const bool          loadFont(const char* const fontFileNameRaw, GraphicDevice& graphicDevice);
        
        public:
            const bool          bakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop);
        
        public:
            const FontData&     getFontData() const { return _fontData; }

        private:
            const bool          initializeFreeType(const char* const fontFaceFileName);
            void                deinitializeFreeType();
            
            const bool          bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY);
            void                completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight);
            void                writeMetaData(const int16 textureWidth, const int16 textureHeight, BinaryFileWriter& binaryFileWriter) const noexcept;

        private:
            FT_Library          _ftLibrary;
            FT_Face             _ftFace;
            Vector<GlyphRange>  _glyphRanges;

        private:
            FontData            _fontData;
            struct FontImageData
            {
                Vector<byte>    _imageData;
                int16           _width = 0;
                int16           _height = 0;
            };
            FontImageData       _fontImageData;
        };
    }
}


#endif // !_MINT_FONT_LOADER_H_