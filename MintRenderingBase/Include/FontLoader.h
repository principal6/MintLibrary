#pragma once


#ifndef _MINT_RENDERING_BASE_FONT_LOADER_H_
#define _MINT_RENDERING_BASE_FONT_LOADER_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/String.h>
#include <MintMath/Include/Float2.h>


typedef struct FT_Glyph_Metrics_ FT_Glyph_Metrics;
typedef struct FT_LibraryRec_* FT_Library;
typedef struct FT_FaceRec_* FT_Face;


namespace mint
{
	class BinaryFileWriter;


	namespace Rendering
	{
		struct FontData;
		class GraphicDevice;
		class FontLoader;
		class ShapeRendererContext;


		using GlyphMetricType = int8;


		class GlyphInfo
		{
			friend FontLoader;
			friend FontData;
			friend ShapeRendererContext;

		public:
			GlyphInfo();
			GlyphInfo(const wchar_t charCode, const FT_Glyph_Metrics* const ftGlyphMetrics);

		private:
			wchar_t _charCode;
			GlyphMetricType _width;
			GlyphMetricType _height;
			GlyphMetricType _horiBearingX;
			GlyphMetricType _horiBearingY;
			GlyphMetricType _horiAdvance;
			Float2 _uv0;
			Float2 _uv1;
		};

		class GlyphRange
		{
			friend FontLoader;

		public:
			GlyphRange();
			GlyphRange(const wchar_t startWchar, const wchar_t endWchar);

		public:
			bool operator<(const GlyphRange& rhs) const noexcept;

		private:
			wchar_t _startWchar;
			wchar_t _endWchar;
		};

		struct FontData
		{
			friend FontLoader;

			uint32 GetSafeGlyphIndex(const wchar_t wideChar) const noexcept;
			float ComputeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;
			uint32 ComputeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept;

			Vector<GlyphInfo> _glyphInfoArray;
			GraphicObjectID _fontTextureID;
			int16 _fontSize = 0;

		private:
			Vector<uint32> _charCodeToGlyphIndexMap;
		};


		class FontLoader
		{
			static constexpr int16 kSpaceBottomForVisibility = 1;
			static_assert(kSpaceBottomForVisibility == 1, "kSpaceBottomForVisibility must be 1");
			static constexpr int16 kSpaceBottom = 1;
			static_assert(kSpaceBottomForVisibility <= kSpaceBottom, "kSpaceBottom must be greater than or equal to kSpaceBottomForVisibility");
			static constexpr const char* const kFontFileExtension = ".fnt";
			static constexpr const char* const kFontFileMagicNumber = "FNT";

		public:
			FontLoader();
			~FontLoader();

		public:
			static bool ExistsFont(const char* const fontFileNameRaw);
			static StringA GetFontFileNameWithExtension(const char* const fontFileName) noexcept;

		public:
			void PushGlyphRange(const GlyphRange& glyphRange) noexcept;

		public:
			bool LoadFont(const char* const fontFileNameRaw, GraphicDevice& graphicDevice);

		public:
			bool BakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop);

		public:
			const FontData& GetFontData() const { return _fontData; }

		private:
			bool InitializeFreeType(const char* const fontFaceFileName);
			void DeinitializeFreeType();

			bool BakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY);
			void CompleteGlyphInfoArray(const int16 textureWidth, const int16 textureHeight);
			void WriteMetaData(const int16 textureWidth, const int16 textureHeight, BinaryFileWriter& binaryFileWriter) const noexcept;

		private:
			FT_Library _ftLibrary;
			FT_Face _ftFace;
			Vector<GlyphRange> _glyphRanges;

		private:
			FontData _fontData;
			struct FontImageData
			{
				Vector<byte> _imageData;
				int16 _width = 0;
				int16 _height = 0;
			};
			FontImageData _fontImageData;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_FONT_LOADER_H_
