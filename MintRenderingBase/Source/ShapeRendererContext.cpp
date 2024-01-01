#include <MintRenderingBase/Include/ShapeRendererContext.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StackVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Float3x3.h>


namespace mint
{
	namespace Rendering
	{
		ShapeRendererContext::ShapeRendererContext(GraphicDevice& graphicDevice)
			: IRendererContext(graphicDevice)
		{
			__noop;
		}

		ShapeRendererContext::~ShapeRendererContext()
		{
			__noop;
		}

		const char* ShapeRendererContext::GetDefaultVertexShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
				#include <ShaderConstantBuffers>
				#include <ShaderStructuredBufferDefinitions>
					
				StructuredBuffer<SB_Transform> sbTransform : register(t0);
					
				VS_OUTPUT_SHAPE main_shape(VS_INPUT_SHAPE input)
				{
					uint transformIndex = input._info & 0x3FFFFFFF;
					float4 transformedPosition = float4(input._position.xyz, 1.0);
					transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
						
					VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
					result._position = mul(transformedPosition, _cbViewProjectionMatrix);
					result._color = input._color;
					result._texCoord = input._texCoord;
					result._info = input._info >> 30;
					result._viewportIndex = 0;
					return result;
				}
				)"
			};
			return kShaderString;
		}

		const char* ShapeRendererContext::GetDefaultGeometryShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
				#include <ShaderStructDefinitions>
					
				[maxvertexcount(3)]
				void main_shape(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
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

		const char* ShapeRendererContext::GetDefaultPixelShaderString() const
		{
			static constexpr const char kShaderString[]
			{
				R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					
					sampler g_sampler0;
					Texture2D<float4> g_texture0;
					
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
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

		void ShapeRendererContext::InitializeShaders() noexcept
		{
			_clipRect = _graphicDevice.GetFullScreenClipRect();

			ShaderPool& shaderPool = _graphicDevice.GetShaderPool();

			{
				if (_vertexShaderID.IsValid())
				{
					shaderPool.RemoveShader(_vertexShaderID);
				}
				_vertexShaderID = shaderPool.AddShaderFromMemory("ShapeRendererVS", GetDefaultVertexShaderString(), "main_shape", GraphicShaderType::VertexShader);

				if (_inputLayoutID.IsValid())
				{
					shaderPool.RemoveInputLayout(_inputLayoutID);
				}
				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.GetCppHlslSteamData().GetTypeMetaData(typeid(VS_INPUT_SHAPE));
				_inputLayoutID = shaderPool.AddInputLayout(_vertexShaderID, typeMetaData);
			}

			{
				if (_geometryShaderID.IsValid())
				{
					shaderPool.RemoveShader(_geometryShaderID);
				}
				_geometryShaderID = shaderPool.AddShaderFromMemory("ShapeRendererGS", GetDefaultGeometryShaderString(), "main_shape", GraphicShaderType::GeometryShader);
			}

			{
				if (_pixelShaderID.IsValid())
				{
					shaderPool.RemoveShader(_pixelShaderID);
				}
				_pixelShaderID = shaderPool.AddShaderFromMemory("ShapeRendererPS", GetDefaultPixelShaderString(), "main_shape", GraphicShaderType::PixelShader);
			}
		}

		void ShapeRendererContext::Flush() noexcept
		{
			_lowLevelRenderer->Flush();

			FlushTransformBuffer();
		}

		void ShapeRendererContext::Render() noexcept
		{
			if (_lowLevelRenderer->IsRenderable() == false)
			{
				return;
			}

			PrepareTransformBuffer();

			// TODO : Slot 처리...
			if (_fontData._fontTextureID.IsValid())
			{
				_graphicDevice.GetResourcePool().BindToShader(_fontData._fontTextureID, GraphicShaderType::PixelShader, 0);
			}

			ShaderPool& shaderPool = _graphicDevice.GetShaderPool();
			shaderPool.BindInputLayoutIfNot(_inputLayoutID);
			shaderPool.BindShaderIfNot(GraphicShaderType::VertexShader, _vertexShaderID);

			if (IsUsingMultipleViewports())
			{
				shaderPool.BindShaderIfNot(GraphicShaderType::GeometryShader, _geometryShaderID);
			}

			shaderPool.BindShaderIfNot(GraphicShaderType::PixelShader, _pixelShaderID);

			GraphicResourcePool& resourcePool = _graphicDevice.GetResourcePool();
			GraphicResource& sbTransformBuffer = resourcePool.GetResource(_graphicDevice.GetCommonSBTransformID());
			sbTransformBuffer.BindToShader(GraphicShaderType::VertexShader, sbTransformBuffer.GetRegisterIndex());

			_lowLevelRenderer->ExecuteRenderCommands();

			if (IsUsingMultipleViewports())
			{
				shaderPool.UnbindShader(GraphicShaderType::GeometryShader);
			}
		}

		bool ShapeRendererContext::InitializeFontData(const FontData& fontData)
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

		void ShapeRendererContext::SetTextColor(const Color& textColor) noexcept
		{
			_textColor = textColor;
		}

		void ShapeRendererContext::AddShape(const Shape& shape)
		{
			const uint32 vertexOffset = _lowLevelRenderer->GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->GetIndexCount();
			const uint32 transformIndex = _sbTransformData.Size();

			VS_INPUT_SHAPE v;
			v._info = ComputeVertexInfo(transformIndex, 0);
			auto& vertices = _lowLevelRenderer->Vertices();
			for (const VS_INPUT_SHAPE& vertex : shape._vertices)
			{
				v._color = vertex._color;
				v._position = vertex._position;
				vertices.PushBack(v);
			}

			auto& indices = _lowLevelRenderer->Indices();
			for (const IndexElementType index : shape._indices)
			{
				indices.PushBack(vertexOffset + index);
			}

			_lowLevelRenderer->PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, shape._indices.Size(), _clipRect);

			PushShapeTransformToBuffer(0.0f);
		}

		void ShapeRendererContext::DrawLine(const Float2& p0, const Float2& p1, const float thickness)
		{

		}

		void ShapeRendererContext::DrawLineStrip(const Vector<Float2>& points, const float thickness)
		{

		}

		void ShapeRendererContext::DrawTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC)
		{
		}

		void ShapeRendererContext::DrawRectangle(const Float2& size, const float borderThickness, const float rotationAngle)
		{
		}

		void ShapeRendererContext::DrawCircle(const float radius)
		{

		}

		void ShapeRendererContext::DrawDynamicText(const wchar_t* const wideText, const Float2& position, const FontRenderingOption& fontRenderingOption)
		{
			DrawDynamicText(wideText, Float4(position._x, position._y, 0.0f, 1.0f), fontRenderingOption);
		}

		void ShapeRendererContext::DrawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption)
		{
			const uint32 textLength = StringUtil::Length(wideText);
			DrawDynamicText(wideText, textLength, position, fontRenderingOption);
		}

		void ShapeRendererContext::DrawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption)
		{
			const float scaledTextWidth = _fontData.ComputeTextWidth(wideText, textLength) * fontRenderingOption._scale;
			const float scaledFontSize = _fontData._fontSize * fontRenderingOption._scale;

			Float4 postTranslation;
			if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Rightward)
			{
				postTranslation._x -= (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Centered) ? scaledTextWidth * 0.5f : scaledTextWidth;
			}
			if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
			{
				postTranslation._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
			}
			postTranslation._y += (-scaledFontSize * 0.5f - 1.0f);

			const uint32 vertexOffset = _lowLevelRenderer->GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->GetIndexCount();

			Float2 glyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				DrawGlyph(wideText[at], glyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, false);
			}

			const uint32 indexCount = _lowLevelRenderer->GetIndexCount() - indexOffset;
			_lowLevelRenderer->PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, indexCount, _clipRect);

			const Float4& preTranslation = position;
			PushFontTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		void ShapeRendererContext::DrawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const uint32 textLength = StringUtil::Length(wideText);
			DrawDynamicTextBitFlagged(wideText, textLength, position, fontRenderingOption, bitFlags);
		}

		void ShapeRendererContext::DrawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const float scaledTextWidth = _fontData.ComputeTextWidth(wideText, textLength) * fontRenderingOption._scale;
			const float scaledFontSize = _fontData._fontSize * fontRenderingOption._scale;

			Float4 postTranslation;
			if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Rightward)
			{
				postTranslation._x -= (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Centered) ? scaledTextWidth * 0.5f : scaledTextWidth;
			}
			if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
			{
				postTranslation._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
			}
			postTranslation._y += (-scaledFontSize * 0.5f - 1.0f);

			const uint32 vertexOffset = _lowLevelRenderer->GetVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->GetIndexCount();

			Float2 glyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				DrawGlyph(wideText[at], glyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, !bitFlags.Get(at));
			}

			const uint32 indexCount = _lowLevelRenderer->GetIndexCount() - indexOffset;
			_lowLevelRenderer->PushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffSetZero, indexOffset, indexCount, _clipRect);

			const Float4& preTranslation = position;
			PushFontTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		void ShapeRendererContext::DrawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace)
		{
			const uint32 transformIndex = _sbTransformData.Size();
			const uint32 glyphIndex = _fontData.GetSafeGlyphIndex(wideChar);
			const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
			if (leaveOnlySpace == false)
			{
				const float scaledFontHeight = static_cast<float>(_fontData._fontSize) * scale;

				Rect glyphRect;
				glyphRect.Left(glyphPosition._x + static_cast<float>(glyphInfo._horiBearingX) * scale);
				glyphRect.Right(glyphRect.Left() + static_cast<float>(glyphInfo._width) * scale);
				glyphRect.Top(glyphPosition._y + scaledFontHeight - static_cast<float>(glyphInfo._horiBearingY) * scale);
				glyphRect.Bottom(glyphRect.Top() + static_cast<float>(glyphInfo._height) * scale);
				if (glyphRect.Right() >= 0.0f && glyphRect.Left() <= _graphicDevice.GetWindowSize()._x
					&& glyphRect.Bottom() >= 0.0f && glyphRect.Top() <= _graphicDevice.GetWindowSize()._y) // 화면을 벗어나면 렌더링 할 필요가 없으므로
				{
					Vector<VS_INPUT_SHAPE>& vertices = _lowLevelRenderer->Vertices();

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

						v._position._x = glyphRect.Right();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv0._y;
						vertices.PushBack(v);

						v._position._x = glyphRect.Left();
						v._position._y = glyphRect.Bottom();
						v._texCoord._x = glyphInfo._uv0._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.PushBack(v);

						v._position._x = glyphRect.Right();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.PushBack(v);
					}

					// Indices
					{
						Vector<IndexElementType>& indices = _lowLevelRenderer->Indices();
						const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertices.Size());
						// 오른손 좌표계
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 0);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 3);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 1);

						indices.PushBack((currentTotalTriangleVertexCount - 4) + 0);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 2);
						indices.PushBack((currentTotalTriangleVertexCount - 4) + 3);
					}
				}
			}

			glyphPosition._x += static_cast<float>(glyphInfo._horiAdvance) * scale;
		}

		uint32 ShapeRendererContext::ComputeVertexInfo(uint32 transformIndex, uint8 type) const
		{
			return (type << 30) | (transformIndex & 0x3FFFFFFF);
		}

		void ShapeRendererContext::PushShapeTransformToBuffer(const float rotationAngle, const bool applyInternalPosition)
		{
			SB_Transform transform;
			transform._transformMatrix = Float4x4::RotationMatrixZ(-rotationAngle);
			transform._transformMatrix._m[0][3] = (applyInternalPosition == true) ? _position._x : 0.0f;
			transform._transformMatrix._m[1][3] = (applyInternalPosition == true) ? _position._y : 0.0f;
			transform._transformMatrix._m[2][3] = (applyInternalPosition == true) ? _position._z : 0.0f;
			_sbTransformData.PushBack(transform);
		}

		void ShapeRendererContext::PushFontTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation)
		{
			SB_Transform transform;
			transform._transformMatrix.PreTranslate(preTranslation.GetXYZ());
			transform._transformMatrix.PostTranslate(postTranslation.GetXYZ());
			transform._transformMatrix *= transformMatrix;
			_sbTransformData.PushBack(transform);
		}
	}
}
