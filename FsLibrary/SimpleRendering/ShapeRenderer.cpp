#include <stdafx.h>
#include <SimpleRendering/ShapeRenderer.h>

#include <SimpleRendering/GraphicDevice.h>
#include <SimpleRendering/TriangleBuffer.hpp>

#include <Math/Float2.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		ShapeRenderer::ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: IRenderer(graphicDevice)
			, _shapeBuffer{ graphicDevice }
		{
			__noop;
		}

		void ShapeRenderer::initializeShaders() noexcept
		{
			fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					VS_OUTPUT_SHAPE main_shape(VS_INPUT_SHAPE input)
					{
						VS_OUTPUT_SHAPE result;
						result._position	= mul(float4(input._position.xyz, 1.0), _cbProjectionMatrix);
						result._position.w  = 1.0;
						result._infoA.xy	= input._infoA.xy;
						result._infoA.zw	= input._infoA.zw;
						result._infoB		= input._infoB;
						result._color		= input._color;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT_SHAPE));
				_vertexShader = shaderPool.pushVertexShader(kShaderString, "main_shape", DxShaderVersion::v_4_0, &typeInfo);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					static const float4 kTransparentColor = float4(0, 0, 0, 0);
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float2 kScreenRatio = float2(1.0, -_cbProjectionMatrix[0][0] / _cbProjectionMatrix[1][1]);
						const float2 p = mul(float4(input._position.xy, 0.0, 1.0), _cbProjectionMatrix).xy * kScreenRatio;
						const float2 c = input._infoA.xy * kScreenRatio;
						const float2 s = input._infoA.zw * kScreenRatio;
						const float r = input._infoB.x;

						float2 hs = s / 2.0;
						float2 offset = p - c + hs;
						if (0.0 < offset.x && offset.x < s.x &&
							0.0 < offset.y && offset.y < s.y)
						{
							const float a = (min(s.x, s.y) / 2.0) * r;
							bool isBottom = offset.y < a;
							bool isTop = (s.y - a) < offset.y;

							// left
							if (offset.x < a && (isBottom == true || isTop == true))
							{
								float d = distance(float2(a, (isTop) ? s.y - a : a), offset);
								if (a < d)
								{
									return kTransparentColor;
								}
							}

							// right
							if (s.x - a < offset.x && (isBottom == true || isTop == true))
							{
								float d = distance(float2(s.x - a, (isTop) ? s.y - a : a), offset);
								if (a < d)
								{
									return kTransparentColor;
								}
							}

							return input._color;
						}

						return kTransparentColor;
					}
					)"
				};
				_pixelShader = shaderPool.pushNonVertexShader(kShaderString, "main_shape", DxShaderVersion::v_4_0, DxShaderType::PixelShader);
			}
		}

		void ShapeRenderer::flushData() noexcept
		{
			_shapeBuffer.flush();
		}

		void ShapeRenderer::render() noexcept
		{
			if (_shapeBuffer.isReady() == true)
			{
				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShader);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShader);

				_shapeBuffer.render();
			}
		}

		void ShapeRenderer::drawRoundedRectangle(const fs::Int2& size, const float r)
		{
			const fs::Float2 positionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			
			CppHlsl::VS_INPUT_SHAPE v;
			v._position._x = positionF._x - halfSizeF._x;
			v._position._y = positionF._y - halfSizeF._y;
			v._color = _defaultColor;
			v._infoA._x = (positionF._x / windowSizeF._x) * 2.0f - 1.0f;
			v._infoA._y = (1.0f - (positionF._y / windowSizeF._y)) * 2.0f - 1.0f;
			v._infoA._z = (size._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (size._y / windowSizeF._y) * 2.0f;
			v._infoB._x = 0.5f;
			
			auto& vertexArray = _shapeBuffer.vertexArray();
			vertexArray.emplace_back(v);
			
			v._position._x = positionF._x + halfSizeF._x;
			v._position._y = positionF._y - halfSizeF._y;
			vertexArray.emplace_back(v);
			
			v._position._x = positionF._x - halfSizeF._x;
			v._position._y = positionF._y + halfSizeF._y;
			vertexArray.emplace_back(v);
			
			v._position._x = positionF._x + halfSizeF._x;
			v._position._y = positionF._y + halfSizeF._y;
			vertexArray.emplace_back(v);
			
			prepareIndexArray();
		}

		void ShapeRenderer::prepareIndexArray()
		{
			const auto& vertexArray = _shapeBuffer.vertexArray();
			const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

			auto& indexArray = _shapeBuffer.indexArray();
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 0);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 1);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 3);
			indexArray.push_back((currentTotalTriangleVertexCount - 4) + 2);
		}
	}
}
