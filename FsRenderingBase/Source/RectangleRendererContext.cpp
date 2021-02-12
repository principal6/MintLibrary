#include <stdafx.h>
#include <FsRenderingBase/Include/RectangleRendererContext.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/TriangleRenderer.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE fs::Float4 getVertexPosition(const uint32 vertexIndex, const fs::Float4& position, const fs::Float2& size)
		{
			return fs::Float4(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y, position._z, position._w);
		}

		FS_INLINE fs::Float2 getVertexTexturePosition(const uint32 vertexIndex, const fs::Float2& position, const fs::Float2& size)
		{
			return fs::Float2(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y);
		}

		RectangleRendererContext::RectangleRendererContext(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: IRendererContext(graphicDevice)
			, _triangleRenderer{ graphicDevice }
		{
			__noop;
		}

		void RectangleRendererContext::initializeShaders() noexcept
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
						VS_OUTPUT result = (VS_OUTPUT)0;
						result._position	= mul(float4(input._position.xyz, 1.0), _cbProjectionMatrix);
						result._color		= input._color;
						result._texCoord	= input._texCoord;
						result._flag		= input._flag;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT));
				_vertexShaderId = shaderPool.pushVertexShader("RectangleRendererVS", kShaderString, "main", &typeInfo);
			}

			// Compile pixel shader
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>

					sampler		sampler0;
					Texture2D	texture0;
				
					float4 main(VS_OUTPUT input) : SV_Target
					{
						float4 result = input._color;
						if (input._flag == 1)
						{
							result = texture0.Sample(sampler0, input._texCoord);
						}
						else if (input._flag == 2)
						{
							result *= texture0.Sample(sampler0, input._texCoord);
						}
						return result;
					}
					)"
				};
				_pixelShaderId = shaderPool.pushNonVertexShader("RectangleRendererPS", kShaderString, "main", DxShaderType::PixelShader);
			}
		}

		void RectangleRendererContext::flushData() noexcept
		{
			_triangleRenderer.flush();
		}

		const bool RectangleRendererContext::hasData() const noexcept
		{
			return _triangleRenderer.isRenderable();
		}

		void RectangleRendererContext::render() noexcept
		{
			if (_triangleRenderer.isRenderable() == true)
			{
				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShaderId);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShaderId);
				_triangleRenderer.render();
			}
		}

		void RectangleRendererContext::drawColored()
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3)));

			prepareIndexArray();
		}

		void RectangleRendererContext::drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRendererContext::drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRendererContext::prepareIndexArray()
		{
			const auto& vertexArray = _triangleRenderer.vertexArray();
			const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		}
	}
}
