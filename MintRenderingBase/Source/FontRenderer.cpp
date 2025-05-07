#include <MintRenderingBase/Include/FontRenderer.h>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
	namespace Rendering
	{
		FontRenderer::FontRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData)
			: IRenderer(graphicsDevice, lowLevelRenderer, sbTransformData)
		{
			__noop;
		}

		FontRenderer::~FontRenderer()
		{
			Terminate();
		}

		void FontRenderer::InitializeShaders() noexcept
		{
			SetClipRect(_graphicsDevice.GetFullScreenClipRect());


			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			GraphicsObjectID vertexShaderID = shaderPool.CreateShaderFromMemory("FontRendererVS", GetDefaultVertexShaderString(), "main_font", GraphicsShaderType::VertexShader);
			using namespace Language;
			const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicsDevice.GetCppHlslSteamData().GetTypeMetaData(typeid(VS_INPUT_SHAPE));
			GraphicsObjectID inputLayoutID = shaderPool.CreateInputLayout(vertexShaderID, typeMetaData);
			GraphicsObjectID geometryShaderID = shaderPool.CreateShaderFromMemory("FontRendererGS", GetDefaultGeometryShaderString(), "main_font", GraphicsShaderType::GeometryShader);
			GraphicsObjectID pixelShaderID = shaderPool.CreateShaderFromMemory("FontRendererPS", GetDefaultPixelShaderString(), "main_font", GraphicsShaderType::PixelShader);

			ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
			{
				ShaderPipelineDesc shaderPipelineDesc;
				shaderPipelineDesc._inputLayoutID = inputLayoutID;
				shaderPipelineDesc._vertexShaderID = vertexShaderID;
				shaderPipelineDesc._geometryShaderID = geometryShaderID;
				shaderPipelineDesc._pixelShaderID = pixelShaderID;
				_shaderPipelineMultipleViewportID = shaderPipelinePool.CreateShaderPipeline(shaderPipelineDesc);
			}

			MaterialPool& materialPool = _graphicsDevice.GetMaterialPool();
			{
				MaterialDesc materialDesc;
				materialDesc._materialName = "DefaultFont";
				materialDesc._shaderPipelineID = _shaderPipelineMultipleViewportID;
				_defaultMaterialID = materialPool.CreateMaterial(materialDesc);
			}
		}

		void FontRenderer::Terminate() noexcept
		{
			ShaderPipelinePool& shaderPipelinePool = _graphicsDevice.GetShaderPipelinePool();
			if (_shaderPipelineMultipleViewportID.IsValid())
			{
				shaderPipelinePool.DestroyShaderPipeline(_shaderPipelineMultipleViewportID);
			}
		}

		bool FontRenderer::InitializeFontData(const FontData& fontData)
		{
			if (fontData._fontTextureID.IsValid() == false)
			{
				MINT_LOG_ERROR("FontData 의 FontTexture 가 Invalid 합니다!");
				return false;
			}

			if (fontData._glyphInfoArray.IsEmpty() == true)
			{
				MINT_LOG_ERROR("FontData 의 GlyphInfo 가 비어 있습니다!");
				return false;
			}

			_fontData = fontData;

			return true;
		}

		void FontRenderer::SetTextColor(const Color& textColor) noexcept
		{
			_textColor = textColor;
		}

		void FontRenderer::Render() noexcept
		{
			if (_lowLevelRenderer.IsRenderable() == false)
			{
				return;
			}

			// TODO : Slot 처리...
			if (_fontData._fontTextureID.IsValid())
			{
				_graphicsDevice.GetResourcePool().BindToShader(_fontData._fontTextureID, GraphicsShaderType::PixelShader, 0);
			}

			_lowLevelRenderer.ExecuteRenderCommands(_graphicsDevice);

			//ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			//shaderPool.UnbindShader(GraphicsShaderType::GeometryShader);
		}

		void FontRenderer::Flush() noexcept
		{
			__noop;
		}

		void FontRenderer::DrawDynamicText(const wchar_t* const wideText, const Float2& position, const FontRenderingOption& fontRenderingOption)
		{
			DrawDynamicText(wideText, Float3(position._x, position._y, 0.0f), fontRenderingOption);
		}

		void FontRenderer::DrawDynamicText(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption)
		{
			const uint32 textLength = StringUtil::Length(wideText);
			DrawDynamicText(wideText, textLength, position, fontRenderingOption);
		}

		void FontRenderer::DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption)
		{
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();

			Float2 currentGlyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				DrawGlyph(wideText[at], currentGlyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, false);
			}

			const uint32 indexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, indexCount, GetClipRect(), _defaultMaterialID);

			const Float3& preTranslation = ApplyCoordinateSpace(position);
			const Float3& postTranslation = ComputePostTranslation(wideText, textLength, fontRenderingOption);
			PushManualTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		void FontRenderer::DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const uint32 textLength = StringUtil::Length(wideText);
			DrawDynamicTextBitFlagged(wideText, textLength, position, fontRenderingOption, bitFlags);
		}

		void FontRenderer::DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float3& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const uint32 indexOffset = _lowLevelRenderer.GetIndexCount();

			Float2 currentGlyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				DrawGlyph(wideText[at], currentGlyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, !bitFlags.Get(at));
			}

			const uint32 indexCount = _lowLevelRenderer.GetIndexCount() - indexOffset;
			_lowLevelRenderer.PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, indexCount, GetClipRect(), _defaultMaterialID);

			const Float3& preTranslation = ApplyCoordinateSpace(position);
			const Float3& postTranslation = ComputePostTranslation(wideText, textLength, fontRenderingOption);
			PushManualTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		bool FontRenderer::IsEmpty() const noexcept
		{
			return _lowLevelRenderer.IsRenderable() == false;
		}

		Float3 FontRenderer::ComputePostTranslation(const wchar_t* const wideText, const uint32 textLength, const FontRenderingOption& fontRenderingOption) const
		{
			const float scaledTextWidth = _fontData.ComputeTextWidth(wideText, textLength) * fontRenderingOption._scale;
			const float scaledFontSize = _fontData._fontSize * fontRenderingOption._scale;

			Float3 postTranslation;
			postTranslation._y = (-scaledFontSize * 0.5f - 1.0f);
			if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Rightward)
			{
				float xOffset = (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Centered) ? -scaledTextWidth * 0.5f : -scaledTextWidth;
				postTranslation._x += xOffset;
			}
			if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
			{
				float yOffset = (fontRenderingOption._directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
				if (_coordinateSpace == CoordinateSpace::Screen)
					yOffset = -yOffset;

				postTranslation._y += yOffset;
			}
			return postTranslation;
		}

		void FontRenderer::DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace)
		{
			const uint32 transformIndex = _sbTransformData.Size();
			const uint32 glyphIndex = _fontData.GetSafeGlyphIndex(wideChar);
			const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
			if (leaveOnlySpace == false)
			{
				Rect glyphRect;
				glyphRect.Left(glyphPosition._x + static_cast<float>(glyphInfo._horiBearingX) * scale);
				glyphRect.Right(glyphRect.Left() + static_cast<float>(glyphInfo._width) * scale);

				glyphRect.Top(glyphPosition._y + (static_cast<float>(glyphInfo._horiBearingY) * scale));
				glyphRect.Bottom(glyphRect.Top() - static_cast<float>(glyphInfo._height) * scale);

				//const bool shouldFlipY = _graphicsDevice.GetProjectionMatrix()._22 < 0.0f;
				//if (shouldFlipY)
				//{
				//	const float scaledFontHeight = static_cast<float>(_fontData._fontSize) * scale;
				//	glyphRect.Top(glyphPosition._y + scaledFontHeight - static_cast<float>(glyphInfo._horiBearingY) * scale);
				//	glyphRect.Bottom(glyphRect.Top() + static_cast<float>(glyphInfo._height) * scale);
				//}
				//else
				//{
				//	glyphRect.Top(glyphPosition._y + (static_cast<float>(glyphInfo._horiBearingY) * scale));
				//	glyphRect.Bottom(glyphRect.Top() - static_cast<float>(glyphInfo._height) * scale);
				//}

				{
					Vector<VS_INPUT_SHAPE>& vertices = _lowLevelRenderer.Vertices();

					// Vertices
					{
						VS_INPUT_SHAPE v;
						v._position._x = glyphRect.Left();
						v._position._y = glyphRect.Top();
						v._position._z = 0.0f;
						v._color = _textColor;
						v._texCoord._x = glyphInfo._uv0._x;
						v._texCoord._y = glyphInfo._uv0._y;
						v._info = ComputeVertexInfo(transformIndex, 2);
						vertices.PushBack(v);

						v._position._y = glyphRect.Bottom();
						v._texCoord._x = glyphInfo._uv0._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.PushBack(v);

						v._position._x = glyphRect.Right();
						v._position._y = glyphRect.Bottom();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.PushBack(v);

						v._position._y = glyphRect.Top();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv0._y;
						vertices.PushBack(v);
					}

					// Indices
					{
						Vector<IndexElementType>& indices = _lowLevelRenderer.Indices();
						const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertices.Size());
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 0);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 1);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 2);

						indices.PushBack((currentTotalTriangleVertexCount - 4) + 0);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 2);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 3);
					}
				}
			}

			glyphPosition._x += static_cast<float>(glyphInfo._horiAdvance) * scale;
		}

		uint32 FontRenderer::ComputeVertexInfo(uint32 transformIndex, uint8 type) const
		{
			return (type << 30) | (transformIndex & 0x3FFFFFFF);
		}

		void FontRenderer::PushManualTransformToBuffer(const Float3& preTranslation, const Float4x4& transformMatrix, const Float3& postTranslation)
		{
			SB_Transform sbTransform;
			sbTransform._transformMatrix.PreTranslate(preTranslation);
			sbTransform._transformMatrix *= transformMatrix;
			sbTransform._transformMatrix.PostTranslate(postTranslation);
			_sbTransformData.PushBack(sbTransform);
		}

		const char* FontRenderer::GetDefaultVertexShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
				#include <ShaderConstantBuffers>
				#include <ShaderStructuredBufferDefinitions>
					
				StructuredBuffer<SB_Transform> sbTransform : register(t0);
					
				VS_OUTPUT_SHAPE main_font(VS_INPUT_SHAPE input)
				{
					uint transformIndex = input._info & 0x3FFFFFFF;
					uint info = input._info >> 30;
					
					float4 transformedPosition = float4(input._position.xyz, 1.0);
					transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
						
					VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
					result._position = mul(transformedPosition, _cbViewProjectionMatrix);
					result._color = input._color;
					result._texCoord = input._texCoord;
					result._info = info;
					result._viewportIndex = 0;
					return result;
				}
				)"
			};
			return kShaderString;
		}

		const char* FontRenderer::GetDefaultGeometryShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
					
				[maxvertexcount(3)]
				void main_font(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
				{
					for (int i = 0; i < 3; ++i)
					{
						OutputStream.Append(input[i]);
					}
					OutputStream.RestartStrip();
				}
				)"
			};
			return kShaderString;
		}

		const char* FontRenderer::GetDefaultPixelShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					
					sampler g_sampler0;
					Texture2D<float4> g_texture0;
					
					float4 main_font(VS_OUTPUT_SHAPE input) : SV_Target
					{
						if (input._info == 1)
						{
							// Textured triangle
							return g_texture0.Sample(g_sampler0, input._texCoord.xy);
						}
						else if (input._info == 2)
						{
							// Font triangle
							const float sampled = g_texture0.Sample(g_sampler0, input._texCoord.xy);
							const float4 sampled4 = float4(input._color.xyz * ((sampled > 0.0) ? 1.0 : 0.0), sampled * input._color.a);
							const bool drawShade = false;
							if (drawShade)
							{
								const float2 rbCoord = input._texCoord - float2(ddx(input._texCoord.x), ddy(input._texCoord.y));
								const float rbSampled = g_texture0.Sample(g_sampler0, rbCoord);
								if (rbSampled > 0.0)
								{
									const float3 rbColor = lerp(sampled4.xyz * 0.25 * max(rbSampled, 0.25), sampled4.xyz, sampled);
									return float4(rbColor, saturate(sampled + rbSampled));
								}
							}
							return sampled4;
						}
						return input._color;
					}
					)"
			};
			return kShaderString;
		}
	}
}
