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
				if (_vertexShaderID.isValid())
				{
					shaderPool.removeShader(_vertexShaderID);
				}
				_vertexShaderID = shaderPool.addShaderFromMemory("ImageRendererVS", getDefaultVertexShaderString(), "main_shape", GraphicShaderType::VertexShader);

				if (_inputLayoutID.isValid())
				{
					shaderPool.removeInputLayout(_inputLayoutID);
				}
				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.getCppHlslSteamData().getTypeMetaData(typeid(VS_INPUT_SHAPE));
				_inputLayoutID = shaderPool.addInputLayout(_vertexShaderID, typeMetaData);
			}

			{
				if (_geometryShaderID.isValid())
				{
					shaderPool.removeShader(_geometryShaderID);
				}
				_geometryShaderID = shaderPool.addShaderFromMemory("ImageRendererGS", getDefaultGeometryShaderString(), "main_shape", GraphicShaderType::GeometryShader);
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
					float4 main_image(VS_OUTPUT_SHAPE input) : SV_Target
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
				shaderString += ");\n";
				shaderString += kShaderStringContent;
				if (_pixelShaderID.isValid())
				{
					shaderPool.removeShader(_pixelShaderID);
				}
				_pixelShaderID = shaderPool.addShaderFromMemory("ImageRendererPS", shaderString.c_str(), "main_image", GraphicShaderType::PixelShader);
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
