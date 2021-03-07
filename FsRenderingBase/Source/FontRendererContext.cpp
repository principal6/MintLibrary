#include <stdafx.h>
#include <FsRenderingBase/Include/FontRendererContext.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <algorithm>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/TriangleRenderer.hpp>

#include <FsContainer/Include/StringUtil.hpp>

#include <FsPlatform/Include/BinaryFile.hpp>
#include <FsPlatform/Include/FileUtil.hpp>


//#define FS_FONT_RENDERER_SAVE_PNG_FOR_TEST
#define FS_FONT_RENDERER_COMPRESS_AS_PNG


namespace fs
{
	namespace RenderingBase
	{
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

		const bool GlyphRange::operator<(const GlyphRange& rhs) const noexcept
		{
			return _startWchar < rhs._startWchar;
		}


		FontRendererContext::FontRendererContext(GraphicDevice* const graphicDevice)
			: FontRendererContext(graphicDevice, FS_NEW(RenderingBase::TriangleRenderer<RenderingBase::VS_INPUT_SHAPE>, graphicDevice))
		{
			_ownTriangleRenderer = true;
		}

		FontRendererContext::FontRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice, fs::RenderingBase::TriangleRenderer<RenderingBase::VS_INPUT_SHAPE>* const triangleRenderer)
			: IRendererContext(graphicDevice)
			, _ftLibrary{ nullptr }
			, _ftFace{ nullptr }
			, _fontSize{ 16 }
			, _triangleRenderer{ triangleRenderer }
			, _ownTriangleRenderer{ false }
		{
			__noop;
		}

		FontRendererContext::~FontRendererContext()
		{
			if (_ownTriangleRenderer == true)
			{
				FS_DELETE(_triangleRenderer);
			}

			deinitializeFreeType();
		}

		void FontRendererContext::pushGlyphRange(const GlyphRange& glyphRange)
		{
			_glyphRangeArray.emplace_back(glyphRange);

			const uint64 glyphRangeCount = _glyphRangeArray.size();
			if (2 <= glyphRangeCount)
			{
				std::sort(_glyphRangeArray.begin(), _glyphRangeArray.end());

				std::vector<uint64> deletionList;
				for (uint64 glyphRangeIndex = 1; glyphRangeIndex < glyphRangeCount; ++glyphRangeIndex)
				{
					GlyphRange& prev = _glyphRangeArray[glyphRangeIndex - 1];
					GlyphRange& curr = _glyphRangeArray[glyphRangeIndex];

					if (curr._startWchar <= prev._endWchar)
					{
						curr._startWchar = std::min(curr._startWchar, prev._startWchar);
						curr._endWchar = std::max(curr._endWchar, prev._endWchar);

						deletionList.emplace_back(glyphRangeIndex - 1);
					}
				}

				const uint64 deletionCount = deletionList.size();
				for (uint64 deletionIndex = 0; deletionIndex < deletionCount; ++deletionIndex)
				{
					_glyphRangeArray.erase(_glyphRangeArray.begin() + deletionList[deletionCount - deletionIndex - 1]);
				}
			}
		}

		const bool FontRendererContext::loadFontData(const char* const fontFileName)
		{
			std::string fontFileNameS = fontFileName;
			fs::StringUtil::excludeExtension(fontFileNameS);
			fontFileNameS.append(kFontFileExtension);

			if (fs::FileUtil::exists(fontFileNameS.c_str()) == false)
			{
				FS_LOG("김장원", "해당 FontFile 이 존재하지 않습니다: ", fontFileName);
				return false;
			}

			fs::BinaryFileReader binaryFileReader;
			if (binaryFileReader.open(fontFileNameS.c_str()) == false)
			{
				FS_LOG_ERROR("김장원", "해당 FontFile 을 여는 데 실패했습니다.");
				return false;
			}

			const char kMagicNumber[4]{ *binaryFileReader.read<char>(), *binaryFileReader.read<char>(), *binaryFileReader.read<char>(), *binaryFileReader.read<char>() };
			if (fs::StringUtil::strcmp(kMagicNumber, "FNT") == false)
			{
				FS_ASSERT("김장원", false, "FNT 파일이 아닙니다!");
				return false;
			}

			const int16 textureWidth = *binaryFileReader.read<int16>();
			const int16 textureHeight = *binaryFileReader.read<int16>();

			const uint64 glyphInfoCount = *binaryFileReader.read<uint64>();
			const uint64 charCodeToGlyphIndexMapSize = *binaryFileReader.read<uint64>();
			_fontData._glyphInfoArray.resize(glyphInfoCount);
			_fontData._charCodeToGlyphIndexMap.resize(charCodeToGlyphIndexMapSize);

			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._charCode = *binaryFileReader.read<wchar_t>();
				glyphInfo._width = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._height = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiBearingX= *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiBearingY = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._horiAdvance = *binaryFileReader.read<GlyphMetricType>();
				glyphInfo._uv0._x = *binaryFileReader.read<float>();
				glyphInfo._uv0._y = *binaryFileReader.read<float>();
				glyphInfo._uv1._x = *binaryFileReader.read<float>();
				glyphInfo._uv1._y = *binaryFileReader.read<float>();
				
				_fontData._charCodeToGlyphIndexMap[glyphInfo._charCode] = glyphIndex;
			}

			std::vector<byte> rawData;
#if defined FS_FONT_RENDERER_COMPRESS_AS_PNG
			const int32 pngLength = *binaryFileReader.read<int32>();
			std::vector<byte> pngData(pngLength);
			for (int32 pngAt = 0; pngAt < pngLength; ++pngAt)
			{
				pngData[pngAt] = *binaryFileReader.read<byte>();
			}

			int32 width{};
			int32 height{};
			int32 comp{};
			int32 req_comp{ 1 };
			stbi_uc* const tempDataPtr = stbi_load_from_memory(&pngData[0], pngLength, &width, &height, &comp, req_comp);
			const int64 dimension = static_cast<int64>(width) * height;
			rawData.resize(dimension);
			for (int64 at = 0; at < dimension; ++at)
			{
				rawData[at] = tempDataPtr[at];
			}
			stbi_image_free(tempDataPtr);
#else
			const uint32 pixelCount = *binaryFileReader.read<uint32>();
			rawData.resize(pixelCount);
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				rawData[pixelIndex] = *binaryFileReader.read<byte>();
			}
#endif
			
			fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
			_fontData._fontTextureId = resourcePool.pushTexture2D(fs::RenderingBase::DxTextureFormat::R8_UNORM, &rawData[0], textureWidth, textureHeight);
			return true;
		}

		const bool FontRendererContext::loadFontData(const FontData& fontData)
		{
			if (fontData._fontTextureId.isValid() == false)
			{
				FS_ASSERT("김장원", false, "FontData 의 FontTexture 가 Invalid 합니다!");
				return false;
			}

			if (fontData._glyphInfoArray.empty() == true)
			{
				FS_ASSERT("김장원", false, "FontData 의 GlyphInfo 가 비어 있습니다!");
				return false;
			}

			_fontData = fontData;

			return true;
		}

		const bool FontRendererContext::bakeFontData(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop)
		{
			std::string fontFaceFileNameS = fontFaceFileName;
			if (fs::StringUtil::hasExtension(fontFaceFileNameS) == false)
			{
				fontFaceFileNameS.append(".ttf");
			}

			if (fs::FileUtil::exists(fontFaceFileNameS.c_str()) == false)
			{
				fs::StringUtil::excludeExtension(fontFaceFileNameS);
				fontFaceFileNameS.append(".otf");
			}

			if (initializeFreeType(fontFaceFileNameS.c_str(), fontSize) == false)
			{
				FS_LOG_ERROR("김장원", "FreeType - 초기화에 실패했습니다.");
				return false;
			}

			static constexpr int16 kInitialHeight = 64;
			std::vector<uint8> pixelArray(static_cast<int64>(textureWidth) * kInitialHeight);

			_fontData._glyphInfoArray.clear();
			_fontData._charCodeToGlyphIndexMap.clear();

			int16 pixelX{ 0 };
			int16 pixelY{ 0 };
			wchar_t maxCharCode = 0;
			for (const auto& glyphRange : _glyphRangeArray)
			{
				maxCharCode = fs::max(maxCharCode, glyphRange._endWchar);
			}
			_fontData._charCodeToGlyphIndexMap.resize(maxCharCode + 1);
			for (const auto& glyphRange : _glyphRangeArray)
			{
				for (wchar_t wch = glyphRange._startWchar; wch <= glyphRange._endWchar; ++wch)
				{
					bakeGlyph(wch, textureWidth, spaceLeft, spaceTop, pixelArray, pixelX, pixelY);
				}
			}

			const int32 textureHeight = static_cast<int32>(pixelArray.size() / textureWidth);
			completeGlyphInfoArray(textureWidth, textureHeight);

#if defined FS_FONT_RENDERER_SAVE_PNG_FOR_TEST
			std::string pngFileName = outputFileName;
			fs::StringUtil::excludeExtension(pngFileName);
			pngFileName.append(".png");
			stbi_write_png(pngFileName.c_str(), textureWidth, textureHeight, 1, &pixelArray[0], textureWidth * 1);
#endif

			fs::BinaryFileWriter binaryFileWriter;
			writeMetaData(textureWidth, textureHeight, binaryFileWriter);

#if defined FS_FONT_RENDERER_COMPRESS_AS_PNG
			int32 length{ 0 };
			unsigned char* png = stbi_write_png_to_mem(reinterpret_cast<unsigned char*>(&pixelArray[0]), textureWidth * 1, textureWidth, textureHeight, 1, &length);
			if (png == nullptr)
			{
				FS_LOG_ERROR("김장원", "FreeType - 텍스처 정보를 추출하는 데 실패했습니다.");
				return false;
			}

			binaryFileWriter.write(length);
			for (int32 at = 0; at < length; ++at)
			{
				binaryFileWriter.write(png[at]);
			}
			STBIW_FREE(png);
#else
			const uint32 pixelCount = static_cast<uint32>(pixelArray.size());
			binaryFileWriter.write(pixelArray.size());
			for (uint32 pixelIndex = 0; pixelIndex < pixelCount; ++pixelIndex)
			{
				binaryFileWriter.write(pixelArray[pixelIndex]);
			}
#endif

			std::string outputFileNameS = outputFileName;
			fs::StringUtil::excludeExtension(outputFileNameS);
			outputFileNameS.append(kFontFileExtension);

			binaryFileWriter.save(outputFileNameS.c_str());

			return true;
		}

		const FontRendererContext::FontData& FontRendererContext::getFontData() const noexcept
		{
			return _fontData;
		}

		const int16 FontRendererContext::getFontSize() const noexcept
		{
			return _fontSize;
		}

		const bool FontRendererContext::initializeFreeType(const char* const fontFaceFileName, const int16 fontSize)
		{
			if (FT_Init_FreeType(&_ftLibrary))
			{
				FS_LOG_ERROR("김장원", "FreeType - 라이브러리 초기화에 실패했습니다.");
				return false;
			}

			if (FT_New_Face(_ftLibrary, fontFaceFileName, 0, &_ftFace))
			{
				FS_LOG_ERROR("김장원", "FreeType - 폰트를 읽어오는 데 실패했습니다.");
				return false;
			}

			_fontSize = fontSize;
			if (FT_Set_Pixel_Sizes(_ftFace, 0, fontSize))
			{
				FS_LOG_ERROR("김장원", "FreeType - 폰트 크기를 지정하는 데 실패했습니다.");
				return false;
			}

			return true;
		}

		const bool FontRendererContext::deinitializeFreeType()
		{
			FT_Done_Face(_ftFace);

			FT_Done_FreeType(_ftLibrary);

			return true;
		}

		const bool FontRendererContext::bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, std::vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY)
		{
			if (FT_Load_Glyph(_ftFace, FT_Get_Char_Index(_ftFace, wch), FT_LOAD_PEDANTIC | FT_LOAD_FORCE_AUTOHINT))
			{
				FS_LOG_ERROR("김장원", "FreeType - Glyph 를 불러오는 데 실패했습니다.");
				return false;
			}

			if (FT_Render_Glyph(_ftFace->glyph, FT_RENDER_MODE_NORMAL))
			{
				FS_LOG_ERROR("김장원", "FreeType - Glyph 를 렌더하는 데 실패했습니다.");
				return false;
			}

			const int16 rows = static_cast<int16>(_ftFace->glyph->bitmap.rows);
			const int16 cols = static_cast<int16>(_ftFace->glyph->bitmap.width);

			const int16 spacedWidth = spaceLeft + cols;
			const int16 spacedHeight = spaceTop + _fontSize + kSpaceBottom;
			if (width <= pixelPositionX + spacedWidth)
			{
				pixelPositionX = 0;
				pixelPositionY += spacedHeight;

				const int16 height = static_cast<int16>(pixelArray.size() / width);
				if (height <= pixelPositionY + spacedHeight)
				{
					pixelArray.resize(static_cast<int64>(width) * height * 2);
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
			_fontData._glyphInfoArray.emplace_back(glyphInfo);
			_fontData._charCodeToGlyphIndexMap[wch] = _fontData._glyphInfoArray.size() - 1;
			
			pixelPositionX += spacedWidth;
			return true;
		}

		void FontRendererContext::completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight)
		{
			const double textureWidthF = static_cast<double>(textureWidth);
			const double textureHeightF = static_cast<double>(textureHeight);

			const uint64 glyphInfoCount = _fontData._glyphInfoArray.size();
			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				glyphInfo._uv0._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) / textureWidthF);
				glyphInfo._uv0._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) / textureHeightF);
				glyphInfo._uv1._x = glyphInfo._uv0._x + static_cast<float>(static_cast<double>(glyphInfo._width) / textureWidthF);
				glyphInfo._uv1._y = glyphInfo._uv0._y + static_cast<float>(static_cast<double>(glyphInfo._height + kSpaceBottomForVisibility) / textureHeightF);
			}
		}

		void FontRendererContext::writeMetaData(const int16 textureWidth, const int16 textureHeight, fs::BinaryFileWriter& binaryFileWriter) const noexcept
		{
			binaryFileWriter.write("FNT");
			binaryFileWriter.write(textureWidth);
			binaryFileWriter.write(textureHeight);

			const uint64 glyphInfoCount = _fontData._glyphInfoArray.size();
			binaryFileWriter.write(glyphInfoCount);
			const uint64 charCodeToGlyphIndexMapSize = _fontData._charCodeToGlyphIndexMap.size();
			binaryFileWriter.write(charCodeToGlyphIndexMapSize);
			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
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

		void FontRendererContext::initializeShaders() noexcept
		{
			fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

			// Compile vertex shader and create input layer
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					VS_OUTPUT_SHAPE main(VS_INPUT_SHAPE input)
					{
						VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
						result._position		= mul(float4(input._position.xyz, 1.0), _cb2DProjectionMatrix);
						result._color			= input._color;
						result._texCoord		= input._texCoord;
						result._info			= input._info;
						result._viewportIndex	= (uint)input._info.x;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslSteamData().getTypeInfo(typeid(fs::RenderingBase::VS_INPUT_SHAPE));
				_vertexShaderId = shaderPool.pushVertexShaderFromMemory("FontRendererVS", kShaderString, "main", &typeInfo);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					
					[maxvertexcount(3)]
					void main(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
					{
						for (int i = 0; i < 3; ++i)
						{
							OutputStream.Append(input[i]);
						}
						OutputStream.RestartStrip();
					}
					)"
				};
				_geometryShaderId = shaderPool.pushNonVertexShaderFromMemory("FontRendererGS", kShaderString, "main", DxShaderType::GeometryShader);
			}

			// Compile pixel shader
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>

					sampler					sampler0;
					Texture2D<float>		texture0;
				
					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float sampled = texture0.Sample(sampler0, input._texCoord.xy);
						float4 sampledColor = float4(input._color.xyz * ((0.0 < sampled) ? 1.0 : 0.0), sampled * input._color.a);
						
						if (input._info.y == 1.0)
						{
							const float2 rbCoord = input._texCoord - float2(ddx(input._texCoord.x), ddy(input._texCoord.y));
							const float rbSampled = texture0.Sample(sampler0, rbCoord);
							if (0.0 < rbSampled)
							{
								const float3 rbColor = lerp(sampledColor.xyz * 0.25 * max(rbSampled, 0.25), sampledColor.xyz, sampled);
								return float4(rbColor, saturate(sampled + rbSampled));
							}
						}
						return sampledColor;
					}
					)"
				};
				_pixelShaderId = shaderPool.pushNonVertexShaderFromMemory("FontRendererPS", kShaderString, "main", DxShaderType::PixelShader);
			}
		}

		void FontRendererContext::flushData() noexcept
		{
			_triangleRenderer->flush();
		}

		const bool FontRendererContext::hasData() const noexcept
		{
			return _triangleRenderer->isRenderable();
		}

		void FontRendererContext::render() noexcept
		{
			if (_triangleRenderer->isRenderable() == true)
			{
				_graphicDevice->getResourcePool().bindToShader(_fontData._fontTextureId, fs::RenderingBase::DxShaderType::PixelShader, 0);
				
				fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vertexShaderId);

				if (getUseMultipleViewports() == true)
				{
					shaderPool.bindShaderIfNot(DxShaderType::GeometryShader, _geometryShaderId);
				}

				shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _pixelShaderId);

				_triangleRenderer->render();

				if (getUseMultipleViewports() == true)
				{
					shaderPool.unbindShader(DxShaderType::GeometryShader);
				}
			}
		}

		void FontRendererContext::drawDynamicText(const wchar_t* const wideText, const fs::Float4& position, const TextRenderDirectionHorz directionHorz, const TextRenderDirectionVert directionVert, const float scale, const bool drawShade)
		{
			const uint32 textLength = fs::StringUtil::wcslen(wideText);
			drawDynamicText(wideText, textLength, position, directionHorz, directionVert, scale, drawShade);
		}

		void FontRendererContext::drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const fs::Float4& position, const TextRenderDirectionHorz directionHorz, const TextRenderDirectionVert directionVert, const float scale, const bool drawShade)
		{
			const float scaledTextWidth = calculateTextWidth(wideText, textLength) * scale;
			const float scaledFontSize = _fontSize * scale;
			fs::Float4 currentPosition = position + fs::Float4(0.0f, -scaledFontSize * 0.5f - 1.0f, 0.0f, 0.0f);
			if (directionHorz != TextRenderDirectionHorz::Rightward)
			{
				currentPosition._x -= (directionHorz == TextRenderDirectionHorz::Centered) ? scaledTextWidth * 0.5f : scaledTextWidth;
			}
			if (directionVert != TextRenderDirectionVert::Centered)
			{
				currentPosition._y += (directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
			}
			for (uint32 at = 0; at < textLength; ++at)
			{
				drawGlyph(wideText[at], currentPosition, scale, drawShade);
			}
		}

		const float FontRendererContext::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
		{
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];
				
				const uint64 glyphIndex = _fontData._charCodeToGlyphIndexMap[wideChar];
				const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;
			}
			return static_cast<float>(totalWidth);
		}

		const uint32 FontRendererContext::calculateIndexFromPositionInText(const wchar_t* const wideText, const uint32 textLength, const float positionInText) const noexcept
		{
			const int32 positionInTextInt = static_cast<int32>(positionInText);
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];

				const uint64 glyphIndex = _fontData._charCodeToGlyphIndexMap[wideChar];
				const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;

				if (positionInTextInt < totalWidth)
				{
					return textAt;
				}
			}
			return textLength;
		}

		const DxObjectId& FontRendererContext::getFontTextureId() const noexcept
		{
			return _fontData._fontTextureId;
		}

		void FontRendererContext::drawGlyph(const wchar_t wideChar, fs::Float4& position, const float scale, const bool drawShade)
		{
			const uint64 glyphIndex = _fontData._charCodeToGlyphIndexMap[wideChar];
			const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
			const float x0 = position._x + static_cast<float>(glyphInfo._horiBearingX) * scale;
			const float x1 = x0 + static_cast<float>(glyphInfo._width) * scale;
			const float scaledFontHeight = static_cast<float>(_fontSize) * scale;
			const float y0 = position._y + scaledFontHeight - static_cast<float>(glyphInfo._horiBearingY) * scale;
			const float y1 = y0 + static_cast<float>(glyphInfo._height) * scale;
			if (0.0f <= x1 && x0 <= _graphicDevice->getWindowSize()._x && 0.0f <= y1 && y0 <= _graphicDevice->getWindowSize()._y) // 화면을 벗어나면 렌더링 할 필요가 없으므로
			{
				auto& vertexArray = _triangleRenderer->vertexArray();

				fs::RenderingBase::VS_INPUT_SHAPE v;
				v._position._x = x0;
				v._position._y = y0;
				v._position._z = position._z;
				v._color = _defaultColor;
				v._texCoord._x = glyphInfo._uv0._x;
				v._texCoord._y = glyphInfo._uv0._y;
				v._info._x = _viewportIndex;
				v._info._y = (drawShade == true) ? 1.0f : 0.0f;
				v._info._z = 1.0f; // used by ShapeFontRendererContext
				vertexArray.emplace_back(v);

				v._position._x = x1;
				v._texCoord._x = glyphInfo._uv1._x;
				v._texCoord._y = glyphInfo._uv0._y;
				vertexArray.emplace_back(v);

				v._position._x = x0;
				v._position._y = y1;
				v._texCoord._x = glyphInfo._uv0._x;
				v._texCoord._y = glyphInfo._uv1._y;
				vertexArray.emplace_back(v);

				v._position._x = x1;
				v._texCoord._x = glyphInfo._uv1._x;
				v._texCoord._y = glyphInfo._uv1._y;
				vertexArray.emplace_back(v);

				prepareIndexArray();
			}

			position._x += static_cast<float>(glyphInfo._horiAdvance) * scale;
		}

		void FontRendererContext::prepareIndexArray()
		{
			const auto& vertexArray = _triangleRenderer->vertexArray();
			const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

			auto& indexArray = _triangleRenderer->indexArray();
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);

			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		}
	}
}
