#include <MintRendering/Include/SpriteRenderer.h>
#include <MintRendering/Include/GraphicsDevice.h>
#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
	namespace Rendering
	{
		SpriteRenderer::SpriteRenderer(GraphicsDevice& graphicsDevice, LowLevelRenderer<VS_INPUT_SHAPE>& lowLevelRenderer, Vector<SB_Transform>& sbTransformData, const uint32 psTextureSlot, const ByteColor& transparentColor)
			: ShapeRenderer(graphicsDevice, lowLevelRenderer, sbTransformData)
			, _psTextureSlot{ psTextureSlot }
			, _transparentColor{ transparentColor }
		{
			__noop;
		}

		void SpriteRenderer::Initialize() noexcept
		{
			InitializeCommon();
		}

		void SpriteRenderer::SetCoordinateSpace(const CoordinateSpace& coordinateSpace) noexcept
		{
			__super::SetCoordinateSpace(coordinateSpace);
		}

		void SpriteRenderer::SetMaterial(const GraphicsObjectID& materialID) noexcept
		{
			__super::SetMaterial(materialID);
		}

		void SpriteRenderer::DrawImage(const Float2& position, const Float2& size, const Float2& uv0, const Float2& uv1)
		{
			_uv0 = uv0;
			_uv1 = uv1;

			DrawRectangle(Float3(position), size);
		}

		const char* SpriteRenderer::GetPixelShaderName() const noexcept
		{
			return "SpriteRendererPS";
		}

		const char* SpriteRenderer::GetPixelShaderString() const noexcept
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
			static StackStringA<1024> shaderString;
			shaderString = kShaderStringInclude;
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
			return shaderString.CString();
		}

		const char* SpriteRenderer::GetPixelShaderEntryPoint() const noexcept
		{
			return "main_image";
		}
	}
}
