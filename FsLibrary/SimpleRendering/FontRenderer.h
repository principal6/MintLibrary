#pragma once


#ifndef FS_BITMAP_FONT_H
#define FS_BITMAP_FONT_H


#include <CommonDefinitions.h>

#include <Math/Float2.h>

#include <SimpleRendering/IRenderer.h>
#include <SimpleRendering/TriangleBuffer.h>

#include <File/BinaryFile.h>


typedef struct FT_Glyph_Metrics_	FT_Glyph_Metrics;
typedef struct FT_LibraryRec_*		FT_Library;
typedef struct FT_FaceRec_*			FT_Face;


namespace fs
{
	namespace SimpleRendering
	{
		class GraphicDevice;
		class FontRenderer;


		using GlyphMetricType = int8;
		class GlyphInfo
		{
			friend FontRenderer;

		public:
									GlyphInfo();
									GlyphInfo(const wchar_t charCode, const FT_Glyph_Metrics* const ftGlyphMetrics);

		private:
			wchar_t					_charCode;
			GlyphMetricType			_width;
			GlyphMetricType			_height;
			GlyphMetricType			_horiBearingX;
			GlyphMetricType			_horiBearingY;
			GlyphMetricType			_horiAdvance;
			fs::Float2				_uv0;
			fs::Float2				_uv1;
		};

		class GlyphRange
		{
			friend FontRenderer;

		public:
									GlyphRange();
									GlyphRange(const wchar_t startWchar, const wchar_t endWchar);

		public:
			const bool				operator<(const GlyphRange& rhs) const noexcept;

		private:
			wchar_t					_startWchar;
			wchar_t					_endWchar;
		};


		class FontRenderer final : public IRenderer
		{
			static constexpr const char* const							kFontFileExtension = ".fnt";

		public:
																		FontRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice);
																		~FontRenderer();

		public:
			void														pushGlyphRange(const GlyphRange& glyphRange);

		public:
			const bool													loadFont(const char* const fontFileName);
			const bool													bakeFont(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop);

		private:
			const bool													initializeFreeType(const char* const fontFaceFileName, const int16 fontSize);
			const bool													deinitializeFreeType();
		
		private:
			const bool													bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, std::vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY);
			void														completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight);
			void														writeMetaData(const int16 textureWidth, const int16 textureHeight, fs::BinaryFileWriter& binaryFileWriter) const noexcept;

		public:
			virtual void												initializeShaders() noexcept override final;
			virtual void												flushData() noexcept override final;
			virtual void												render() noexcept final;

		public:
			void														drawDynamicText(const wchar_t* const wideText, const fs::Int2& position);
		
		private:
			void														drawGlyph(const wchar_t wideChar, fs::Int2& position);

		private:
			void														prepareIndexArray();

		private:
			FT_Library													_ftLibrary;
			FT_Face														_ftFace;
			int16														_fontSize;
			std::vector<GlyphRange>										_glyphRangeArray;
		
		private:
			std::vector<GlyphInfo>										_glyphInfoArray;
			std::unordered_map<wchar_t, uint64>							_glyphMap;
			ComPtr<ID3D11ShaderResourceView>							_fontTextureSrv;

		private:
			fs::SimpleRendering::TriangleBuffer<CppHlsl::VS_INPUT>		_rendererBuffer;
			DxObjectId													_vertexShader;
			DxObjectId													_pixelShader;
		};
	}
}


#endif // !FS_BITMAP_FONT_H
