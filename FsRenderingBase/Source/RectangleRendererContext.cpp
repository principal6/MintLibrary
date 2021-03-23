#include <stdafx.h>
#include <FsRenderingBase/Include/RectangleRendererContext.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/TriangleRenderer.hpp>


namespace fs
{
	namespace RenderingBase
	{
		FS_INLINE fs::Float4 getVertexPosition(const uint32 vertexIndex, const fs::Float4& position, const fs::Float2& size)
		{
			return fs::Float4(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y, position._z, position._w);
		}

		FS_INLINE fs::Float2 getVertexTexturePosition(const uint32 vertexIndex, const fs::Float2& position, const fs::Float2& size)
		{
			return fs::Float2(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y);
		}

		RectangleRendererContext::RectangleRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice)
			: IRendererContext(graphicDevice)
			, _triangleRenderer{ graphicDevice }
		{
			__noop;
		}

		void RectangleRendererContext::initializeShaders() noexcept
		{
			fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

			// Compile vertex shader and create input layer
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					VS_OUTPUT_SHAPE main(VS_INPUT_SHAPE input)
					{
						VS_OUTPUT_SHAPE result	= (VS_OUTPUT_SHAPE)0;
						result._position		= mul(float4(input._position.xyz, 1.0), _cb2DProjectionMatrix);
						result._color			= input._color;
						result._texCoord		= input._texCoord;
						result._info			= input._info;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslSteamData().getTypeInfo(typeid(fs::RenderingBase::VS_INPUT_SHAPE));
				_vertexShaderId = shaderPool.pushVertexShaderFromMemory("RectangleRendererVS", kShaderString, "main", &typeInfo);
			}

			// Compile pixel shader
			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>

					sampler		sampler0;
					Texture2D	texture0;
				
					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						float4 result = input._color;
						if (input._info.x == 1.0)
						{
							result = texture0.Sample(sampler0, input._texCoord);
						}
						else if (input._info.x == 2.0)
						{
							result *= texture0.Sample(sampler0, input._texCoord);
						}
						return result;
					}
					)"
				};
				_pixelShaderId = shaderPool.pushNonVertexShaderFromMemory("RectangleRendererPS", kShaderString, "main", DxShaderType::PixelShader);
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
				fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vertexShaderId);
				shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _pixelShaderId);
				_triangleRenderer.render();
			}
		}

		void RectangleRendererContext::drawColored()
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			fs::RenderingBase::VS_INPUT_SHAPE vertex;
			for (uint32 iter = 0; iter < 4; iter++)
			{
				vertex._position = getVertexPosition(iter, _position, _size);
				vertex._color = getColorInternal(iter);
				vertexArray.emplace_back(vertex);
			}
			prepareIndexArray();
		}

		void RectangleRendererContext::drawTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			fs::RenderingBase::VS_INPUT_SHAPE vertex;
			for (uint32 iter = 0; iter < 4; iter++)
			{
				vertex._position = getVertexPosition(iter, _position, _size);
				const fs::Float2& texCoord = getVertexTexturePosition(iter, texturePosition, textureSize);
				vertex._texCoord._x = texCoord._x;
				vertex._texCoord._y = texCoord._y;
				vertexArray.emplace_back(vertex);
			}
			prepareIndexArray();
		}

		void RectangleRendererContext::drawColoredTextured(const fs::Float2& texturePosition, const fs::Float2& textureSize)
		{
			auto& vertexArray = _triangleRenderer.vertexArray();
			fs::RenderingBase::VS_INPUT_SHAPE vertex;
			for (uint32 iter = 0; iter < 4; iter++)
			{
				vertex._position = getVertexPosition(iter, _position, _size);
				vertex._color = getColorInternal(iter);
				const fs::Float2& texCoord = getVertexTexturePosition(iter, texturePosition, textureSize);
				vertex._texCoord._x = texCoord._x;
				vertex._texCoord._y = texCoord._y;
				vertexArray.emplace_back(vertex);
			}
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
