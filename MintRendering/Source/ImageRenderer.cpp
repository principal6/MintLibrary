#include <MintRendering/Include/ImageRenderer.h>
#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
	namespace Rendering
	{
		ImageRenderer::ImageRenderer(GraphicDevice& graphicDevice, const uint32 psTextureSlot, const ByteColor& transparentColor)
			: ShapeRendererContext(graphicDevice)
			, _psTextureSlot{ psTextureSlot }
			, _transparentColor{ transparentColor }
		{
			initializeShaders();
		}

		void ImageRenderer::initializeShaders() noexcept
		{
			_clipRect = _graphicDevice.getFullScreenClipRect();

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			{
				if (_vertexShaderID.IsValid())
				{
					shaderPool.removeShader(_vertexShaderID);
				}
				_vertexShaderID = shaderPool.addShaderFromMemory("ImageRendererVS", getDefaultVertexShaderString(), "main_shape", GraphicShaderType::VertexShader);

				if (_inputLayoutID.IsValid())
				{
					shaderPool.removeInputLayout(_inputLayoutID);
				}
				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.getCppHlslSteamData().getTypeMetaData(typeid(VS_INPUT_SHAPE));
				_inputLayoutID = shaderPool.addInputLayout(_vertexShaderID, typeMetaData);
			}

			{
				if (_geometryShaderID.IsValid())
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
				StackStringA<256> textBuffer;
				StringUtil::ToString(_psTextureSlot, textBuffer);
				StackStringA<1024> shaderString = kShaderStringInclude;
				shaderString += "sampler g_linearSampler : register(s0);\n";
				shaderString += "sampler g_pointSampler : register(s1);\n";
				shaderString += "Texture2D<float4> g_texture0 : register(t";
				shaderString += textBuffer.CString();
				shaderString += ");\n";

				StackStringA<256> transparentColorString;
				FormatString(transparentColorString, "float4(%.1f, %.1f, %.1f, %.1f);\n", _transparentColor.rAsFloat(), _transparentColor.gAsFloat(), _transparentColor.bAsFloat(), _transparentColor.aAsFloat());

				shaderString += "float4 main_image(VS_OUTPUT_SHAPE input) : SV_Target\n";
				shaderString += "{\n";
				shaderString += "	const float4 kTransparentColor = ";
				shaderString += transparentColorString;
				shaderString += "	const float4 sampledColor = g_texture0.Sample(g_pointSampler, input._texCoord.xy);\n";
				shaderString += "	const float alpha = clamp(distance(sampledColor.rgb, kTransparentColor.rgb), 0.0, 1.0);\n";
				shaderString += "	float4 result = float4(sampledColor.rgb * alpha, alpha);\n";
				shaderString += "	return result;\n";
				shaderString += "}";
				if (_pixelShaderID.IsValid())
				{
					shaderPool.removeShader(_pixelShaderID);
				}
				_pixelShaderID = shaderPool.addShaderFromMemory("ImageRendererPS", shaderString.CString(), "main_image", GraphicShaderType::PixelShader);
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
