#pragma once


#ifndef FS_BITMAP_FONT_H
#define FS_BITMAP_FONT_H


#include <CommonDefinitions.h>

#include <FsMath/Include/Float2.h>

#include <FsLibrary/SimpleRendering/SimpleRenderingCommon.h>
#include <FsLibrary/SimpleRendering/IRenderer.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.h>

#include <FsLibrary/File/BinaryFile.h>


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
			static constexpr int16										kSpaceBottomForVisibility = 1;
			static_assert(kSpaceBottomForVisibility == 1, "kSpaceBottomForVisibility must be 1");
			static constexpr int16										kSpaceBottom = 1;
			static_assert(kSpaceBottomForVisibility <= kSpaceBottom, "kSpaceBottom must be greater than or equal to kSpaceBottomForVisibility");
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
			void														drawDynamicText(const wchar_t* const wideText, const fs::Float2& position, const TextRenderDirectionHorz directionHorz = TextRenderDirectionHorz::Rightward, const TextRenderDirectionVert directionVert = TextRenderDirectionVert::Downward, const bool drawShade = false);
			const float													calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept;

		private:
			void														drawGlyph(const wchar_t wideChar, fs::Float2& position, const bool drawShade);

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

		private:
			fs::SimpleRendering::TriangleRenderer<CppHlsl::VS_INPUT>	_triangleRenderer;
			DxObjectId													_vertexShaderId;
			DxObjectId													_pixelShaderId;
			DxObjectId													_fontTextureId;
		};
	}
}


#endif // !FS_BITMAP_FONT_H
