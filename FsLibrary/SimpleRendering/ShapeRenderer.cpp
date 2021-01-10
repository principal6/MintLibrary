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
					
					static float2x2 getRotationMatrix(in float angle)
					{
						const float cosAngle = cos(angle);
						const float sinAngle = sin(angle);
						return float2x2(cosAngle, -sinAngle, sinAngle, cosAngle);
					}
					
					static const float		kPi = 3.1415926535;
					static const float		kPiOverTwo = kPi * 0.5;
					static const float		kPiOverFour = kPiOverTwo * 0.5;
					static const float		kDeltaTwoPixel = _cbProjectionMatrix[0][0];
					static const float		kDeltaPixel = kDeltaTwoPixel / 2.0;
					static const float		kAntiAliasingFactor = kDeltaTwoPixel;
					static const float2		kScreenRatio = float2(1.0, -_cbProjectionMatrix[0][0] / _cbProjectionMatrix[1][1]);
					static const float4		kTransparentColor = float4(0, 0, 0, 0);
					static const float		kRoundnessAbsoluteBase = 2.0;
					static const float2x2	kRotationMatrixRightAngle = getRotationMatrix(kPiOverTwo);
					
					float2 normalizePosition(in float4 position)
					{
						return mul(float4(position.xy, 0.0, 1.0), _cbProjectionMatrix).xy * kScreenRatio;
					}
					
					float calculateRoundAlphaForAntiAliasing(in float dist, in float radius)
					{
						const float displacement = dist - radius; // In noramlized pixel units
						return (displacement < -kAntiAliasingFactor) ? 1.0 : saturate(1.0 - saturate((displacement + kAntiAliasingFactor) / kAntiAliasingFactor));
					}
					
					
					float rectangle(in float2 p, in float2 s, in float2x2 rotationMatrix)
					{
						// Half-size
						const float2 hs = s / 2.0;
						
						// Rotated edge vectors
						const float3 vL = float3(mul(rotationMatrix, float2(0.0, -s.y)), 0.0);
						const float3 vT = float3(mul(rotationMatrix, float2(+s.x, 0.0)), 0.0);
						
						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						const float3 crossL = cross(vL, lt);
						const float3 crossT = cross(lt, vT);
						const float3 crossR = cross(-vL, rb);
						const float3 crossB = cross(rb, -vT);
						if (0.0 <= crossL.z && 0.0 <= crossT.z && 0.0 <= crossR.z && 0.0 <= crossB.z)
						{
							// Inside
							const float2 nL = normalize(vT);
							const float2 nT = normalize(vL);
							
							const float dotL = dot(lt, nL);
							const float dotT = dot(lt, nT);
							const float dotR = dot(rb, -nL);
							const float dotB = dot(rb, -nT);
							
							// Close to left border
							if (dotL <= kAntiAliasingFactor)
							{
								return dotL / kAntiAliasingFactor;
							}
							
							// Close to top border
							if (dotT <= kAntiAliasingFactor)
							{
								return dotT / kAntiAliasingFactor;
							}
							
							// Close to right border
							if (dotR <= kAntiAliasingFactor)
							{
								return dotR / kAntiAliasingFactor;
							}
							
							// Close to bottom border
							if (dotB <= kAntiAliasingFactor)
							{
								return dotB / kAntiAliasingFactor;
							}
							
							// Body
							return 1.0;
						}
						
						// Outside
						return 0.0;
					}

					float roundedRectangle(in float2 p, in float2 s, in float r, in float2x2 rotationMatrix)
					{
						// Half-size
						const float2 hs = s / 2.0;
						
						// Rotated edge vectors
						const float3 vL = float3(mul(rotationMatrix, float2(0.0, -s.y)), 0.0);
						const float3 vT = float3(mul(rotationMatrix, float2(+s.x, 0.0)), 0.0);
						
						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						const float3 crossL = cross(vL, lt);
						const float3 crossT = cross(lt, vT);
						const float3 crossR = cross(-vL, rb);
						const float3 crossB = cross(rb, -vT);
						if (0.0 <= crossL.z && 0.0 <= crossT.z && 0.0 <= crossR.z && 0.0 <= crossB.z)
						{
							// Inside
							const float radius = (kRoundnessAbsoluteBase <= r) ? r - kRoundnessAbsoluteBase : (min(s.x, s.y) / 2.0) * r;
							const float2 nL = normalize(vT);
							const float2 nT = normalize(vL);
							
							const float dotL = dot(lt, nL);
							const float dotT = dot(lt, nT);
							const float dotR = dot(rb, -nL);
							const float dotB = dot(rb, -nT);
							
							const bool isTop = dotT < radius;
							const bool isBottom = dotB < radius;
							
							// Left corners
							if (dotL < radius && (isTop == true || isBottom == true))
							{
								float dist = distance(float2(dotL, (isTop == true) ? dotT : dotB), float2(radius, radius));
								return calculateRoundAlphaForAntiAliasing(dist, radius);
							}
							
							// Right corners
							if (dotR < radius && (isTop == true || isBottom == true))
							{
								float dist = distance(float2(dotR, (isTop == true) ? dotT : dotB), float2(radius, radius));
								return calculateRoundAlphaForAntiAliasing(dist, radius);
							}
							
							// Close to left border
							if (dotL <= kAntiAliasingFactor)
							{
								return dotL / kAntiAliasingFactor;
							}
							
							// Close to top border
							if (dotT <= kAntiAliasingFactor)
							{
								return dotT / kAntiAliasingFactor;
							}
							
							// Close to right border
							if (dotR <= kAntiAliasingFactor)
							{
								return dotR / kAntiAliasingFactor;
							}
							
							// Close to bottom border
							if (dotB <= kAntiAliasingFactor)
							{
								return dotB / kAntiAliasingFactor;
							}
							
							// Body
							return 1.0;
						}
						
						return 0.0;
					}
					
					float taperedRectangle(in float2 p, in float2 s, in float t, in float b, in float2x2 rotationMatrix)
					{
						// Process inputs
						t = clamp(t, 0.0, 1.0);
						b = clamp(b, 0.0, 1.0);
						const float spaceLength = s.x * (1.0 - t);
						const float leftOffset = spaceLength * b;
						const float rightOffset = spaceLength * (1.0 - b);
						
						// Half-size
						const float2 hs = s / 2.0;
						
						// Original edge vectors
						const float2 ovL = float2(-leftOffset, -s.y);
						const float2 ovT = float2(+s.x, 0.0);
						const float2 ovR = float2(-rightOffset, +s.y); 
						const float2 ovB = float2(-s.x, 0.0);
						
						// Rotated edge vectors
						const float3 vL = float3(mul(rotationMatrix, ovL), 0.0);
						const float3 vT = float3(mul(rotationMatrix, ovT), 0.0);
						const float3 vR = float3(mul(rotationMatrix, ovR), 0.0);
						const float3 vB = float3(mul(rotationMatrix, ovB), 0.0);
						
						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x + leftOffset, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						const float3 crossL = cross(vL, lt);
						const float3 crossT = cross(lt, vT);
						const float3 crossR = cross(vR, rb);
						const float3 crossB = cross(rb, vB);
						if (0.0 <= crossL.z && 0.0 <= crossT.z && 0.0 <= crossR.z && 0.0 <= crossB.z)
						{
							// Inside
							const float2 nL = normalize(mul(kRotationMatrixRightAngle,  vL));
							const float2 nT = normalize(mul(kRotationMatrixRightAngle, -vT));
							const float2 nR = normalize(mul(kRotationMatrixRightAngle,  vR));
							const float2 nB = normalize(mul(kRotationMatrixRightAngle, -vB));
							
							const float dotL = dot(lt, nL);
							const float dotT = dot(lt, nT);
							const float dotR = dot(rb, nR);
							const float dotB = dot(rb, nB);
							
							// Close to left border
							if (dotL <= kAntiAliasingFactor)
							{
								return dotL / kAntiAliasingFactor;
							}
							
							// Close to top border
							if (dotT <= kAntiAliasingFactor)
							{
								return dotT / kAntiAliasingFactor;
							}
							
							// Close to right border
							if (dotR <= kAntiAliasingFactor)
							{
								return dotR / kAntiAliasingFactor;
							}
							
							// Close to bottom border
							if (dotB <= kAntiAliasingFactor)
							{
								return dotB / kAntiAliasingFactor;
							}
							
							// Body
							return 1.0;
						}
						
						return 0.0;
					}

					float bluntLine(in float2 pixelPosition, in float2 l0, in float2 l1, in float thickness)
					{
						float thicknessInPixels = kDeltaPixel * thickness;
						float2 segment = l1 - l0;
						float segmentLength = length(segment);
						float2 direction = segment / segmentLength;
						
						float2 p0 = pixelPosition - l0;
						float t = dot(p0, direction);
						float2 lp = l0 + direction * t;
    
						if ((t < 0.0) || (segmentLength < t))
						{
							float distToBorder = distance(pixelPosition, (t < 0.0) ? l0 : l1);
							return calculateRoundAlphaForAntiAliasing(distToBorder, thicknessInPixels);
						}
    
						float dist = distance(lp, pixelPosition);
						if (dist < thicknessInPixels)
						{
							return calculateRoundAlphaForAntiAliasing(dist, thicknessInPixels);
						}
						
						return 0.0;
					}
					
					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float angle = input._infoB.z;
						
						if (input._infoB.w == 0.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float alphaFactor = rectangle(p, s, rotationMatrix);
							return input._color * float4(1.0, 1.0, 1.0, alphaFactor);
						}
						else if (input._infoB.w == 1.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float r = input._infoB.x;
							const float alphaFactor = roundedRectangle(p, s, r, rotationMatrix);
							return input._color * float4(1.0, 1.0, 1.0, alphaFactor);
						}
						else if (input._infoB.w == 2.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float t = input._infoB.x;
							const float b = input._infoB.y;
							const float alphaFactor = taperedRectangle(p, s, t, b, rotationMatrix);
							return input._color * float4(1.0, 1.0, 1.0, alphaFactor);							
						}
						else if (input._infoB.w == 3.0)
						{
							const float2 p = normalizePosition(input._position);
							
							const float2 l0 = input._infoA.xy * kScreenRatio;
							const float2 l1 = input._infoA.zw * kScreenRatio;
							const float2 thickness = input._infoB.x;
							const float alphaFactor = bluntLine(p, l0, l1, thickness);
							return input._color * float4(1.0, 1.0, 1.0, alphaFactor);
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

		void ShapeRenderer::drawRectangle(const fs::Int2& size, const float angle)
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
			v._infoB._z = angle;
			v._infoB._w = 0.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
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
			v._infoA._z = normalizedP1._x;
			v._infoA._w = normalizedP1._y;
			v._infoB._x = thickness;
			v._infoB._w = 3.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) + thickness * 1.25f));
			prepareIndexArray();
		}

		fs::Float2 ShapeRenderer::normalizePosition(const fs::Float2& position, const fs::Float2& screenSize)
		{
			return fs::Float2((position._x / screenSize._x) * 2.0f - 1.0f, -((position._y / screenSize._y) * 2.0f - 1.0f));
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
