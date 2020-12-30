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
				_vertexShader = shaderPool.pushVertexShader("ShapeRendererVS", kShaderString, "main_shape", DxShaderVersion::v_4_0, &typeInfo);
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

					static const float kLineThicknessFactor = 0.002;
					bool bluntLine(in float2 pixelPosition, in float2 l0, in float2 l1, in float thickness)
					{
						float factoredThickness = kLineThicknessFactor * thickness;
						float2 segment = l1 - l0;
						float2 direction = normalize(segment);
						float2 p0 = pixelPosition - l0;
						float t = dot(p0, direction);
						float2 lp = l0 + direction * t;
    
						float lsl = length(segment);
						if ((t < 0.0) || (lsl < t))
						{
							float d0 = distance(pixelPosition, (t < 0.0) ? l0 : l1);
							if (factoredThickness <= d0)
							{
								return false;
							}
						}
    
						float dist = distance(lp, pixelPosition);
						if (dist < factoredThickness)
						{
							return true;
						}
						return false;
					}
					
					float2 normalizePosition(in float4 position)
					{
						return mul(float4(position.xy, 0.0, 1.0), _cbProjectionMatrix).xy * kScreenRatio;
					}

					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float angle = input._infoB.z;
						
						if (input._infoB.w == 1.0)
						{
							const float cosAngle = cos(angle);
							const float sinAngle = sin(angle);
							const float2x2 rotationMatrix = float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
							const float2 c = input._infoA.xy;
							const float2 p = mul(rotationMatrix, normalizePosition(input._position) - c);
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float r = input._infoB.x;
							if (roundedRectangle(p, s, r) == true)
							{
								return input._color;
							}
						}
						else if (input._infoB.w == 2.0)
						{
							const float cosAngle = cos(angle);
							const float sinAngle = sin(angle);
							const float2x2 rotationMatrix = float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
							const float2 c = input._infoA.xy;
							const float2 p = mul(rotationMatrix, normalizePosition(input._position) - c);
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float t = input._infoB.x;
							const float b = input._infoB.y;
							if (taperedRectangle(p, s, t, b) == true)
							{
								return input._color;
							}
						}
						else if (input._infoB.w == 3.0)
						{
							const float2 p = normalizePosition(input._position);
							
							const float2 l0 = input._infoA.xy * kScreenRatio;
							const float2 l1 = input._infoA.zw * kScreenRatio;
							const float2 thickness = input._infoB.x;
							if (bluntLine(p, l0, l1, thickness) == true)
							{
								return input._color;
							}
						}
						return kTransparentColor;
					}
					)"
				};
				_pixelShader = shaderPool.pushNonVertexShader("ShapeRendererPS", kShaderString, "main", DxShaderVersion::v_4_0, DxShaderType::PixelShader);
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
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;
			
			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = roundness;
			v._infoB._z = angle;
			v._infoB._w = 1.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::drawTaperedRectangle(const fs::Int2& size, const float tapering, const float bias, const float angle)
		{
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = tapering;
			v._infoB._y = bias;
			v._infoB._z = angle;
			v._infoB._w = 2.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::drawLine(const fs::Int2& p0, const fs::Int2& p1, const float thickness)
		{
			const fs::Float2 p0F = fs::Float2(p0);
			const fs::Float2 p1F = fs::Float2(p1);
			const fs::Float2 centerPositionF = (p0F + p1F) * 0.5f;
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedP0 = normalizePosition(p0F, windowSizeF);
			const fs::Float2 normalizedP1 = normalizePosition(p1F, windowSizeF);
			const fs::Float2 sizeF = fs::Float2::abs(fs::Float2(p0 - p1));
			const fs::Float2 halfSizeF = sizeF * 0.5f;

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._infoA._x = normalizedP0._x;
			v._infoA._y = normalizedP0._y;
			v._infoB._z = normalizedP1._x;
			v._infoB._w = normalizedP1._y;
			v._infoB._x = thickness;
			v._infoB._w = 3.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		fs::Float2 ShapeRenderer::normalizePosition(const fs::Float2& position, const fs::Float2& screenSize)
		{
			return fs::Float2((position._x / screenSize._x) * 2.0f - 1.0f, (1.0f - (position._y / screenSize._y)) * 2.0f - 1.0f);
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
