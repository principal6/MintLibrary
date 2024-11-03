#include <MintRenderingBase/Include/FontLoader.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Algorithm.hpp>
#include <MintPlatform/Include/FileUtil.hpp>
#include <MintPlatform/Include/BinaryFile.hpp>
#include <MintRenderingBase/Include/GraphicsDevice.h>
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
		uint32 FontData::GetSafeGlyphIndex(const wchar_t wideChar) const noexcept
		{
			return (_charCodeToGlyphIndexMap.Size() <= static_cast<uint32>(wideChar)) ? 0 : _charCodeToGlyphIndexMap[wideChar];
		}

		float FontData::ComputeTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
		{
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];

				const uint32 glyphIndex = GetSafeGlyphIndex(wideChar);
				const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;
			}
			return static_cast<float>(totalWidth);
		}

		uint32 FontData::ComputeIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
		{
			const int32 positionInTextInt = static_cast<int32>(positionInText);
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];

				const uint32 glyphIndex = GetSafeGlyphIndex(wideChar);
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
			DeinitializeFreeType();
		}

		bool FontLoader::ExistsFont(const char* const fontFileNameRaw)
		{
			const StringA fontFileName = GetFontFileNameWithExtension(fontFileNameRaw);
			return FileUtil::Exists(fontFileName.CString());
		}

		StringA FontLoader::GetFontFileNameWithExtension(const char* const fontFileName) noexcept
		{
			StringA fontFileNameWithExtension = fontFileName;
			StringUtil::ExcludeExtension(fontFileNameWithExtension);
			fontFileNameWithExtension.Append(kFontFileExtension);
			return fontFileNameWithExtension;
		}

		void FontLoader::PushGlyphRange(const GlyphRange& glyphRange) noexcept
		{
			_glyphRanges.PushBack(glyphRange);

			const uint32 glyphRangeCount = _glyphRanges.Size();
			if (glyphRangeCount <= 1)
			{
				return;
			}

			QuickSort(_glyphRanges, ComparatorAscending<GlyphRange>());

			Vector<uint32> deletionList;
			for (uint32 glyphRangeIndex = 1; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				GlyphRange& prev = _glyphRanges[glyphRangeIndex - 1];
				GlyphRange& curr = _glyphRanges[glyphRangeIndex];

				if (curr._startWchar <= prev._endWchar)
				{
					curr._startWchar = mint::Min(curr._startWchar, prev._startWchar);
					curr._endWchar = mint::Max(curr._endWchar, prev._endWchar);

					deletionList.PushBack(glyphRangeIndex - 1);
				}
			}

			const uint32 deletionCount = deletionList.Size();
			for (uint32 deletionIndex = 0; deletionIndex < deletionCount; ++deletionIndex)
			{
				_glyphRanges.Erase(deletionList[deletionCount - deletionIndex - 1]);
			}
		}

		bool FontLoader::LoadFont(const char* const fontFileNameRaw, GraphicsDevice& graphicsDevice)
		{
			const StringA fontFileName = GetFontFileNameWithExtension(fontFileNameRaw);
			if (FileUtil::Exists(fontFileName.CString()) == false)
			{
				MINT_LOG_ERROR("해당 FontFile 이 존재하지 않습니다: %s", fontFileName.CString());
				return false;
			}

			BinaryFileReader binaryFileReader;
			if (binaryFileReader.Open(fontFileName.CString()) == false)
			{
				MINT_LOG_ERROR("해당 FontFile 을 여는 데 실패했습니다: %s", fontFileName.CString());
				return false;
			}

			const char kMagicNumber[4]{ *binaryFileReader.Read<char>(), *binaryFileReader.Read<char>(), *binaryFileReader.Read<char>(), *binaryFileReader.Read<char>() };
			if (StringUtil::Equals(kMagicNumber, kFontFileMagicNumber) == false)
			{
				MINT_LOG_ERROR("%s 파일이 아닙니다!", kFontFileMagicNumber);
				return false;
			}

			_fontData._fontSize = *binaryFileReader.Read<int16>();

			_fontImageData._width = *binaryFileReader.Read<int16>();
			_fontImageData._height = *binaryFileReader.Read<int16>();

			const uint32 glyphInfoCount = *binaryFileReader.Read<uint32>();
			const uint32 charCodeToGlyphIndexMapSize = *binaryFileReader.Read<uint32>();
			_fontData._glyphInfoArray.Resize(glyphInfoCount);
			_fontData._charCodeToGlyphIndexMap.Resize(charCodeToGlyphIndexMapSize);

			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._charCode = *binaryFileReader.Read<wchar_t>();
				glyphInfo._width = *binaryFileReader.Read<GlyphMetricType>();
				glyphInfo._height = *binaryFileReader.Read<GlyphMetricType>();
				glyphInfo._horiBearingX = *binaryFileReader.Read<GlyphMetricType>();
				glyphInfo._horiBearingY = *binaryFileReader.Read<GlyphMetricType>();
				glyphInfo._horiAdvance = *binaryFileReader.Read<GlyphMetricType>();
				glyphInfo._uv0._x = *binaryFileReader.Read<float>();
				glyphInfo._uv0._y = *binaryFileReader.Read<float>();
				glyphInfo._uv1._x = *binaryFileReader.Read<float>();
				glyphInfo._uv1._y = *binaryFileReader.Read<float>();

				_fontData._charCodeToGlyphIndexMap[glyphInfo._charCode] = glyphIndex;
			}

#if defined MINT_FONT_RENDERER_COMPRESS_AS_PNG
			const int32 pngLength = *binaryFileReader.Read<int32>();
			Vector<byte> pngData(pngLength);
			for (int32 pngAt = 0; pngAt < pngLength; ++pngAt)
			{
				pngData[pngAt] = *binaryFileReader.Read<byte>();
			}

			ImageLoader imageLoader;
			imageLoader.LoadImage_(1, pngData, _fontImageData._imageData);
#else
			const uint32 pixelCount = *binaryFileReader.read<uint32>();
			_fontImageData._imageData.resize(pixelCount);
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				_fontImageData._imageData[pixelIndex] = *binaryFileReader.read<byte>();
			}
#endif

			GraphicsResourcePool& resourcePool = graphicsDevice.GetResourcePool();
			_fontData._fontTextureID = resourcePool.AddTexture2D(TextureFormat::R8_UNORM, &_fontImageData._imageData[0], _fontImageData._width, _fontImageData._height);
			return true;
		}

		bool FontLoader::BakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop)
		{
			StringA fontFaceFileNameS = fontFaceFileName;
			if (StringUtil::HasExtension(fontFaceFileNameS) == false)
			{
				fontFaceFileNameS.Append(".ttf");
			}
			if (FileUtil::Exists(fontFaceFileNameS.CString()) == false)
			{
				StringUtil::ExcludeExtension(fontFaceFileNameS);
				fontFaceFileNameS.Append(".otf");
			}

			_fontData._fontSize = fontSize;

			if (InitializeFreeType(fontFaceFileNameS.CString()) == false)
			{
				MINT_LOG_ERROR("FreeType - 초기화에 실패했습니다.");
				return false;
			}

			static constexpr int16 kInitialHeight = 64;
			Vector<uint8> pixelArray(static_cast<int64>(textureWidth) * kInitialHeight);

			_fontData._glyphInfoArray.Clear();
			_fontData._charCodeToGlyphIndexMap.Clear();

			int16 pixelX{ 0 };
			int16 pixelY{ 0 };
			wchar_t maxCharCode = 0;
			const uint32 glyphRangeCount = _glyphRanges.Size();
			if (glyphRangeCount == 0)
			{
				MINT_LOG_ERROR("glyphRangeCount 가 0 입니다!! PushGlyphRange() 함수를 먼저 호출해주세요!");
				return false;
			}

			for (uint32 glyphRangeIndex = 0; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				const GlyphRange& glyphRange = _glyphRanges[glyphRangeIndex];
				maxCharCode = Max(maxCharCode, glyphRange._endWchar);
			}

			_fontData._charCodeToGlyphIndexMap.Resize(maxCharCode + 1);

			for (uint32 glyphRangeIndex = 0; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
			{
				const GlyphRange& glyphRange = _glyphRanges[glyphRangeIndex];
				for (wchar_t wch = glyphRange._startWchar; wch <= glyphRange._endWchar; ++wch)
				{
					BakeGlyph(wch, textureWidth, spaceLeft, spaceTop, pixelArray, pixelX, pixelY);
				}
			}

			const int32 textureHeight = static_cast<int32>(pixelArray.Size() / textureWidth);
			CompleteGlyphInfoArray(textureWidth, textureHeight);

#if defined MINT_FONT_RENDERER_SAVE_PNG_FOR_TEST
			StringA pngFileName = outputFileName;
			StringUtil::ExcludeExtension(pngFileName);
			pngFileName.Append(".png");
			stbi_write_png(pngFileName.CString(), textureWidth, textureHeight, 1, &pixelArray[0], textureWidth * 1);
#endif

			BinaryFileWriter binaryFileWriter;
			WriteMetaData(textureWidth, textureHeight, binaryFileWriter);

#if defined MINT_FONT_RENDERER_COMPRESS_AS_PNG
			ImageLoader imageLoader;
			imageLoader.SaveImagePNG(pixelArray.Data(), textureWidth, textureHeight, 1, binaryFileWriter, true);
#else
			const uint32 pixelCount = static_cast<uint32>(pixelArray.size());
			binaryFileWriter.write(pixelArray.size());
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				binaryFileWriter.write(pixelArray[pixelIndex]);
			}
#endif

			StringA outputFileNameS = outputFileName;
			StringUtil::ExcludeExtension(outputFileNameS);
			outputFileNameS.Append(kFontFileExtension);

			binaryFileWriter.Save(outputFileNameS.CString());

			return true;
		}

		bool FontLoader::InitializeFreeType(const char* const fontFaceFileName)
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

		void FontLoader::DeinitializeFreeType()
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

		bool FontLoader::BakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, Vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY)
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

				const int16 height = static_cast<int16>(pixelArray.Size() / width);
				if (height <= pixelPositionY + spacedHeight)
				{
					pixelArray.Resize(static_cast<int32>(static_cast<int64>(width) * height * 2));
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
			_fontData._glyphInfoArray.PushBack(glyphInfo);
			_fontData._charCodeToGlyphIndexMap[wch] = _fontData._glyphInfoArray.Size() - 1;

			pixelPositionX += spacedWidth;
			return true;
		}

		void FontLoader::CompleteGlyphInfoArray(const int16 textureWidth, const int16 textureHeight)
		{
			const double textureWidthF = static_cast<double>(textureWidth);
			const double textureHeightF = static_cast<double>(textureHeight);

			const uint32 glyphInfoCount = _fontData._glyphInfoArray.Size();
			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._uv0._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) / textureWidthF);
				glyphInfo._uv0._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) / textureHeightF);
				glyphInfo._uv1._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) + (static_cast<double>(glyphInfo._width) / textureWidthF));
				glyphInfo._uv1._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) + (static_cast<double>(glyphInfo._height) + kSpaceBottomForVisibility) / textureHeightF);
			}
		}

		void FontLoader::WriteMetaData(const int16 textureWidth, const int16 textureHeight, BinaryFileWriter& binaryFileWriter) const noexcept
		{
			binaryFileWriter.Write(kFontFileMagicNumber);

			binaryFileWriter.Write(_fontData._fontSize);

			binaryFileWriter.Write(textureWidth);
			binaryFileWriter.Write(textureHeight);

			const uint32 glyphInfoCount = _fontData._glyphInfoArray.Size();
			binaryFileWriter.Write(glyphInfoCount);
			const uint32 charCodeToGlyphIndexMapSize = _fontData._charCodeToGlyphIndexMap.Size();
			binaryFileWriter.Write(charCodeToGlyphIndexMapSize);
			for (uint32 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				binaryFileWriter.Write(glyphInfo._charCode);
				binaryFileWriter.Write(glyphInfo._width);
				binaryFileWriter.Write(glyphInfo._height);
				binaryFileWriter.Write(glyphInfo._horiBearingX);
				binaryFileWriter.Write(glyphInfo._horiBearingY);
				binaryFileWriter.Write(glyphInfo._horiAdvance);
				binaryFileWriter.Write(glyphInfo._uv0);
				binaryFileWriter.Write(glyphInfo._uv1);
			}
		}
	}
}
