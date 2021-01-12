#include <stdafx.h>
#include <FsLibrary/SimpleRendering/FontRenderer.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <algorithm>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>
#include <FsLibrary/SimpleRendering/TriangleBuffer.hpp>

#include <FsLibrary/Container/StringUtil.hpp>

#include <FsLibrary/File/BinaryFile.hpp>
#include <FsLibrary/File/FileUtil.hpp>


//#define FS_FONT_RENDERER_SAVE_PNG_FOR_TEST
#define FS_FONT_RENDERER_COMPRESS_AS_PNG


namespace fs
{
	namespace SimpleRendering
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


		FontRenderer::FontRenderer(GraphicDevice* const graphicDevice)
			: IRenderer(graphicDevice)
			, _ftLibrary{ nullptr }
			, _ftFace{ nullptr }
			, _fontSize{ 16 }
			, _rendererBuffer{ graphicDevice }
		{
			__noop;
		}

		FontRenderer::~FontRenderer()
		{
			deinitializeFreeType();
		}

		void FontRenderer::pushGlyphRange(const GlyphRange& glyphRange)
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

		const bool FontRenderer::loadFont(const char* const fontFileName)
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

			uint64 glyphInfoCount = 0;
			glyphInfoCount = *binaryFileReader.read<uint64>();

			_glyphInfoArray.resize(glyphInfoCount);
			_glyphMap.clear();
			_glyphMap.reserve(glyphInfoCount);

			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
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

				_glyphMap[glyphInfo._charCode] = glyphIndex;
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
			
			D3D11_TEXTURE2D_DESC texture2DDescriptor{};
			texture2DDescriptor.Width = textureWidth;
			texture2DDescriptor.Height = textureHeight;
			texture2DDescriptor.MipLevels = 1;
			texture2DDescriptor.ArraySize = 1;
			texture2DDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_R8_UNORM; //DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			texture2DDescriptor.SampleDesc.Count = 1;
			texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
			texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
			texture2DDescriptor.CPUAccessFlags = 0;

			D3D11_SUBRESOURCE_DATA subResource{};
			subResource.pSysMem = &rawData[0];
			subResource.SysMemPitch = texture2DDescriptor.Width; // *4;
			subResource.SysMemSlicePitch = 0;

			ComPtr<ID3D11Texture2D> fontTexture;
			_graphicDevice->getDxDevice()->CreateTexture2D(&texture2DDescriptor, &subResource, fontTexture.ReleaseAndGetAddressOf());

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
			srvDesc.Format = texture2DDescriptor.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			_graphicDevice->getDxDevice()->CreateShaderResourceView(fontTexture.Get(), &srvDesc, _fontTextureSrv.ReleaseAndGetAddressOf());

			return true;
		}

		const bool FontRenderer::bakeFont(const char* const fontFaceFileName, const int16 fontSize, const char* const outputFileName, const int16 textureWidth, const int16 spaceLeft, const int16 spaceTop)
		{
			if (initializeFreeType(fontFaceFileName, fontSize) == false)
			{
				FS_LOG_ERROR("김장원", "FreeType - 초기화에 실패했습니다.");
				return false;
			}

			static constexpr int16 kInitialHeight = 64;
			std::vector<uint8> pixelArray(static_cast<int64>(textureWidth) * kInitialHeight);

			_glyphInfoArray.clear();
			_glyphMap.clear();

			int16 pixelX{ 0 };
			int16 pixelY{ 0 };
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

		const bool FontRenderer::initializeFreeType(const char* const fontFaceFileName, const int16 fontSize)
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

		const bool FontRenderer::deinitializeFreeType()
		{
			FT_Done_Face(_ftFace);

			FT_Done_FreeType(_ftLibrary);

			return true;
		}

		const bool FontRenderer::bakeGlyph(const wchar_t wch, const int16 width, const int16 spaceLeft, const int16 spaceTop, std::vector<uint8>& pixelArray, int16& pixelPositionX, int16& pixelPositionY)
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
			_glyphInfoArray.emplace_back(glyphInfo);
			_glyphMap[wch] = _glyphInfoArray.size() - 1;
			
			pixelPositionX += spacedWidth;
			return true;
		}

		void FontRenderer::completeGlyphInfoArray(const int16 textureWidth, const int16 textureHeight)
		{
			const double textureWidthF = static_cast<double>(textureWidth);
			const double textureHeightF = static_cast<double>(textureHeight);

			const uint64 glyphInfoCount = _glyphInfoArray.size();
			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
				glyphInfo._uv0._x = static_cast<float>(static_cast<double>(glyphInfo._uv0._x) / textureWidthF);
				glyphInfo._uv0._y = static_cast<float>(static_cast<double>(glyphInfo._uv0._y) / textureHeightF);
				glyphInfo._uv1._x = glyphInfo._uv0._x + static_cast<float>(static_cast<double>(glyphInfo._width) / textureWidthF);
				glyphInfo._uv1._y = glyphInfo._uv0._y + static_cast<float>(static_cast<double>(glyphInfo._height + kSpaceBottomForVisibility) / textureHeightF);
			}
		}

		void FontRenderer::writeMetaData(const int16 textureWidth, const int16 textureHeight, fs::BinaryFileWriter& binaryFileWriter) const noexcept
		{
			binaryFileWriter.write("FNT");
			binaryFileWriter.write(textureWidth);
			binaryFileWriter.write(textureHeight);

			const uint64 glyphInfoCount = _glyphInfoArray.size();
			binaryFileWriter.write(glyphInfoCount);
			for (uint64 glyphIndex = 0; glyphIndex < glyphInfoCount; ++glyphIndex)
			{
				const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
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

		void FontRenderer::initializeShaders() noexcept
		{
			fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

			// Compile vertex shader and create input layer
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					VS_OUTPUT main(VS_INPUT input)
					{
						VS_OUTPUT result;
						result._position	= mul(float4(input._position.xyz, 1.0), _cbProjectionMatrix);
						result._color		= input._color;
						result._texCoord	= input._texCoord;
						result._flag		= input._flag;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT));
				_vertexShader = shaderPool.pushVertexShader("FontRendererVS", kShaderString, "main", DxShaderVersion::v_4_0, &typeInfo);
			}

			// Compile pixel shader
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>

					sampler					sampler0;
					Texture2D<float>		texture0;
				
					float4 main(VS_OUTPUT input) : SV_Target
					{
						const float sampled = texture0.Sample(sampler0, input._texCoord);
						float4 sampledColor = float4(input._color.xyz * ((0.0 < sampled) ? 1.0 : 0.0), sampled);
						
						if (input._flag == 1)
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
				_pixelShader = shaderPool.pushNonVertexShader("FontRendererPS", kShaderString, "main", DxShaderVersion::v_4_0, DxShaderType::PixelShader);
			}
		}

		void FontRenderer::flushData() noexcept
		{
			_rendererBuffer.flush();
		}

		void FontRenderer::render() noexcept
		{
			if (_rendererBuffer.isReady() == true)
			{
				_graphicDevice->getDxDeviceContext()->PSSetShaderResources(0, 1, _fontTextureSrv.GetAddressOf());

				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShader);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShader);
				_rendererBuffer.render();
			}
		}

		void FontRenderer::drawDynamicText(const wchar_t* const wideText, const fs::Int2& position, const TextHorzAlignment textHorzAlignment, const bool drawShade)
		{
			auto& vertexArray = _rendererBuffer.vertexArray();

			const uint32 textLength = fs::StringUtil::wcslen(wideText);
			const float textWidth = calculateTextWidth(wideText, textLength);
			
			fs::Int2 currentPosition = position;
			if (textHorzAlignment != TextHorzAlignment::Left)
			{
				currentPosition._x -= static_cast<int32>((textHorzAlignment == TextHorzAlignment::Center) ? textWidth * 0.5f : textWidth);
			}
			for (uint32 at = 0; at < textLength; ++at)
			{
				drawGlyph(wideText[at], currentPosition, drawShade);
			}
		}

		const float FontRenderer::calculateTextWidth(const wchar_t* const wideText, const uint32 textLength) const noexcept
		{
			int32 totalWidth = 0;
			for (uint32 textAt = 0; textAt < textLength; ++textAt)
			{
				const wchar_t& wideChar = wideText[textAt];
				
				uint64 glyphIndex = 0;
				auto found = _glyphMap.find(wideChar);
				if (found != _glyphMap.end())
				{
					glyphIndex = _glyphMap.at(wideChar);
				}

				const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
				totalWidth += glyphInfo._horiAdvance;
			}
			return static_cast<float>(totalWidth);
		}

		void FontRenderer::drawGlyph(const wchar_t wideChar, fs::Int2& position, const bool drawShade)
		{
			uint64 glyphIndex = 0;
			auto found = _glyphMap.find(wideChar);
			if (found != _glyphMap.end())
			{
				glyphIndex = _glyphMap.at(wideChar);
			}

			const float fontHeight = static_cast<float>(_fontSize);
			const fs::Float2 positionF = fs::Float2(position);
			const GlyphInfo& glyphInfo = _glyphInfoArray[glyphIndex];
			auto& vertexArray = _rendererBuffer.vertexArray();
			
			fs::CppHlsl::VS_INPUT v;
			v._position._x = positionF._x + static_cast<float>(glyphInfo._horiBearingX);
			v._position._y = positionF._y + fontHeight - static_cast<float>(glyphInfo._horiBearingY);
			v._color = _defaultColor;
			v._texCoord = glyphInfo._uv0;
			v._flag = (drawShade == true) ? 1 : 0;
			vertexArray.emplace_back(v);
			
			v._position._x += static_cast<float>(glyphInfo._width);
			v._texCoord._x = glyphInfo._uv1._x;
			v._texCoord._y = glyphInfo._uv0._y;
			vertexArray.emplace_back(v);
			
			v._position._x = positionF._x + static_cast<float>(glyphInfo._horiBearingX);
			v._position._y += static_cast<float>(glyphInfo._height);
			v._texCoord._x = glyphInfo._uv0._x;
			v._texCoord._y = glyphInfo._uv1._y;
			vertexArray.emplace_back(v);

			v._position._x += static_cast<float>(glyphInfo._width);
			v._texCoord = glyphInfo._uv1;
			vertexArray.emplace_back(v);

			prepareIndexArray();

			position._x += static_cast<int32>(glyphInfo._horiAdvance);
		}

		void FontRenderer::prepareIndexArray()
		{
			const auto& vertexArray = _rendererBuffer.vertexArray();
			const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

			auto& indexArray = _rendererBuffer.indexArray();
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		}
	}
}
