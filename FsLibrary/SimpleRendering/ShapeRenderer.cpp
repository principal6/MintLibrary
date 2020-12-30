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
					
					static const float2 kScreenRatio = float2(1.0, -_cbProjectionMatrix[0][0] / _cbProjectionMatrix[1][1]);
					static const float4 kTransparentColor = float4(0, 0, 0, 0);
					
					bool roundedRectangle(in float2 p, in float2 s, in float r)
					{
						float2 hs = s / 2.0;
						float2 offset = p + hs;
						if (0.0 < offset.x && offset.x < s.x &&
							0.0 < offset.y && offset.y < s.y)
						{
							float a = (min(s.x, s.y) / 2.0) * r;
							bool isBottom = offset.y < a;
							bool isTop = s.y - a < offset.y;
							
							// left
							if (offset.x < a && (isBottom == true || isTop == true))
							{
								float d = distance(float2(a, (isTop) ? s.y - a : a), offset);
								if (a < d)
								{
									return false;
								}
							}
							
							// right
							if (s.x - a < offset.x && (isBottom == true || isTop == true))
							{
								float d = distance(float2(s.x - a, (isTop) ? s.y - a : a), offset);
								if (a < d)
								{
									return false;
								}
							}
							return true;
						}
						return false;
					}
					
					bool taperedRectangle(in float2 p, in float2 s, in float t, in float b)
					{
						float2 hs = s / 2.0;
						float2 offset = p + hs;
						if (0.0 < offset.x && offset.x < s.x &&
							0.0 < offset.y && offset.y < s.y)
						{
							t = clamp(t, 0.0, 1.0);
							b = clamp(b, 0.0, 1.0);
							
							float Msw = s.x * (1.0 - t); // max space width
							float Mtx = Msw * b;         // max tapering x offset
							
							float hr = (offset.y / s.y); // height ratio = [0.0, 1.0]
							float lt = t * hr;           // lerped t = [0.0, t]
							float lMtx = Mtx * hr;       // lerped tapering x offset
							float ltw = s.x * lt;        // lerped tapered width
							
							if ((lMtx < offset.x) && (offset.x + ltw - lMtx < s.x))
							{
								return true;
							}
							
							return false;
						}
						return false;
					}
					
					float2 normalizePosition(in float4 position)
					{
						return mul(float4(position.xy, 0.0, 1.0), _cbProjectionMatrix).xy * kScreenRatio;
					}

					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						float angle = input._infoB.z;
						float cosAngle = cos(angle);
						float sinAngle = sin(angle);
						float2x2 rotationMatrix = float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
						const float2 c = input._infoA.xy;
						const float2 p = mul(rotationMatrix, normalizePosition(input._position) - c);
						const float2 s = input._infoA.zw * kScreenRatio;
						
						if (input._infoB.w == 1.0)
						{						
							const float r = input._infoB.x;
							if (roundedRectangle(p, s, r) == true)
							{
								return input._color;
							}
						}
						else if (input._infoB.w == 2.0)
						{
							const float t = input._infoB.x;
							const float b = input._infoB.y;
							if (taperedRectangle(p, s, t, b) == true)
							{
								return input._color;
							}
						}
						return kTransparentColor;
					}
					)"
				};
				_pixelShader = shaderPool.pushNonVertexShader(kShaderString, "main", DxShaderVersion::v_4_0, DxShaderType::PixelShader);
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

		void ShapeRenderer::drawRoundedRectangle(const fs::Int2& size, const float roundness, const float angle)
		{
			const fs::Float2 positionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			
			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._infoA._x = (positionF._x / windowSizeF._x) * 2.0f - 1.0f;
			v._infoA._y = (1.0f - (positionF._y / windowSizeF._y)) * 2.0f - 1.0f;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = roundness;
			v._infoB._z = angle;
			v._infoB._w = 1.0f;

			prepareVertexArray(v, positionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::drawTaperedRectangle(const fs::Int2& size, const float tapering, const float bias, const float angle)
		{
			const fs::Float2 positionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._infoA._x = (positionF._x / windowSizeF._x) * 2.0f - 1.0f;
			v._infoA._y = (1.0f - (positionF._y / windowSizeF._y)) * 2.0f - 1.0f;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = tapering;
			v._infoB._y = bias;
			v._infoB._z = angle;
			v._infoB._w = 2.0f;

			prepareVertexArray(v, positionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::prepareVertexArray(fs::CppHlsl::VS_INPUT_SHAPE& data, const fs::Float2& position, const fs::Float2& halfSize)
		{
			auto& vertexArray = _shapeBuffer.vertexArray();
			data._position._x = position._x - halfSize._x;
			data._position._y = position._y - halfSize._y;
			vertexArray.emplace_back(data);

			data._position._x = position._x + halfSize._x;
			data._position._y = position._y - halfSize._y;
			vertexArray.emplace_back(data);

			data._position._x = position._x - halfSize._x;
			data._position._y = position._y + halfSize._y;
			vertexArray.emplace_back(data);

			data._position._x = position._x + halfSize._x;
			data._position._y = position._y + halfSize._y;
			vertexArray.emplace_back(data);
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
