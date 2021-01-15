#include <stdafx.h>
#include <FsLibrary/SimpleRendering/ShapeRenderer.h>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>
#include <FsLibrary/SimpleRendering/TriangleBuffer.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		ShapeRenderer::ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: IRenderer(graphicDevice)
			, _shapeBuffer{ graphicDevice }
			, _shapeBufferFast{ graphicDevice }
			, _borderColor{ fs::Float4(1.0f, 1.0f, 1.0f, 1.0f) }
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
						result._infoA		= input._infoA;
						result._infoB		= input._infoB;
						result._infoC		= input._infoC;
						result._borderColor	= input._borderColor;
						result._color		= input._color;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT_SHAPE));
				_vertexShader = shaderPool.pushVertexShader("ShapeRendererVS", kShaderString, "main_shape", &typeInfo);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					VS_OUTPUT_SHAPE_FAST main_shape(VS_INPUT_SHAPE_FAST input)
					{
						VS_OUTPUT_SHAPE_FAST result;
						result._position	= float4(mul(float4(input._position.xy, 0.0, 1.0), _cbProjectionMatrix).xy, 0.0, 1.0);
						result._color		= input._color;
						result._texCoord	= input._texCoord;
						result._info.xy		= input._position.zw;
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT_SHAPE_FAST));
				_vertexShaderFast = shaderPool.pushVertexShader("ShapeRendererVSFast", kShaderString, "main_shape", &typeInfo);
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
					static const float		kTwoPi = kPi * 2.0;
					static const float		kDeltaTwoPixel = _cbProjectionMatrix[0][0];
					static const float		kDeltaPixel = kDeltaTwoPixel / 2.0;
					static const float		kAntiAliasingFactor = kDeltaTwoPixel;
					static const float2		kScreenRatio = float2(1.0, -_cbProjectionMatrix[0][0] / _cbProjectionMatrix[1][1]);
					static const float4		kTransparentColor = float4(0, 0, 0, 0);
					static const float		kRoundnessAbsoluteBase = 2.0;
					static const float2x2	kRotationMatrixRightAngle = getRotationMatrix(kPiOverTwo);
					static const float		kDeltaAngle = kPi / 180.0; // == 1 degree
					static const float		kDeltaDoubleAngle = kDeltaAngle * 2.0;
					
					float2 normalizePosition(in float4 position)
					{
						return mul(float4(position.xy, 0.0, 1.0), _cbProjectionMatrix).xy * kScreenRatio;
					}
					
					float calculateRoundAlphaForAntiAliasing(in float dist, in float radius)
					{
						const float displacement = dist - radius; // In noramlized pixel units
						return (displacement < -kAntiAliasingFactor) ? 1.0 : saturate(1.0 - saturate((displacement + kAntiAliasingFactor) / kAntiAliasingFactor));
					}
					
					float getClosestSignedDistanceFromEdges(in float2 lt, in float2 rb, in float2 nL, in float2 nT, in float2 nR, in float2 nB)
					{
						return min(dot(lt, nL), min(dot(lt, nT), min(dot(rb, nR), dot(rb, nB))));
					}
					
					float rectangle(in float2 p, in float2 s, in float2x2 rotationMatrix)
					{
						// Half-size
						const float2 hs = s / 2.0;
						
						// Rotated edge vectors
						const float3 vL = float3(mul(rotationMatrix, float2(0.0, -s.y)), 0.0);
						const float3 vT = float3(mul(rotationMatrix, float2(+s.x, 0.0)), 0.0);
						
						// Edge normals
						const float2 nL = normalize(vT);
						const float2 nT = normalize(vL);
						
						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						// Get closest signed distance
						return getClosestSignedDistanceFromEdges(lt, rb, nL, nT, -nL, -nT);
					}

					float roundedRectangle(in float2 p, in float2 s, in float r, in float2x2 rotationMatrix)
					{
						// Half-size
						const float2 hs = s / 2.0;
						
						// Rotated edge vectors
						const float3 vL = float3(mul(rotationMatrix, float2(0.0, -s.y)), 0.0);
						const float3 vT = float3(mul(rotationMatrix, float2(+s.x, 0.0)), 0.0);
						
						// Edge normals
						const float2 nL = normalize(vT);
						const float2 nT = normalize(vL);
						
						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						// Corner radius
						const float radius = (kRoundnessAbsoluteBase <= r) ? r - kRoundnessAbsoluteBase : (min(s.x, s.y) / 2.0) * r;
						
						// If at corner, calculate distance
						const bool isTop = -lt.y <= radius;
						const bool isBottom = (s.y + lt.y) <= radius;
						if (lt.x <= radius && (isTop == true || isBottom == true))
						{
							// ContiguousVector pointing to the left corner
							float2 vC = (float2(lt.x, -lt.y) - float2(radius, (isTop == true) ? radius : s.y - radius));
							return radius - length(vC);
						}
						else if (-rb.x <= radius && (isTop == true || isBottom == true))
						{
							// ContiguousVector pointing to the right corner
							float2 vC = (float2(-rb.x, rb.y) - float2(radius, (isTop == true) ? s.y - radius : radius));
							return radius - length(vC);
						}
						
						// Get closest signed distance
						return getClosestSignedDistanceFromEdges(lt, rb, nL, nT, -nL, -nT);
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
						
						// Edge normals
						const float2 nL = normalize(mul(kRotationMatrixRightAngle,  vL));
						const float2 nT = normalize(mul(kRotationMatrixRightAngle, -vT));
						const float2 nR = normalize(mul(kRotationMatrixRightAngle,  vR));
						const float2 nB = normalize(mul(kRotationMatrixRightAngle, -vB));

						// Rotated position (offset from corners)
						const float2 toLt = mul(rotationMatrix, float2(-hs.x + leftOffset, +hs.y));
						const float2 toRb = mul(rotationMatrix, float2(+hs.x, -hs.y));
						const float3 lt = float3(p - toLt, 0.0); // Offset from left-top corner
						const float3 rb = float3(p - toRb, 0.0); // Offset from right-bottom corner
						
						// Get closest signed distance
						return getClosestSignedDistanceFromEdges(lt, rb, nL, nT, nR, nB);
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
					
					float normalizeAngle(in float angle)
					{
						if (kTwoPi < angle)
						{
							return -kTwoPi + (angle % kTwoPi);
						}
						else if (angle < -kTwoPi)
						{
							return kTwoPi - (angle % kTwoPi);
						}
						return angle;
					}
					
					float getSignedDistanceToBetweenAngles(in float angle, in float normalizedAngleA, in float normalizedAngleB)
					{
						if (angle < normalizedAngleA)
						{
							const float angleDiff = normalizedAngleA - angle;
							return -(angleDiff / kDeltaAngle) * kAntiAliasingFactor; // Convert to pixel units
						}
						else if (normalizedAngleB < angle)
						{
							const float angleDiff = angle - normalizedAngleB;
							return -(angleDiff / kDeltaAngle) * kAntiAliasingFactor; // Convert to pixel units
						}
						return 1.0f;
					}
					
					float circularArc(in float2 p, in float2 center, in float radius, in float arcAngleA, in float arcAngleB, in float innerRadius)
					{
						const float dist = length(p);
						const float dotDir = dot(float2(0, 1), p / dist);
						const float angle = ((p.x < 0.0) ? -acos(dotDir) : acos(dotDir)); // [-pi, +pi]
						
						const float angleL = angle - kPi; // [-2pi, 0]
						const float angleR = angle + kPi; // [0, 2pi]
						
						// Between angles
						const float signedDistance = radius - dist - kDeltaTwoPixel;
						if (-kAntiAliasingFactor < signedDistance && innerRadius - kAntiAliasingFactor < dist)
						{
							// [-2pi, +2pi]
							const float normalizedAngleA = normalizeAngle(arcAngleA);
							const float normalizedAngleB = normalizeAngle(arcAngleB);
							
							const float resultL = getSignedDistanceToBetweenAngles(angleL, normalizedAngleA, normalizedAngleB);
							if (1.0f != resultL)
							{
								return getSignedDistanceToBetweenAngles(angleR, normalizedAngleA, normalizedAngleB);
							}
							return resultL;
						}
						
						// Inner
						if (dist < innerRadius)
						{
							return (dist - innerRadius);
						}
						
						// Outer
						return signedDistance;
					}
					
					float4 calculateFinalColor(in float signedDistance, in float borderThickness, in float4 backgroundColor, in float4 borderColor)
					{
						if (0.0 < signedDistance)
						{
							if (0.0 < borderThickness)
							{
								// Blend background and border color
								const float backgroundRatio = saturate(signedDistance / kAntiAliasingFactor);
								return backgroundColor * backgroundRatio + borderColor * (1.0 - backgroundRatio);
							}
							
							// No border
							return backgroundColor;
						}
						
						if (0.0 < borderThickness)
						{
							// Border color
							const float alpha = saturate((borderThickness + kAntiAliasingFactor - abs(signedDistance)) / kAntiAliasingFactor);
							return borderColor * float4(1.0, 1.0, 1.0, alpha);
						}
						
						// No border
						const float alpha = 1.0 - saturate(abs(signedDistance) / kAntiAliasingFactor);
						return backgroundColor * float4(1.0, 1.0, 1.0, alpha);
					}
					
					float4 main(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float angle = input._infoB.z;
						const float borderThickness = kDeltaTwoPixel * input._infoC.x;
						
						if (input._infoB.w == 0.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							return calculateFinalColor(rectangle(p, s, rotationMatrix), borderThickness, input._color, input._borderColor);
						}
						else if (input._infoB.w == 1.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float r = input._infoB.x;
							return calculateFinalColor(roundedRectangle(p, s, r, rotationMatrix), borderThickness, input._color, input._borderColor);
						}
						else if (input._infoB.w == 2.0)
						{
							const float2x2 rotationMatrix = getRotationMatrix(angle);
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float2 s = input._infoA.zw * kScreenRatio;
							const float t = input._infoB.x;
							const float b = input._infoB.y;
							return calculateFinalColor(taperedRectangle(p, s, t, b, rotationMatrix), borderThickness, input._color, input._borderColor);
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
						else if (input._infoB.w == 4.0)
						{
							const float2 c = input._infoA.xy * kScreenRatio;
							const float2 p = normalizePosition(input._position) - c;
							
							const float radius = input._infoA.z;
							const float arcAngleA = input._infoA.w;
							const float arcAngleB = input._infoB.x;
							const float innerRadius = input._infoB.y;
							return calculateFinalColor(circularArc(p, c, radius, arcAngleA, arcAngleB, innerRadius), 0.0, input._color, kTransparentColor);
						}
						return kTransparentColor;
					}
					)"
				};
				_pixelShader = shaderPool.pushNonVertexShader("ShapeRendererPS", kShaderString, "main", DxShaderType::PixelShader);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					static const float kDeltaDoublePixel = _cbProjectionMatrix[0][0];
					static const float kDeltaPixel = kDeltaDoublePixel * 0.5;
					
					float4 main_shape(VS_OUTPUT_SHAPE_FAST input) : SV_Target
					{
						const float u = input._texCoord.x;
						const float v = input._texCoord.y;
						
						float signedDistance = 0.0;
						if (1.0 == input._info.x)
						{
							// Solid
							return input._color;
						}
						else if (2.0 == input._info.x)
						{
							if (v < u)
							{
								return input._color;
							}
							
							// Circular section
							const float x = (1.0 - u);
							signedDistance = sqrt(1.0 - x * x) - v;
						}
						else
						{
							// Quadratic Bezier
							signedDistance = -(u * u - v);
						}
						clip(signedDistance + kDeltaDoublePixel);
						
						const float alpha = saturate(signedDistance / kDeltaDoublePixel);
						return float4(input._color.xyz, input._color.w * alpha);
					}
					)"
				};
				_pixelShaderFast = shaderPool.pushNonVertexShader("ShapeRendererPSFast", kShaderString, "main_shape", DxShaderType::PixelShader);
			}
		}

		void ShapeRenderer::flushData() noexcept
		{
			_shapeBuffer.flush();
			_shapeBufferFast.flush();
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

			if (_shapeBufferFast.isReady() == true)
			{
				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShaderFast);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShaderFast);

				_shapeBufferFast.render();
			}
		}

		void ShapeRenderer::setBorderColor(const fs::Float4& borderColor) noexcept
		{
			_borderColor = borderColor;
		}

		void ShapeRenderer::drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate)
		{
			const fs::Float2(&pointArray)[2] = { pointA, pointB };
			auto& vertexArray = _shapeBufferFast.vertexArray();

			uint8 flip = 0;
			if (validate == true)
			{
				// The control point must be on the left side of the AB segment!
				const fs::Float3 ac = fs::Float3(controlPoint - pointA);
				const fs::Float3 ab = fs::Float3(pointB - pointA);
				const fs::Float3& cross = fs::Float3::cross(ab, ac);
				flip = (cross._z > 0.0f) ? 1 : 0; // y 좌표계가 (아래가 + 방향으로) 뒤집혀 있어서 z 값 비교도 뒤집혔다.
			}
			
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = pointArray[0 ^ flip]._x;
			v._position._y = pointArray[0 ^ flip]._y;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			v._position._x = controlPoint._x;
			v._position._y = controlPoint._y;
			v._texCoord._x = 0.5f;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			v._position._x = pointArray[1 ^ flip]._x;
			v._position._y = pointArray[1 ^ flip]._y;
			v._texCoord._x = 1.0f;
			v._texCoord._y = 1.0f;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - 3;
			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC)
		{
			auto& vertexArray = _shapeBufferFast.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = pointA._x;
			v._position._y = pointA._y;
			v._position._z = kInfoSolid;
			vertexArray.emplace_back(v);

			v._position._x = pointB._x;
			v._position._y = pointB._y;
			vertexArray.emplace_back(v);

			v._position._x = pointC._x;
			v._position._y = pointC._y;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - 3;
			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawCircularTriangle(const float radius, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const float halfRadius = radius * 0.5f;

			auto& vertexArray = _shapeBufferFast.vertexArray();
			
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -halfRadius;
			v._position._y = -halfRadius;
			v._position._z = kInfoCircular;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 1.0f;
			vertexArray.emplace_back(v);

			v._position._x += radius;
			v._texCoord._x = 1.0f;
			v._texCoord._y = 1.0f;
			vertexArray.emplace_back(v);

			v._position._x = -halfRadius;
			v._position._y += radius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			transformVertices(vertexOffset, kDeltaVertexCount, rotationAngle);

			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawQuarterCircle(const float radius, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const float halfRadius = radius * 0.5f;
			
			auto& vertexArray = _shapeBufferFast.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -halfRadius;
			v._position._y = -halfRadius;
			v._position._z = kInfoCircular;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 1.0f;
			vertexArray.emplace_back(v);

			v._position._x += radius;
			v._texCoord._x = 1.0f;
			v._texCoord._y = 1.0f;
			vertexArray.emplace_back(v);

			v._position._x = -halfRadius;
			v._position._y += radius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			v._position._x += radius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - 4;
			transformVertices(vertexOffset, kDeltaVertexCount, rotationAngle);

			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
			
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawRectangleFast(const fs::Float2& size, const float borderThickness, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const fs::Float2 halfSize = size * 0.5f;

			auto& vertexArray = _shapeBufferFast.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -halfSize._x;
			v._position._y = -halfSize._y;
			v._position._z = kInfoSolid;
			v._position._w = rotationAngle;
			vertexArray.emplace_back(v);

			v._position._x += size._x;
			v._position._y = -halfSize._y;
			vertexArray.emplace_back(v);

			v._position._x = -halfSize._x;
			v._position._y = +halfSize._y;
			vertexArray.emplace_back(v);

			v._position._x += size._x;
			v._position._y = +halfSize._y;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			transformVertices(vertexOffset, kDeltaVertexCount, rotationAngle);

			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawTaperedRectangleFast(const fs::Float2& size, const float tapering, const float bias, const float borderThickness, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const fs::Float2 halfSize = size * 0.5f;
			const float horizontalSpace = size._x * (1.0f - tapering);
			const float horizontalOffsetL = horizontalSpace * bias;
			const float horizontalOffsetR = horizontalSpace * (1.0f - bias);

			auto& vertexArray = _shapeBufferFast.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -halfSize._x + horizontalOffsetL;
			v._position._y = -halfSize._y;
			v._position._z = kInfoSolid;
			vertexArray.emplace_back(v);

			v._position._x = +halfSize._x - horizontalOffsetR;
			v._position._y = -halfSize._y;
			vertexArray.emplace_back(v);

			v._position._x = -halfSize._x;
			v._position._y = +halfSize._y;
			vertexArray.emplace_back(v);

			v._position._x += size._x;
			v._position._y = +halfSize._y;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			transformVertices(vertexOffset, kDeltaVertexCount, rotationAngle);

			auto& indexArray = _shapeBufferFast.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawRoundedRectangleFast(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle)
		{
			const fs::Float2 halfSize = size * 0.5f;
			const float radius = fs::min(size._x, size._y) * fs::saturate(roundness);
			const float doubleRadius = radius * 2.0f;
			const float middleWidth = size._x - doubleRadius;
			const float middleHeight = size._y - doubleRadius;
			const float halfMiddleWidth = middleWidth * 0.5f;
			const float halfMiddleHeight = middleHeight * 0.5f;

#define FS_USE_FASTER_ROUNDED_RECTANGLE_METHOD
#if !defined FS_USE_FASTER_ROUNDED_RECTANGLE_METHOD
			const fs::Float3 originalPosition = _position;
			const float halfRadius = radius * 0.5f;

			// Center box
			drawRectangleFast(fs::Float2(middleWidth, size._y), 0.0f, 0.0f);

			// Left side box
			setPosition(originalPosition + fs::Float3(-(halfMiddleWidth + halfRadius), 0.0f, 0.0f));
			drawRectangleFast(fs::Float2(radius, middleHeight), 0.0f, 0.0f);

			// Right side box
			setPosition(originalPosition + fs::Float3(+(halfMiddleWidth + halfRadius), 0.0f, 0.0f));
			drawRectangleFast(fs::Float2(radius, middleHeight), 0.0f, 0.0f);

			// Left top corner
			setPosition(originalPosition + fs::Float3(-(halfMiddleWidth + halfRadius), -(halfMiddleHeight + halfRadius), 0.0f));
			drawQuarterCircle(radius, 0.0f);

			// Left bottom corner
			setPosition(originalPosition + fs::Float3(-(halfMiddleWidth + halfRadius), +(halfMiddleHeight + halfRadius), 0.0f));
			drawQuarterCircle(radius, fs::Math::kPiOverTwo);

			// Right top corner
			setPosition(originalPosition + fs::Float3(+(halfMiddleWidth + halfRadius), -(halfMiddleHeight + halfRadius), 0.0f));
			drawQuarterCircle(radius, -fs::Math::kPiOverTwo);

			// Right bottom corner
			setPosition(originalPosition + fs::Float3(+(halfMiddleWidth + halfRadius), +(halfMiddleHeight + halfRadius), 0.0f));
			drawQuarterCircle(radius, fs::Math::kPi);
			
			// Restore position
			setPosition(originalPosition);
#else
			const fs::Float2 position2 = fs::Float2(_position._x, _position._y);
			fs::Float2 pointA;
			fs::Float2 pointB;
			fs::Float2 pointC;

			// Center box
			{
				pointA = position2 + fs::Float2(-halfMiddleWidth, -halfSize._y);
				pointB = position2 + fs::Float2(+halfMiddleWidth, -halfSize._y);
				pointC = position2 + fs::Float2(-halfMiddleWidth, +halfSize._y);
				drawSolidTriangle(pointA, pointB, pointC);

				pointA = position2 + fs::Float2(+halfMiddleWidth, +halfSize._y);
				drawSolidTriangle(pointC, pointB, pointA);
			}

			// Left top corner
			{
				pointA = position2 + fs::Float2(-halfSize._x, -halfMiddleHeight);
				pointB = position2 + fs::Float2(-halfMiddleWidth, -halfSize._y);
				drawQuadraticBezier(pointA, pointB, position2 - halfSize, false);
			}

			// Left side box
			{
				pointC = position2 + fs::Float2(-halfMiddleWidth, +halfMiddleHeight);
				drawSolidTriangle(pointA, pointB, pointC);

				pointA = position2 + fs::Float2(-halfSize._x, -halfMiddleHeight);
				pointB = position2 + fs::Float2(-halfSize._x, +halfMiddleHeight);
				drawSolidTriangle(pointC, pointB, pointA);
			}

			// Left bottom corner
			{
				pointA = position2 + fs::Float2(-halfSize._x, +halfMiddleHeight);
				pointB = position2 + fs::Float2(-halfMiddleWidth, +halfSize._y);
				drawQuadraticBezier(pointB, pointA, position2 + fs::Float2(-halfSize._x, +halfSize._y), false);
				drawSolidTriangle(pointB, pointA, position2 + fs::Float2(-halfMiddleWidth, +halfMiddleHeight));
			}

			// Right top corner
			{
				pointA = position2 + fs::Float2(+halfSize._x, -halfMiddleHeight);
				pointB = position2 + fs::Float2(+halfMiddleWidth, -halfSize._y);
				drawQuadraticBezier(pointB, pointA, position2 + fs::Float2(+halfSize._x, -halfSize._y), false);
			}

			// Right side box
			{
				pointC = position2 + fs::Float2(+halfMiddleWidth, +halfMiddleHeight);
				drawSolidTriangle(pointC, pointB, pointA);
				
				pointA = position2 + fs::Float2(+halfSize._x, -halfMiddleHeight);
				pointB = position2 + fs::Float2(+halfSize._x, +halfMiddleHeight);
				drawSolidTriangle(pointA, pointB, pointC);
			}

			// Right bottom corner
			{
				pointA = position2 + fs::Float2(+halfSize._x, +halfMiddleHeight);
				pointB = position2 + fs::Float2(+halfMiddleWidth, +halfSize._y);
				drawQuadraticBezier(pointA, pointB, position2 + fs::Float2(+halfSize._x, +halfSize._y), false);
				drawSolidTriangle(pointA, pointB, position2 + fs::Float2(+halfMiddleWidth, +halfMiddleHeight));
			}
#endif
		}

		void ShapeRenderer::transformVertices(const uint32 vertexOffset, const uint32 targetVertexCount, const float rotationAngle)
		{
			const fs::Float4x4& rotationMatrix = fs::Float4x4::rotationMatrixZ(-rotationAngle);

			auto& vertexArray = _shapeBufferFast.vertexArray();
			for (uint32 vertexIter = 0; vertexIter < targetVertexCount; ++vertexIter)
			{
				CppHlsl::VS_INPUT_SHAPE_FAST& vertex = vertexArray[vertexOffset + vertexIter];
				vertex._position = rotationMatrix.mul(vertex._position);

				vertex._position._x += _position._x;
				vertex._position._y += _position._y;
			}
		}

		void ShapeRenderer::drawRectangle(const fs::Int2& size, const uint32 borderThickness, const float angle)
		{
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._borderColor = _borderColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._z = angle;
			v._infoB._w = 0.0f;
			v._infoC._x = static_cast<float>(borderThickness);

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::drawRoundedRectangle(const fs::Int2& size, const float roundness, const uint32 borderThickness, const float angle)
		{
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;
			
			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._borderColor = _borderColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = roundness;
			v._infoB._z = angle;
			v._infoB._w = 1.0f;
			v._infoC._x = static_cast<float>(borderThickness);

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y) * 1.5f));
			prepareIndexArray();
		}

		void ShapeRenderer::drawTaperedRectangle(const fs::Int2& size, const float tapering, const float bias, const uint32 borderThickness, const float angle)
		{
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(size);
			const fs::Float2 halfSizeF = sizeF * 0.5f;

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._borderColor = _borderColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (sizeF._x / windowSizeF._x) * 2.0f;
			v._infoA._w = (sizeF._y / windowSizeF._y) * 2.0f;
			v._infoB._x = tapering;
			v._infoB._y = bias;
			v._infoB._z = angle;
			v._infoB._w = 2.0f;
			v._infoC._x = static_cast<float>(borderThickness);

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

		void ShapeRenderer::drawCircularArc(const float radius, const float arcAngleA, const float arcAngleB, const float innerRadius)
		{
			const fs::Float2 centerPositionF = fs::Float2(_position._x, _position._y);
			const fs::Float2 windowSizeF = fs::Float2(_graphicDevice->getWindowSize());
			const fs::Float2 normalizedCenterPosition = normalizePosition(centerPositionF, windowSizeF);
			const fs::Float2 sizeF = fs::Float2(radius * 2.0f);
			const fs::Float2 halfSizeF = sizeF * 0.5f;

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._borderColor = _borderColor;
			v._infoA._x = normalizedCenterPosition._x;
			v._infoA._y = normalizedCenterPosition._y;
			v._infoA._z = (radius / windowSizeF._x) * 2.0f;
			v._infoA._w = arcAngleA;
			v._infoB._x = arcAngleB;
			v._infoB._y = (innerRadius / windowSizeF._x) * 2.0f;
			v._infoB._w = 4.0f;

			prepareVertexArray(v, centerPositionF, fs::Float2(fs::max(halfSizeF._x, halfSizeF._y)));
			prepareIndexArray();
		}

		void ShapeRenderer::drawColorPallete(const float radius)
		{
			static constexpr uint32 colorCount = 12;
			static const fs::Float4 colorArray[colorCount] = {
				// Red => Green
				fs::Float4(1.0f, 0.0f, 0.0f, 1.0f),
				fs::Float4(1.0f, 0.25f, 0.0f, 1.0f),
				fs::Float4(1.0f, 0.5f, 0.0f, 1.0f),
				fs::Float4(1.0f, 0.75f, 0.0f, 1.0f),
				fs::Float4(1.0f, 1.0f, 0.0f, 1.0f),
				fs::Float4(0.5f, 1.0f, 0.0f, 1.0f),

				// Gren => Blue
				fs::Float4(0.0f, 0.875f, 0.125f, 1.0f),
				fs::Float4(0.0f, 0.666f, 1.0f, 1.0f),
				fs::Float4(0.0f, 0.333f, 1.0f, 1.0f),
				fs::Float4(0.0f, 0.0f, 1.0f, 1.0f),

				// Blue => Red
				fs::Float4(0.5f, 0.0f, 1.0f, 1.0f),
				fs::Float4(1.0f, 0.0f, 0.5f, 1.0f),
			};

			static constexpr uint32 outerStepSmoothingOffset = 4;
			static constexpr uint32 innerStepSmoothingOffset = 0;
			const uint32 outerStepCount = 5;
			const uint32 innerStepCount = 4;
			const float stepHeight = radius / (innerStepCount + outerStepCount);
			
			const float deltaAngle = fs::Math::kTwoPi / colorCount;
			const float halfDeltaAngle = deltaAngle * 0.5f;
			for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
			{
				const float rgbDenom = (colorCount / 3.0f);
				const uint32 rgb = static_cast<uint32>(colorIndex / rgbDenom);
				
				int32 colorIndexCorrected = colorIndex - colorCount / 2;
				if (colorIndexCorrected < 0)
				{
					colorIndexCorrected = colorCount + colorIndexCorrected;
				}
				colorIndexCorrected = colorCount - colorIndexCorrected;
				if (colorIndexCorrected == colorCount)
				{
					colorIndexCorrected = 0;
				}
				const fs::Float4& stepsColor = colorArray[colorIndexCorrected];

				const float angleA = -(fs::Math::kPi / colorCount) + deltaAngle * colorIndex;
				const float angleB = angleA + deltaAngle;

				// Outer steps
				for (uint32 outerStepIndex = 0; outerStepIndex < outerStepCount; ++outerStepIndex)
				{
					const float outerStepRatio = 1.0f - static_cast<float>(outerStepIndex) / (outerStepCount + outerStepSmoothingOffset);
					setColor(stepsColor * outerStepRatio + fs::Float4(0.0f, 0.0f, 0.0f, 1.0f));

					drawCircularArc(stepHeight * (innerStepCount + outerStepIndex + 1) + 1.0f, angleA, angleB, stepHeight * (innerStepCount + outerStepIndex));
				}

				// Inner steps
				const fs::Float4 deltaColor = fs::Float4(1.0f, 1.0f, 1.0f, 0.0f) / (innerStepCount + innerStepSmoothingOffset);
				for (uint32 innerStepIndex = 0; innerStepIndex < innerStepCount; ++innerStepIndex)
				{
					setColor(stepsColor + deltaColor * static_cast<float>(innerStepCount - innerStepIndex));

					drawCircularArc(stepHeight * (innerStepIndex + 1) + 1.0f, angleA, angleB, stepHeight * innerStepIndex);
				}
			}
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
