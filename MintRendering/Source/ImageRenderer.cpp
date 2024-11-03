#include <MintRendering/Include/ImageRenderer.h>
#include <MintRenderingBase/Include/GraphicsDevice.h>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
	namespace Rendering
	{
		ImageRenderer::ImageRenderer(GraphicsDevice& graphicsDevice, const uint32 psTextureSlot)
			: ImageRenderer(graphicsDevice, psTextureSlot, ByteColor(0, 0, 0, 0))
		{
			__noop;
		}

		ImageRenderer::ImageRenderer(GraphicsDevice& graphicsDevice, const uint32 psTextureSlot, const ByteColor& transparentColor)
			: ShapeRendererContext(graphicsDevice)
			, _psTextureSlot{ psTextureSlot }
			, _transparentColor{ transparentColor }
		{
			InitializeShaders();
		}

		void ImageRenderer::InitializeShaders() noexcept
		{
			_clipRect = _graphicsDevice.GetFullScreenClipRect();

			ShaderPool& shaderPool = _graphicsDevice.GetShaderPool();
			{
				if (_vertexShaderID.IsValid())
				{
					shaderPool.RemoveShader(_vertexShaderID);
				}
				_vertexShaderID = shaderPool.AddShaderFromMemory("ImageRendererVS", GetDefaultVertexShaderString(), "main_shape", GraphicsShaderType::VertexShader);

				if (_inputLayoutID.IsValid())
				{
					shaderPool.RemoveInputLayout(_inputLayoutID);
				}
				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicsDevice.GetCppHlslSteamData().GetTypeMetaData(typeid(VS_INPUT_SHAPE));
				_inputLayoutID = shaderPool.AddInputLayout(_vertexShaderID, typeMetaData);
			}

			{
				if (_geometryShaderID.IsValid())
				{
					shaderPool.RemoveShader(_geometryShaderID);
				}
				_geometryShaderID = shaderPool.AddShaderFromMemory("ImageRendererGS", GetDefaultGeometryShaderString(), "main_shape", GraphicsShaderType::GeometryShader);
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

				bool usesTransparentColor = (_transparentColor != ByteColor(0, 0, 0, 0));
				if (usesTransparentColor)
				{
					StackStringA<256> transparentColorString;
					FormatString(transparentColorString, "float4(%.1f, %.1f, %.1f, %.1f);\n", _transparentColor.RAsFloat(), _transparentColor.GAsFloat(), _transparentColor.BAsFloat(), _transparentColor.AAsFloat());

					shaderString += "float4 main_image(VS_OUTPUT_SHAPE input) : SV_Target\n";
					shaderString += "{\n";
					shaderString += "	const float4 kTransparentColor = ";
					shaderString += transparentColorString;
					shaderString += "	const float4 sampledColor = g_texture0.Sample(g_pointSampler, input._texCoord.xy);\n";
					shaderString += "	const float alpha = clamp(distance(sampledColor.rgb, kTransparentColor.rgb), 0.0, 1.0);\n";
					shaderString += "	float4 result = float4(sampledColor.rgb * alpha, alpha);\n";
					shaderString += "	return result;\n";
					shaderString += "}";
				}
				else
				{
					shaderString += "float4 main_image(VS_OUTPUT_SHAPE input) : SV_Target\n";
					shaderString += "{\n";
					shaderString += "	const float4 sampledColor = g_texture0.Sample(g_pointSampler, input._texCoord.xy);\n";
					shaderString += "	return sampledColor;\n";
					shaderString += "}";
				}
				
				if (_pixelShaderID.IsValid())
				{
					shaderPool.RemoveShader(_pixelShaderID);
				}
				_pixelShaderID = shaderPool.AddShaderFromMemory("ImageRendererPS", shaderString.CString(), "main_image", GraphicsShaderType::PixelShader);
			}
		}

		void ImageRenderer::Flush() noexcept
		{
			__super::Flush();
		}

		void ImageRenderer::Render() noexcept
		{
			__super::Render();
		}

		void ImageRenderer::DrawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1)
		{
			SetPosition(Float4(position));
			_uv0 = uv0;
			_uv1 = uv1;
			DrawRectangle(size, 0.0f, 0.0f);
			PushShapeTransformToBuffer(0.0f);
		}

		void ImageRenderer::DrawImageScreenSpace(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1)
		{
			const Float2 halfSize = size * 0.5f;
			SetPosition(Float4(position + halfSize));
			_uv0 = uv0;
			_uv1 = uv1;
			DrawRectangle(Float2(size._x, -size._y), 0.0f, 0.0f);
			PushShapeTransformToBuffer(0.0f);
		}
	}
}
