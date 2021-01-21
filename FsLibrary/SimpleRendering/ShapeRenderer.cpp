#include <stdafx.h>
#include <FsLibrary/SimpleRendering/ShapeRenderer.h>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>
#include <FsLibrary/SimpleRendering/TriangleRenderer.hpp>


namespace fs
{
	namespace SimpleRendering
	{
		ShapeRenderer::ShapeRenderer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: IRenderer(graphicDevice)
			, _triangleRenderer{ graphicDevice }
			, _borderColor{ fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f) }
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
					#include <ShaderStructuredBufferDefinitions>
					
					StructuredBuffer<SB_Transform> sbTransform : register(t0);
					
					VS_OUTPUT_SHAPE_FAST main_shape(VS_INPUT_SHAPE_FAST input)
					{
						const uint shapeIndex = (uint)input._position.w;
						float4 transformedPosition = float4(input._position.xy, 0.0, 1.0);
						transformedPosition = mul(transformedPosition, sbTransform[shapeIndex]._transformMatrix);
						
						VS_OUTPUT_SHAPE_FAST result;
						result._position	= float4(mul(transformedPosition, _cbProjectionMatrix).xy, 0.0, 1.0);
						result._color		= input._color;
						result._texCoord	= input._texCoord;
						result._info.xy		= input._position.zw;
						
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT_SHAPE_FAST));
				_vertexShaderId = shaderPool.pushVertexShader("ShapeRendererVSFast", kShaderString, "main_shape", &typeInfo);
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
						const float scale = input._texCoord.w;
						
						float signedDistance = 0.0;
						if (1.0 == input._info.x)
						{
							// Solid
							return input._color;
						}
						else if (2.0 == input._info.x)
						{
							// Circular section
							signedDistance = input._texCoord.z * (1.0 -  sqrt(u * u + v * v));
						}
						else
						{
							// Quadratic Bezier
							signedDistance = -(u * u - v);
						}
						
						if (0.0 < scale)
						{
							signedDistance *= (scale * kDeltaPixel);
						}
						clip(signedDistance + kDeltaPixel);
						const float alpha = (0.0 < signedDistance) ? 1.0 : 1.0 - saturate(-signedDistance / kDeltaPixel);
						return float4(input._color.xyz, input._color.w * alpha);
					}
					)"
				};
				_pixelShaderId = shaderPool.pushNonVertexShader("ShapeRendererPSFast", kShaderString, "main_shape", DxShaderType::PixelShader);
			}
		}

		void ShapeRenderer::flushData() noexcept
		{
			_triangleRenderer.flush();

			flushShapeTransform();
		}

		void ShapeRenderer::render() noexcept
		{
			if (_triangleRenderer.isRenderable() == true)
			{
				prepareStructuredBuffer();

				fs::SimpleRendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShaderId);
				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShaderId);

				fs::SimpleRendering::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
				resourcePool.bindToShader(_sbTransformBufferId, DxShaderType::VertexShader, 0);

				_triangleRenderer.render();
			}
		}

		void ShapeRenderer::setBorderColor(const fs::SimpleRendering::Color& borderColor) noexcept
		{
			_borderColor = borderColor;
		}

		void ShapeRenderer::drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate)
		{
			drawQuadraticBezierInternal(pointA, pointB, controlPoint, _defaultColor, validate);

			pushShapeTransform(0.0f);
		}

		void ShapeRenderer::drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const fs::SimpleRendering::Color& color, const bool validate)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const fs::Float2(&pointArray)[2] = { pointA, pointB };
			auto& vertexArray = _triangleRenderer.vertexArray();

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
			v._color = color;
			v._position._x = pointArray[0 ^ flip]._x;
			v._position._y = pointArray[0 ^ flip]._y;
			v._position._w = getShapeTransformIndexAsFloat();
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			v._texCoord._w = abs(pointA._x - pointB._x);
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

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC)
		{
			drawSolidTriangleInternal(pointA, pointB, pointC, _defaultColor);

			pushShapeTransform(0.0f);
		}

		void ShapeRenderer::drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC, const fs::SimpleRendering::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			auto& vertexArray = _triangleRenderer.vertexArray();
			{
				v._color = color;
				v._position._x = pointA._x;
				v._position._y = pointA._y;
				v._position._z = kInfoSolid;
				v._position._w = getShapeTransformIndexAsFloat();
				vertexArray.emplace_back(v);

				v._position._x = pointB._x;
				v._position._y = pointB._y;
				vertexArray.emplace_back(v);

				v._position._x = pointC._x;
				v._position._y = pointC._y;
				vertexArray.emplace_back(v);
			}
			
			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _triangleRenderer.indexArray();
			{
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);
			}
		}

		void ShapeRenderer::drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const float halfRadius = radius * 0.5f;

			auto& vertexArray = _triangleRenderer.vertexArray();
			
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -halfRadius;
			v._position._y = -halfRadius;
			v._position._z = kInfoCircular;
			v._position._w = getShapeTransformIndexAsFloat();
			v._texCoord._x = 0.0f;
			v._texCoord._y = 1.0f;
			v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
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

			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
			
			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawQuarterCircle(const float radius, const float rotationAngle)
		{
			const float halfRadius = radius * 0.5f;
			
			drawQuarterCircleInternal(fs::Float2::kZero, halfRadius, _defaultColor);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawQuarterCircleInternal(const fs::Float2& offset, const float halfRadius, const fs::SimpleRendering::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 4;

			auto& vertexArray = _triangleRenderer.vertexArray();
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			{
				v._color = color;
				v._position._x = offset._x - halfRadius;
				v._position._y = offset._y - halfRadius;
				v._position._z = kInfoCircular;
				v._position._w = getShapeTransformIndexAsFloat();
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = 1.0f;
				v._texCoord._w = halfRadius * 2.0f;
				vertexArray.emplace_back(v);

				v._position._x = offset._x + halfRadius;
				v._texCoord._x = 1.0f;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._y = offset._y + halfRadius;
				v._texCoord._x = 1.0f;
				v._texCoord._y = 0.0f;
				vertexArray.emplace_back(v);

				v._position._x = offset._x - halfRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - 4;
			auto& indexArray = _triangleRenderer.indexArray();
			{
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);

				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 2);
				indexArray.push_back(vertexOffset + 3);
			}
		}

		void ShapeRenderer::drawHalfCircle(const float radius, const float rotationAngle, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const float scaledRadius = fs::Math::kSqrtOfTwo * radius;

			auto& vertexArray = _triangleRenderer.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = -scaledRadius;
			v._position._z = kInfoCircular;
			v._position._w = getShapeTransformIndexAsFloat();
			v._texCoord._x = -fs::Math::kSqrtOfTwo;
			v._texCoord._y = 0.0f;
			v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
			vertexArray.emplace_back(v);

			v._position._x = 0.0f;
			v._position._y = -scaledRadius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = +fs::Math::kSqrtOfTwo;
			vertexArray.emplace_back(v);

			v._position._x = +scaledRadius;
			v._position._y = 0.0f;
			v._texCoord._x = +fs::Math::kSqrtOfTwo;
			v._texCoord._y = 0.0f;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawCircularArc(const float radius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 6;
			const float halfArcAngle = fs::Math::clamp(arcAngle, 0.0f, fs::Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);

			auto& vertexArray = _triangleRenderer.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			
			// Right arc section
			{
				v._color = _defaultColor;
				v._position._x = 0.0f;
				v._position._y = -radius;
				v._position._z = kInfoCircular;
				v._position._w = getShapeTransformIndexAsFloat();
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = 1.0f;
				v._texCoord._w = radius;
				vertexArray.emplace_back(v);

				v._position._x = +radius * sinHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._y = -radius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}
			
			// Left arc section
			{
				v._position._x = -radius * sinHalfArcAngle;
				v._position._y = -radius;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._y = -radius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}

			// Center
			{
				v._position._x = 0.0f;
				v._position._y = 0.0f;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 4);

			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 2);
			indexArray.push_back(vertexOffset + 5);

			indexArray.push_back(vertexOffset + 4);
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 5);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 13;
			const float halfArcAngle = fs::Math::clamp(arcAngle, 0.0f, fs::Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);
			const float tanHalfArcAngle = tan(halfArcAngle);

			auto& vertexArray = _triangleRenderer.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;

			// Right outer arc section
			{
				v._color = _defaultColor;
				v._position._x = 0.0f;
				v._position._y = -outerRadius;
				v._position._z = kInfoCircular;
				v._position._w = getShapeTransformIndexAsFloat();
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = +1.0f; // @IMPORTANT
				v._texCoord._w = outerRadius;
				vertexArray.emplace_back(v);

				v._position._x = +outerRadius * tanHalfArcAngle;
				v._texCoord._x = +tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._x = +outerRadius * sinHalfArcAngle;
				v._position._y = -outerRadius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}

			// Left outer arc section
			{
				v._position._x = -outerRadius * tanHalfArcAngle;
				v._position._y = -outerRadius;
				v._texCoord._x = -tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._x = -outerRadius * sinHalfArcAngle;
				v._position._y = -outerRadius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}

			const float innerRatio = innerRadius / outerRadius;
			// Middle
			{
				v._position._x = 0.0f;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = innerRatio;
				vertexArray.emplace_back(v);
			}

			// Right inner arc section
			{
				v._position._x = 0.0f;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = -1.0f; // @IMPORTANT
				vertexArray.emplace_back(v);

				v._position._x = +innerRadius * tanHalfArcAngle;
				v._texCoord._x = +tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._x = +innerRadius * sinHalfArcAngle;
				v._position._y = -innerRadius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}

			// Left inner arc section
			{
				v._position._x = -innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				v._texCoord._x = -tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.emplace_back(v);

				v._position._x = -innerRadius * sinHalfArcAngle;
				v._position._y = -innerRadius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.emplace_back(v);
			}

			// Right side
			{
				v._position._x = +innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				v._texCoord._z = +1.0f; // @IMPORTANT
				vertexArray.emplace_back(v);
			}

			// Left side
			{
				v._position._x = -innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _triangleRenderer.indexArray();
			
			// Right outer arc section
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			// Left outer arc section
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 4);

			// Middle-right
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 2);
			indexArray.push_back(vertexOffset + 5);

			// Middle-left
			indexArray.push_back(vertexOffset + 4);
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 5);

			// Right inner arc section
			indexArray.push_back(vertexOffset + 6);
			indexArray.push_back(vertexOffset + 7);
			indexArray.push_back(vertexOffset + 8);

			// Left inner arc section
			indexArray.push_back(vertexOffset + 9);
			indexArray.push_back(vertexOffset + 6);
			indexArray.push_back(vertexOffset + 10);

			// Right side
			indexArray.push_back(vertexOffset + 5);
			indexArray.push_back(vertexOffset + 2);
			indexArray.push_back(vertexOffset + 11);

			// Left side
			indexArray.push_back(vertexOffset + 4);
			indexArray.push_back(vertexOffset + 5);
			indexArray.push_back(vertexOffset + 12);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawRectangle(const fs::Float2& size, const float borderThickness, const float rotationAngle)
		{
			const fs::Float2 halfSize = size * 0.5f;
			
			if (1.0f <= borderThickness)
			{
				drawRectangleInternal(fs::Float2(0.0f, -halfSize._y - borderThickness * 0.5f), fs::Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _borderColor);

				drawRectangleInternal(fs::Float2(0.0f, +halfSize._y + borderThickness * 0.5f), fs::Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _borderColor);

				drawRectangleInternal(fs::Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), fs::Float2(borderThickness * 0.5f, halfSize._y), _borderColor);

				drawRectangleInternal(fs::Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), fs::Float2(borderThickness * 0.5f, halfSize._y), _borderColor);
			}

			drawRectangleInternal(fs::Float2::kZero, halfSize, _defaultColor);
			
			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawRectangleInternal(const fs::Float2& offset, const fs::Float2& halfSize, const fs::SimpleRendering::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 4;

			auto& vertexArray = _triangleRenderer.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			{
				v._color = color;
				v._position._x = offset._x - halfSize._x;
				v._position._y = offset._y - halfSize._y;
				v._position._z = kInfoSolid;
				v._position._w = getShapeTransformIndexAsFloat();
				vertexArray.emplace_back(v);

				v._position._x = offset._x + halfSize._x;
				v._position._y = offset._y - halfSize._y;
				vertexArray.emplace_back(v);

				v._position._x = offset._x - halfSize._x;
				v._position._y = offset._y + halfSize._y;
				vertexArray.emplace_back(v);

				v._position._x = offset._x + halfSize._x;
				v._position._y = offset._y + halfSize._y;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _triangleRenderer.indexArray();

			// Body left upper
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			// Body right lower
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRenderer::drawTaperedRectangle(const fs::Float2& size, const float tapering, const float bias, const float rotationAngle)
		{
			const fs::Float2 halfSize = size * 0.5f;
			const float horizontalSpace = size._x * (1.0f - tapering);
			static constexpr uint32 kDeltaVertexCount = 4;
			const float horizontalOffsetL = horizontalSpace * bias;
			const float horizontalOffsetR = horizontalSpace * (1.0f - bias);

			auto& vertexArray = _triangleRenderer.vertexArray();
			CppHlsl::VS_INPUT_SHAPE_FAST v;
			{
				v._color = _defaultColor;
				v._position._x = -halfSize._x + horizontalOffsetL;
				v._position._y = -halfSize._y;
				v._position._z = kInfoSolid;
				v._position._w = getShapeTransformIndexAsFloat();
				vertexArray.emplace_back(v);

				v._position._x = +halfSize._x - horizontalOffsetR;
				v._position._y = -halfSize._y;
				vertexArray.emplace_back(v);

				v._position._x = -halfSize._x;
				v._position._y = +halfSize._y;
				vertexArray.emplace_back(v);

				v._position._x = +halfSize._x;
				v._position._y = +halfSize._y;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _triangleRenderer.indexArray();
			{
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);

				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 3);
				indexArray.push_back(vertexOffset + 2);
			}

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawRoundedRectangle(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle)
		{
			const float radius = fs::min(size._x, size._y) * fs::Math::saturate(roundness);
			const fs::Float2& halfSize = size * 0.5f;
			const fs::Float2& halfCoreSize = halfSize - fs::Float2(radius);

			if (1.0f <= borderThickness)
			{
				fs::Float2 pointA;
				fs::Float2 pointB;
				fs::Float2 pointC;
				
				// Left top
				{
					pointA = fs::Float2(-halfSize._x - borderThickness, -halfCoreSize._y);
					pointB = fs::Float2(-halfCoreSize._x, -halfSize._y - borderThickness);
					drawQuadraticBezierInternal(pointA, pointB, -halfSize - fs::Float2(borderThickness), _borderColor);

					pointC = fs::Float2(-halfCoreSize._x, -halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
				}
				
				// Right top
				{
					pointA = fs::Float2(+halfCoreSize._x, -halfSize._y - borderThickness);
					pointB = fs::Float2(+halfSize._x + borderThickness, -halfCoreSize._y);
					drawQuadraticBezierInternal(pointA, pointB, fs::Float2(+halfSize._x, -halfSize._y) + fs::Float2(+borderThickness, -borderThickness), _borderColor);

					pointC = fs::Float2(+halfCoreSize._x, -halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
				}

				// Left bottom
				{
					pointA = fs::Float2(-halfCoreSize._x, +halfSize._y + borderThickness);
					pointB = fs::Float2(-halfSize._x - borderThickness, +halfCoreSize._y);
					drawQuadraticBezierInternal(pointA, pointB, fs::Float2(-halfSize._x, +halfSize._y) + fs::Float2(-borderThickness, +borderThickness), _borderColor);

					pointC = fs::Float2(-halfCoreSize._x, +halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
				}

				// Right bottom
				{
					pointA = fs::Float2(+halfSize._x + borderThickness, +halfCoreSize._y);
					pointB = fs::Float2(+halfCoreSize._x, +halfSize._y + borderThickness);
					drawQuadraticBezierInternal(pointA, pointB, halfSize + fs::Float2(borderThickness), _borderColor);

					pointC = fs::Float2(+halfCoreSize._x, +halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
				}

				// Top
				drawRectangleInternal(fs::Float2(0.0f, -halfSize._y - borderThickness * 0.5f), fs::Float2(halfCoreSize._x, borderThickness * 0.5f), _borderColor);

				// Bottom
				drawRectangleInternal(fs::Float2(0.0f, +halfSize._y + borderThickness * 0.5f), fs::Float2(halfCoreSize._x, borderThickness * 0.5f), _borderColor);

				// Left
				drawRectangleInternal(fs::Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), fs::Float2(borderThickness * 0.5f, halfCoreSize._y), _borderColor);

				// Right
				drawRectangleInternal(fs::Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), fs::Float2(borderThickness * 0.5f, halfCoreSize._y), _borderColor);
			}

			drawRoundedRectangleInternal(radius, halfSize, roundness, _defaultColor);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRenderer::drawRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::SimpleRendering::Color& color)
		{
			const fs::Float2& halfCoreSize = halfSize - fs::Float2(radius);

			fs::Float2 pointA;
			fs::Float2 pointB;
			fs::Float2 pointC;

			// Center box
			{
				pointA = fs::Float2(-halfCoreSize._x, -halfSize._y);
				pointB = fs::Float2(+halfCoreSize._x, -halfSize._y);
				pointC = fs::Float2(-halfCoreSize._x, +halfSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);

				pointA = fs::Float2(+halfCoreSize._x, +halfSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);
			}

			// Left top corner
			{
				pointA = fs::Float2(-halfSize._x, -halfCoreSize._y);
				pointB = fs::Float2(-halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, -halfSize, color, false);
			}

			// Left side box
			{
				pointC = fs::Float2(-halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);

				pointA = fs::Float2(-halfSize._x, -halfCoreSize._y);
				pointB = fs::Float2(-halfSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);
			}

			// Left bottom corner
			{
				pointA = fs::Float2(-halfSize._x, +halfCoreSize._y);
				pointB = fs::Float2(-halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, fs::Float2(-halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointB, pointA, fs::Float2(-halfCoreSize._x, +halfCoreSize._y), color);
			}

			// Right top corner
			{
				pointA = fs::Float2(+halfSize._x, -halfCoreSize._y);
				pointB = fs::Float2(+halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, fs::Float2(+halfSize._x, -halfSize._y), color, false);
			}

			// Right side box
			{
				pointC = fs::Float2(+halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);

				pointA = fs::Float2(+halfSize._x, -halfCoreSize._y);
				pointB = fs::Float2(+halfSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);
			}

			// Right bottom corner
			{
				pointA = fs::Float2(+halfSize._x, +halfCoreSize._y);
				pointB = fs::Float2(+halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, fs::Float2(+halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointA, pointB, fs::Float2(+halfCoreSize._x, +halfCoreSize._y), color);
			}
		}

		void ShapeRenderer::drawLine(const fs::Float2& p0, const fs::Float2& p1, const float thickness)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const fs::Float2& dir = fs::Float2::normalize(p1 - p0);
			const fs::Float2& normal = fs::Float2(-dir._y, dir._x);
			const float halfThickness = thickness * 0.5f;

			const fs::Float2 v0 = p0 - normal * halfThickness;
			const fs::Float2 v1 = p1 - normal * halfThickness;
			const fs::Float2 v2 = p0 + normal * halfThickness;
			const fs::Float2 v3 = p1 + normal * halfThickness;

			auto& vertexArray = _triangleRenderer.vertexArray();

			CppHlsl::VS_INPUT_SHAPE_FAST v;
			v._color = _defaultColor;
			v._position._x = v0._x;
			v._position._y = v0._y;
			v._position._z = kInfoSolid;
			v._position._w = getShapeTransformIndexAsFloat();
			vertexArray.emplace_back(v);

			v._position._x = v1._x;
			v._position._y = v1._y;
			vertexArray.emplace_back(v);

			v._position._x = v2._x;
			v._position._y = v2._y;
			vertexArray.emplace_back(v);

			v._position._x = v3._x;
			v._position._y = v3._y;
			vertexArray.emplace_back(v);

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _triangleRenderer.indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);

			pushShapeTransform(0.0f);
		}

		void ShapeRenderer::flushShapeTransform()
		{
			_sbTransformData.clear();
		}

		const float ShapeRenderer::getShapeTransformIndexAsFloat() const noexcept
		{
			return static_cast<float>(_sbTransformData.size());
		}

		void ShapeRenderer::pushShapeTransform(const float rotationAngle, const bool applyInternalPosition)
		{
			fs::CppHlsl::SB_Transform transform;
			transform._transformMatrix = fs::Float4x4::rotationMatrixZ(-rotationAngle);
			transform._transformMatrix._m[0][3] = (applyInternalPosition == true) ? _position._x : 0.0f;
			transform._transformMatrix._m[1][3] = (applyInternalPosition == true) ? _position._y : 0.0f;
			_sbTransformData.emplace_back(transform);
		}

		void ShapeRenderer::prepareStructuredBuffer()
		{
			fs::SimpleRendering::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();

			const uint32 elementCount = static_cast<uint32>(_sbTransformData.size());
			if (_sbTransformBufferId.isValid() == false && 0 < elementCount)
			{
				_sbTransformBufferId = resourcePool.pushStructuredBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), sizeof(_sbTransformData[0]), elementCount);
			}
			
			if (_sbTransformBufferId.isValid() == true)
			{
				fs::SimpleRendering::DxResource& structuredBuffer = resourcePool.getResource(_sbTransformBufferId);
				structuredBuffer.updateBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), elementCount);
			}
		}

		void ShapeRenderer::drawColorPallete(const float radius)
		{
			static constexpr uint32 colorCount = 12;
			static const fs::SimpleRendering::Color colorArray[colorCount] = {
				// Red => Green
				fs::SimpleRendering::Color(1.0f, 0.0f, 0.0f, 1.0f),
				fs::SimpleRendering::Color(1.0f, 0.25f, 0.0f, 1.0f),
				fs::SimpleRendering::Color(1.0f, 0.5f, 0.0f, 1.0f),
				fs::SimpleRendering::Color(1.0f, 0.75f, 0.0f, 1.0f),
				fs::SimpleRendering::Color(1.0f, 1.0f, 0.0f, 1.0f),
				fs::SimpleRendering::Color(0.5f, 1.0f, 0.0f, 1.0f),

				// Gren => Blue
				fs::SimpleRendering::Color(0.0f, 0.875f, 0.125f, 1.0f),
				fs::SimpleRendering::Color(0.0f, 0.666f, 1.0f, 1.0f),
				fs::SimpleRendering::Color(0.0f, 0.333f, 1.0f, 1.0f),
				fs::SimpleRendering::Color(0.0f, 0.0f, 1.0f, 1.0f),

				// Blue => Red
				fs::SimpleRendering::Color(0.5f, 0.0f, 1.0f, 1.0f),
				fs::SimpleRendering::Color(1.0f, 0.0f, 0.5f, 1.0f),
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
				
				int32 colorIndexCorrected = colorIndex;
				const fs::SimpleRendering::Color& stepsColor = colorArray[colorIndexCorrected];

				// Outer steps
				for (uint32 outerStepIndex = 0; outerStepIndex < outerStepCount; ++outerStepIndex)
				{
					const float outerStepRatio = 1.0f - static_cast<float>(outerStepIndex) / (outerStepCount + outerStepSmoothingOffset);
					setColor(stepsColor * outerStepRatio + fs::SimpleRendering::Color(0.0f, 0.0f, 0.0f, 1.0f));

					drawDoubleCircularArc(stepHeight * (innerStepCount + outerStepIndex + 1) + 1.0f, stepHeight * (innerStepCount + outerStepIndex), deltaAngle, deltaAngle * colorIndex);
				}

				// Inner steps
				const fs::SimpleRendering::Color deltaColor = fs::SimpleRendering::Color(1.0f, 1.0f, 1.0f, 0.0f) / (innerStepCount + innerStepSmoothingOffset);
				for (uint32 innerStepIndex = 0; innerStepIndex < innerStepCount; ++innerStepIndex)
				{
					setColor(stepsColor + deltaColor * static_cast<float>(innerStepCount - innerStepIndex));
				
					drawDoubleCircularArc(stepHeight * (innerStepIndex + 1) + 1.0f, stepHeight * innerStepIndex, deltaAngle, deltaAngle * colorIndex);
				}
			}
		}
	}
}
