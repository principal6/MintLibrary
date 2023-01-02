#include <MintRendering/Include/ImageRenderer.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
	namespace Rendering
	{
		ImageRenderer::ImageRenderer(GraphicDevice& graphicDevice, const uint32 psTextureSlot)
			: ShapeRendererContext(graphicDevice)
			, _psTextureSlot{ psTextureSlot }
		{
			initializeShaders();
		}

		void ImageRenderer::initializeShaders() noexcept
		{
			_clipRect = _graphicDevice.getFullScreenClipRect();

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();

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
						const uint packedInfo = asuint(input._info.x);
						const uint shapeType = (packedInfo >> 28) & 0xF;
						const uint transformIndex = packedInfo & 0x3FFFFFFF;
						
						float4 transformedPosition = float4(input._position.xyz, 1.0);
						transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
						
						VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
						result._position = float4(mul(transformedPosition, _cb2DProjectionMatrix).xyz, 1.0);
						result._color = input._color;
						result._texCoord = input._texCoord;
						result._info.x = (float)shapeType;
						result._info.y = input._info.y;
						result._viewportIndex = 0;
						
						return result;
					}
					)"
				};

				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.getCppHlslSteamData().getTypeMetaData(typeid(VS_INPUT_SHAPE));
				_vertexShaderID = shaderPool.addShaderFromMemory("ShapeRendererVS", kShaderString, "main_shape", GraphicShaderType::VertexShader);
				_inputLayoutID = shaderPool.addInputLayout(_vertexShaderID, typeMetaData);
			}

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
				_geometryShaderID = shaderPool.addShaderFromMemory("ShapeRendererGS", kShaderString, "main_shape", GraphicShaderType::GeometryShader);
			}

			{
				static constexpr const char kShaderStringInclude[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					)"
				};
				static constexpr const char kShaderStringContent[]
				{
					R"(
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
					{
						float4 color = g_texture0.Sample(g_sampler0, input._texCoord.xy);
						if (color.b >= 0.5 && color.r < 0.5 && color.g < 0.5)
						{
							color.a = 1.0f - color.b;
							color.rgb = 0.0f;
						}
						return color;
					}
					)"
				};
				StackStringA<256> textureSlotString;
				StringUtil::toString(_psTextureSlot, textureSlotString);
				StackStringA<1024> shaderString = kShaderStringInclude;
				shaderString += "sampler g_sampler0 : register(s0);\n";
				shaderString += "Texture2D<float4> g_texture0 : register(t";
				shaderString += textureSlotString.c_str();
				shaderString += "0);\n";
				shaderString += kShaderStringContent;
				_pixelShaderID = shaderPool.addShaderFromMemory("ShapeRendererPS", shaderString.c_str(), "main_shape", GraphicShaderType::PixelShader);
			}
		}

		void ImageRenderer::flush() noexcept
		{
			__super::flush();
		}

		void ImageRenderer::render() noexcept
		{
			__super::render();
		}

		void ImageRenderer::drawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1)
		{
			const Float2 halfSize = size * 0.5f;
			setPosition(Float4(position + halfSize));
			_uv0 = uv0;
			_uv1 = uv1;
			drawRectangleInternal(Float2::kZero, halfSize, Color::kWhite, ShapeType::TexturedTriangle);
			pushShapeTransformToBuffer(0.0f);
		}
	}
}
