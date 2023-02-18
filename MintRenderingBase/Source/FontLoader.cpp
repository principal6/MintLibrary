#include <MintRenderingBase/Include/FontLoader.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintPlatform/Include/FileUtil.hpp>
#include <MintPlatform/Include/BinaryFile.hpp>
#include <MintLibrary/Include/Algorithm.hpp>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/ImageLoader.h>

#include <ft2build.h>
#include FT_FREETYPE_H


//#define MINT_FONT_RENDERER_SAVE_PNG_FOR_TEST
#define MINT_FONT_RENDERER_COMPRESS_AS_PNG


namespace mint
{
	namespace Rendering
	{
#pragma region GlyphInfo
		GlyphInfo::GlyphInfo()
			: _charCode{}
			, _width{}
			, _height{}
			, _horiBearingX{}
			, _horiBearingY{}
			, _horiAdvance{}
		{
			__noop;
		}

		GlyphInfo::GlyphInfo(const wchar_t charCode, const FT_Glyph_Metrics* const ftGlyphMetrics)
			: _charCode{ charCode }
			, _width{ static_cast<GlyphMetricType>(ftGlyphMetrics->width >> 6) }
			, _height{ static_cast<GlyphMetricType>(ftGlyphMetrics->height >> 6) }
			, _horiBearingX{ static_cast<GlyphMetricType>(ftGlyphMetrics->horiBearingX >> 6) }
			, _horiBearingY{ static_cast<GlyphMetricType>(ftGlyphMetrics->horiBearingY >> 6) }
			, _horiAdvance{ static_cast<GlyphMetricType>(ftGlyphMetrics->horiAdvance >> 6) }
		{
			__noop;
		}
#pragma endregion


#pragma region GlyphRange
		GlyphRange::GlyphRange()
			: _startWchar{ 0 }
			, _endWchar{ 0 }
		{
			__noop;
		}

		GlyphRange::GlyphRange(const wchar_t startWchar, const wchar_t endWchar)
			: _startWchar{ startWchar }
			, _endWchar{ endWchar }
		{
			__noop;
		}

		bool GlyphRange::operator<(const GlyphRange& rhs) const noexcept
		{
			return _startWchar < rhs._startWchar;
		}
#pragma endregion


#pragma region FontData
		uint32 FontData::getSafeGlyphIndex(const wchar_t wideChar) const noexcept
		{
			return (_charCodeToGlyphIndexMap.size() <= static_cast<uint32>(wideChar)) ? 0 : _charCodeToGlyphIndexMap[wideChar];
		}

		float FontData::computeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
		{
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];

				const uint32 glyphIndex = getSafeGlyphIndex(wideChar);
				const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;
			}
			return static_cast<float>(totalWidth);
		}

		uint32 FontData::computeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
		{
			const int32 positionInTextInt = static_cast<int32>(positionInText);
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];

				const uint32 glyphIndex = getSafeGlyphIndex(wideChar);
				const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;

				if (positionInTextInt < totalWidth)
				{
					return textAt;
				}
			}
			return textLength;
		}
#pragma endregion


		FontLoader::FontLoader()
			: _ftLibrary{ nullptr }
			, _ftFace{ nullptr }
		{
			__noop;
		}

		FontLoader::~FontLoader()
		{
			deinitializeFreeType();
		}

		bool FontLoader::doesExistFont(const char* const fontFileNameRaw)
		{
			const std::string fontFileName = getFontFileNameWithExtension(fontFileNameRaw);
			return FileUtil::exists(fontFileName.c_str());
		}

		std::string FontLoader::getFontFileNameWithExtension(const char* const fontFileName) noexcept
		{
			std::string fontFileNameWithExtension = fontFileName;
			StringUtil::excludeExtension(fontFileNameWithExtension);
			fontFileNameWithExtension.append(kFontFileExtension);
			return fontFileNameWithExtension;
		}

		void FontLoader::pushGlyphRange(const GlyphRange& glyphRange) noexcept
		{
			_glyphRanges.push_back(glyphRange);

			const uint32 glyphRangeCount = _glyphRanges.size();
			if (glyphRangeCount <= 1)
			{
				return;
			}

			quickSort(_glyphRanges, ComparatorAscending<GlyphRange>());

			Vector<uint32> deletionList;
			for (uint32 glyphRangeIndex = 1; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				GlyphRange& prev = _glyphRanges[glyphRangeIndex - 1];
				GlyphRange& curr = _glyphRanges[glyphRangeIndex];

				if (curr._startWchar <= prev._endWchar)
				{
					curr._startWchar = mint::Min(curr._startWchar, prev._startWchar);
					curr._endWchar = mint::Max(curr._endWchar, prev._endWchar);

					deletionList.push_back(glyphRangeIndex - 1);
				}
			}

			const uint32 deletionCount = deletionList.size();
			for (uint32 deletionIndex = 0; deletionIndex < deletionCount; ++deletionIndex)
			{
				_glyphRanges.erase(deletionList[deletionCount - deletionIndex - 1]);
			}
		}

		bool FontLoader::loadFont(const char* const fontFileNameRaw, GraphicDevice& graphicDevice)
		{
			const std::string fontFileName = getFontFileNameWithExtension(fontFileNameRaw);
			if (FileUtil::exists(fontFileName.c_str()) == false)
			{
				MINT_LOG_ERROR("해당 FontFile 이 존재하지 않습니다: %s", fontFileName.c_str());
				return false;
			}

			BinaryFileReader binaryFileReader;
			if (binaryFileReader.open(fontFileName.c_str()) == false)
			{
				MINT_LOG_ERROR("해당 FontFile 을 여는 데 실패했습니다: %s", fontFileName.c_str());
				return false;
			}

			const char kMagicNumber[4]{ *binaryFileReader.read<char>(), *binaryFileReader.read<char>(), *binaryFileReader.read<char>(), *binaryFileReader.read<char>() };
			if (StringUtil::compare(kMagicNumber, kFontFileMagicNumber) == false)
			{
				MINT_LOG_ERROR("%s 파일이 아닙니다!", kFontFileMagicNumber);
				return false;
			}

			_fontData._fontSize = *binaryFileReader.read<int16>();

			_fontImageData._width = *binaryFileReader.read<int16>();
			_fontImageData._height = *binaryFileReader.read<int16>();

			const uint32 glyphInfoCount = *binaryFileReader.read<uint32>();
			const uint32 charCodeToGlyphIndexMapSize = *binaryFileReader.read<uint32>();
			_fontData._glyphInfoArray.resize(glyphInfoCount);
			_fontData._charCodeToGlyphIndexMap.resize(charCodeToGlyphIndexMapSize);

			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._charCode = *binaryFileReader.read<wchar_t>();
				glyphInfo._width = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._height = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiBearingX = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiBearingY = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiAdvance = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._uv0._x = *binaryFileReader.read<float>();
				glyphInfo._uv0._y = *binaryFileReader.read<float>();
				glyphInfo._uv1._x = *binaryFileReader.read<float>();
				glyphInfo._uv1._y = *binaryFileReader.read<float>();

				_fontData._charCodeToGlyphIndexMap[glyphInfo._charCode] = glyphIndex;
			}

#if defined MINT_FONT_RENDERER_COMPRESS_AS_PNG
			const int32 pngLength = *binaryFileReader.read<int32>();
			Vector<byte> pngData(pngLength);
			for (int32 pngAt = 0; pngAt < pngLength; ++pngAt)
			{
				pngData[pngAt] = *binaryFileReader.read<byte>();
			}

			ImageLoader imageLoader;
			imageLoader.loadImage(1, pngData, _fontImageData._imageData);
#else
			const uint32 pixelCount = *binaryFileReader.read<uint32>();
			_fontImageData._imageData.resize(pixelCount);
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				_fontImageData._imageData[pixelIndex] = *binaryFileReader.read<byte>();
			}
#endif

			DxResourcePool& resourcePool = graphicDevice.getResourcePool();
			_fontData._fontTextureID = resourcePool.addTexture2D(DxTextureFormat::R8_UNORM, &_fontImageData._imageData[0], _fontImageData._width, _fontImageData._height);
			return true;
		}

		bool FontLoader::bakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop)
		{
			std::string fontFaceFileNameS = fontFaceFileName;
			if (StringUtil::hasExtension(fontFaceFileNameS) == false)
			{
				fontFaceFileNameS.append(".ttf");
			}
			if (FileUtil::exists(fontFaceFileNameS.c_str()) == false)
			{
				StringUtil::excludeExtension(fontFaceFileNameS);
				fontFaceFileNameS.append(".otf");
			}

			_fontData._fontSize = fontSize;

			if (initializeFreeType(fontFaceFileNameS.c_str()) == false)
			{
				MINT_LOG_ERROR("FreeType - 초기화에 실패했습니다.");
				return false;
			}

			static constexpr int16 kInitialHeight = 64;
			Vector<uint8> pixelArray(static_cast<int64>(textureWidth) * kInitialHeight);

			_fontData._glyphInfoArray.clear();
			_fontData._charCodeToGlyphIndexMap.clear();

			int16 pixelX{ 0 };
			int16 pixelY{ 0 };
			wchar_t maxCharCode = 0;
			const uint32 glyphRangeCount = _glyphRanges.size();
			if (glyphRangeCount == 0)
			{
				MINT_LOG_ERROR("glyphRangeCount 가 0 입니다!! pushGlyphRange() 함수를 먼저 호출해주세요!");
				return false;
			}

			for (uint32 glyphRangeIndex = 0; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				const GlyphRange& glyphRange = _glyphRanges[glyphRangeIndex];
				maxCharCode = Max(maxCharCode, glyphRange._endWchar);
			}

			_fontData._charCodeToGlyphIndexMap.resize(maxCharCode + 1);

			for (uint32 glyphRangeIndex = 0; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				const GlyphRange& glyphRange = _glyphRanges[glyphRangeIndex];
				for (wchar_t wch = glyphRange._startWchar; wch <= glyphRange._endWchar; ++wch)
				{
					bakeGlyph(wch, textureWidth, spaceLeft, spaceTop, pixelArray, pixelX, pixelY);
				}
			}

			const int32 textureHeight = static_cast<int32>(pixelArray.size() / textureWidth);
			completeGlyphInfoArray(textureWidth, textureHeight);

#if defined MINT_FONT_RENDERER_SAVE_PNG_FOR_TEST
			std::string pngFileName = outputFileName;
			StringUtil::excludeExtension(pngFileName);
			pngFileName.append(".png");
			stbi_write_png(pngFileName.c_str(), textureWidth, textureHeight, 1, &pixelArray[0], textureWidth * 1);
#endif

			BinaryFileWriter binaryFileWriter;
			writeMetaData(textureWidth, textureHeight, binaryFileWriter);

#if defined MINT_FONT_RENDERER_COMPRESS_AS_PNG
			ImageLoader imageLoader;
			imageLoader.saveImagePNG(pixelArray.data(), textureWidth, textureHeight, 1, binaryFileWriter, true);
#else
			const uint32 pixelCount = static_cast<uint32>(pixelArray.size());
			binaryFileWriter.write(pixelArray.size());
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				binaryFileWriter.write(pixelArray[pixelIndex]);
			}
#endif

			std::string outputFileNameS = outputFileName;
			StringUtil::excludeExtension(outputFileNameS);
			outputFileNameS.append(kFontFileExtension);

			binaryFileWriter.save(outputFileNameS.c_str());

			return true;
		}

		bool FontLoader::initializeFreeType(const char* const fontFaceFileName)
		{
			if (FT_Init_FreeType(&_ftLibrary))
			{
				MINT_LOG_ERROR("FreeType - 라이브러리 초기화에 실패했습니다.");
				return false;
			}

			if (FT_New_Face(_ftLibrary, fontFaceFileName, 0, &_ftFace))
			{
				MINT_LOG_ERROR("FreeType - 폰트를 읽어오는 데 실패했습니다.");
				return false;
			}

			if (FT_Set_Pixel_Sizes(_ftFace, 0, _fontData._fontSize))
			{
				MINT_LOG_ERROR("FreeType - 폰트 크기를 지정하는 데 실패했습니다.");
				return false;
			}

			return true;
		}

		void FontLoader::deinitializeFreeType()
		{
			if (_ftFace != nullptr)
			{
				FT_Done_Face(_ftFace);
			}

			if (_ftLibrary != nullptr)
			{
				FT_Done_FreeType(_ftLibrary);
			}
		}

		bool FontLoader::bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY)
		{
			if (FT_Load_Glyph(_ftFace, FT_Get_Char_Index(_ftFace, wch), FT_LOAD_PEDANTIC | FT_FACE_FLAG_HINTER | FT_LOAD_TARGET_NORMAL))
			{
				MINT_LOG_ERROR("FreeType - Glyph 를 불러오는 데 실패했습니다.");
				return false;
			}

			if (FT_Render_Glyph(_ftFace->glyph, FT_RENDER_MODE_NORMAL))
			{
				MINT_LOG_ERROR("FreeType - Glyph 를 렌더하는 데 실패했습니다.");
				return false;
			}

			const int16 rows = static_cast<int16>(_ftFace->glyph->bitmap.rows);
			const int16 cols = static_cast<int16>(_ftFace->glyph->bitmap.width);

			const int16 spacedWidth = spaceLeft + cols;
			const int16 spacedHeight = spaceTop + _fontData._fontSize + kSpaceBottom;
			if (width <= pixelPositionX + spacedWidth)
			{
				pixelPositionX = 0;
				pixelPositionY += spacedHeight;

				const int16 height = static_cast<int16>(pixelArray.size() / width);
				if (height <= pixelPositionY + spacedHeight)
				{
					pixelArray.resize(static_cast<int32>(static_cast<int64>(width) * height * 2));
				}
			}

			for (int16 y = 0; y < rows; ++y)
			{
				for (int16 x = 0; x < cols; ++x)
				{
					pixelArray[(spaceTop + pixelPositionY + y) * width + (spaceLeft + pixelPositionX + x)] = _ftFace->glyph->bitmap.buffer[y * cols + x];
				}
			}

			GlyphInfo glyphInfo{ wch, &_ftFace->glyph->metrics };
			glyphInfo._uv0._x = static_cast<float>(spaceLeft + pixelPositionX);
			glyphInfo._uv0._y = static_cast<float>(spaceTop + pixelPositionY);
			//if (wch == L' ' && glyphInfo._width == 0) // 띄어쓰기 렌더링이 안 되는 경우 예외 처리...
			//{
			//	glyphInfo._width = glyphInfo._horiAdvance;
			//	glyphInfo._horiAdvance = _fontData._glyphInfoArray.front()._horiAdvance;
			//}
			_fontData._glyphInfoArray.push_back(glyphInfo);
			_fontData._charCodeToGlyphIndexMap[wch] = _fontData._glyphInfoArray.size() - 1;

			pixelPositionX += spacedWidth;
			return true;
		}

		void FontLoader::completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight)
		{
			const double textureWidthF = static_cast<double>(textureWidth);
			const double textureHeightF = static_cast<double>(textureHeight);

			const uint32 glyphInfoCount = _fontData._glyphInfoArray.size();
			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._uv0._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) / textureWidthF);
				glyphInfo._uv0._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) / textureHeightF);
				glyphInfo._uv1._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) + (static_cast<double>(glyphInfo._width) / textureWidthF));
				glyphInfo._uv1._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) + (static_cast<double>(glyphInfo._height) + kSpaceBottomForVisibility) / textureHeightF);
			}
		}

		void FontLoader::writeMetaData(const int16 textureWidth, const int16 textureHeight, BinaryFileWriter& binaryFileWriter) const noexcept
		{
			binaryFileWriter.write(kFontFileMagicNumber);

			binaryFileWriter.write(_fontData._fontSize);

			binaryFileWriter.write(textureWidth);
			binaryFileWriter.write(textureHeight);

			const uint32 glyphInfoCount = _fontData._glyphInfoArray.size();
			binaryFileWriter.write(glyphInfoCount);
			const uint32 charCodeToGlyphIndexMapSize = _fontData._charCodeToGlyphIndexMap.size();
			binaryFileWriter.write(charCodeToGlyphIndexMapSize);
			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				binaryFileWriter.write(glyphInfo._charCode);
				binaryFileWriter.write(glyphInfo._width);
				binaryFileWriter.write(glyphInfo._height);
				binaryFileWriter.write(glyphInfo._horiBearingX);
				binaryFileWriter.write(glyphInfo._horiBearingY);
				binaryFileWriter.write(glyphInfo._horiAdvance);
				binaryFileWriter.write(glyphInfo._uv0);
				binaryFileWriter.write(glyphInfo._uv1);
			}
		}
	}
}
