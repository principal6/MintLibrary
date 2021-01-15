#include <stdafx.h>
#include <FsLibrary/SimpleRendering/RectangleRenderer.h>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		FS_INLINE fs::Float3 getVertexPosition(const uint32 vertexIndex, const fs::Float3& position, const fs::Float2& size)
		{
			return fs::Float3(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y, position._z);
		}

		FS_INLINE fs::Float2 getVertexTexturePosition(const uint32 vertexIndex, const fs::Float2& position, const fs::Float2& size)
		{
			return fs::Float2(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y);
		}

		RectangleRenderer::RectangleRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: IRenderer(graphicDevice)
			, _triangleRenderer{ graphicDevice }
		{
			__noop;
		}

		void RectangleRenderer::initializeShaders() noexcept
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
				_vertexShader = shaderPool.pushVertexShader("RectangleRendererVS", kShaderString, "main", &typeInfo);
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
				_pixelShader = shaderPool.pushNonVertexShader("RectangleRendererPS", kShaderString, "main", DxShaderType::PixelShader);
			}
		}

		void RectangleRenderer::flushData() noexcept
		{
			_triangleRenderer.flush();
		}

		void RectangleRenderer::render() noexcept
		{
			if (_triangleRenderer.isRenderable() == true)
			{
				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShader);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShader);
				_triangleRenderer.render();
			}
		}

		void RectangleRenderer::drawColored()
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3)));

			prepareIndexArray();
		}

		void RectangleRenderer::drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRenderer::drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(0, _position, _size), getColorInternal(0), getVertexTexturePosition(0, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(1, _position, _size), getColorInternal(1), getVertexTexturePosition(1, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(2, _position, _size), getColorInternal(2), getVertexTexturePosition(2, texturePosition, textureSize)));
			vertexArray.emplace_back(fs::CppHlsl::VS_INPUT(getVertexPosition(3, _position, _size), getColorInternal(3), getVertexTexturePosition(3, texturePosition, textureSize)));

			prepareIndexArray();
		}

		void RectangleRenderer::prepareIndexArray()
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
