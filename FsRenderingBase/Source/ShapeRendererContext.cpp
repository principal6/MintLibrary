#include <stdafx.h>
#include <FsRenderingBase/Include/ShapeRendererContext.h>

#include <FsRenderingBase/Include/GraphicDevice.h>
#include <FsRenderingBase/Include/TriangleRenderer.hpp>

#include <FsMath/Include/Float2x2.h>
#include <FsMath/Include/Float3x3.h>


namespace fs
{
	namespace RenderingBase
	{
		ShapeRendererContext::ShapeRendererContext(fs::RenderingBase::GraphicDevice* const graphicDevice)
			: IRendererContext(graphicDevice)
			, _triangleRenderer{ nullptr }
			, _borderColor{ fs::RenderingBase::Color(1.0f, 1.0f, 1.0f) }
		{
			_triangleRenderer = FS_NEW(RenderingBase::TriangleRenderer<CppHlsl::VS_INPUT_SHAPE>, graphicDevice);
		}

		ShapeRendererContext::~ShapeRendererContext()
		{
			FS_DELETE(_triangleRenderer);
		}

		void ShapeRendererContext::initializeShaders() noexcept
		{
			fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					#include <ShaderStructuredBufferDefinitions>
					
					StructuredBuffer<SB_Transform> sbTransform : register(t0);
					
					VS_OUTPUT_SHAPE main_shape(VS_INPUT_SHAPE input)
					{
						const uint shapeInfo = asuint(input._info.y);
						const uint shapeType = (shapeInfo >> 30) & 3;
						const uint shapeIndex = shapeInfo & 0x3FFFFFFF;
						
						float4 transformedPosition = float4(input._position.xyz, 1.0);
						transformedPosition = mul(transformedPosition, sbTransform[shapeIndex]._transformMatrix);
						
						VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
						result._position		= float4(mul(transformedPosition, _cbProjectionMatrix).xyz, 1.0);
						result._color			= input._color;
						result._texCoord		= input._texCoord;
						result._info.x			= (float)shapeType;
						result._viewportIndex	= (uint)input._info.x;
						
						return result;
					}
					)"
				};
				const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslStructs().getTypeInfo(typeid(fs::CppHlsl::VS_INPUT_SHAPE));
				_vertexShaderId = shaderPool.pushVertexShader("ShapeRendererVS", kShaderString, "main_shape", &typeInfo);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					
					[maxvertexcount(3)]
					void main_shape(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
					{
						for (int i = 0; i < 3; ++i)
						{
							OutputStream.Append(input[i]);
						}
						OutputStream.RestartStrip();
					}
					)"
				};
				_geometryShaderId = shaderPool.pushNonVertexShader("ShapeRendererGS", kShaderString, "main_shape", DxShaderType::GeometryShader);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>

					static const float kDeltaDoublePixel = _cbProjectionMatrix[0][0];
					static const float kDeltaPixel = kDeltaDoublePixel * 0.5;
					static const float kDeltaHalfPixel = kDeltaPixel * 0.5;
					
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float u = input._texCoord.x;
						const float v = input._texCoord.y;
						const float scale = input._texCoord.w;
						const float flipped = input._texCoord.z;
						
						float signedDistance = 0.0;
						if (1.0 == input._info.x)
						{
							// Solid triangle
							return input._color;
						}
						else if (2.0 == input._info.x)
						{
							// Circular section
							signedDistance = flipped * (1.0 -  sqrt(u * u + v * v));
						}
						else if (3.0 == input._info.x)
						{
							// Circle
							signedDistance = flipped * (1.0 - sqrt(input._texCoord.x * input._texCoord.x + input._texCoord.y * input._texCoord.y));
						}
						else
						{
							// Quadratic Bezier
							signedDistance = -(u * u - v);
						}
						
						// Apply scale to the signed distance for more consistent anti-aliasing
						if (0.0 < scale)
						{
							signedDistance *= (scale * kDeltaPixel);
						}
						
						const float alpha = (kDeltaHalfPixel < signedDistance) ? 1.0 : 1.0 - saturate(abs(signedDistance - kDeltaHalfPixel) / kDeltaPixel);
						return float4(input._color.xyz, input._color.w * alpha);
					}
					)"
				};
				_pixelShaderId = shaderPool.pushNonVertexShader("ShapeRendererPS", kShaderString, "main_shape", DxShaderType::PixelShader);
			}
		}

		void ShapeRendererContext::flushData() noexcept
		{
			_triangleRenderer->flush();

			flushShapeTransform();
		}

		const bool ShapeRendererContext::hasData() const noexcept
		{
			return _triangleRenderer->isRenderable();
		}

		void ShapeRendererContext::render() noexcept
		{
			if (_triangleRenderer->isRenderable() == true)
			{
				prepareStructuredBuffer();

				fs::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
				shaderPool.bindShader(DxShaderType::VertexShader, _vertexShaderId);

				if (getUseMultipleViewports() == true)
				{
					shaderPool.bindShader(DxShaderType::GeometryShader, _geometryShaderId);
				}

				shaderPool.bindShader(DxShaderType::PixelShader, _pixelShaderId);

				fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
				resourcePool.bindToShader(_sbTransformBufferId, DxShaderType::VertexShader, 0);

				_triangleRenderer->render();

				if (getUseMultipleViewports() == true)
				{
					shaderPool.unbindShader(DxShaderType::GeometryShader);
				}
			}
		}

		void ShapeRendererContext::setBorderColor(const fs::RenderingBase::Color& borderColor) noexcept
		{
			_borderColor = borderColor;
		}

		void ShapeRendererContext::drawQuadraticBezier(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const bool validate)
		{
			drawQuadraticBezierInternal(pointA, pointB, controlPoint, _defaultColor, validate);

			pushShapeTransform(0.0f, false);
		}

		void ShapeRendererContext::drawQuadraticBezierInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& controlPoint, const fs::RenderingBase::Color& color, const bool validate)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const fs::Float2(&pointArray)[2] = { pointA, pointB };
			auto& vertexArray = _triangleRenderer->vertexArray();

			uint8 flip = 0;
			if (validate == true)
			{
				// The control point must be on the left side of the AB segment!
				const fs::Float3 ac = fs::Float3(controlPoint - pointA);
				const fs::Float3 ab = fs::Float3(pointB - pointA);
				const fs::Float3& cross = fs::Float3::cross(ab, ac);
				flip = (cross._z > 0.0f) ? 1 : 0; // y 좌표계가 (아래가 + 방향으로) 뒤집혀 있어서 z 값 비교도 뒤집혔다.
			}

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = color;
			v._position = _position;
			v._position._x = pointArray[0 ^ flip]._x;
			v._position._y = pointArray[0 ^ flip]._y;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			v._texCoord._w = abs(pointA._x - pointB._x);
			v._info._x = _viewportIndex;
			v._info._y = getShapeInfoAsFloat(ShapeType::QuadraticBezierTriangle);
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
			auto& indexArray = _triangleRenderer->indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRendererContext::drawSolidTriangle(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC)
		{
			drawSolidTriangleInternal(pointA, pointB, pointC, _defaultColor);

			pushShapeTransform(0.0f, false);
		}

		void ShapeRendererContext::drawSolidTriangleInternal(const fs::Float2& pointA, const fs::Float2& pointB, const fs::Float2& pointC, const fs::RenderingBase::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			
			CppHlsl::VS_INPUT_SHAPE v;
			auto& vertexArray = _triangleRenderer->vertexArray();
			{
				v._color = color;
				v._position = _position;
				v._position._x = pointA._x;
				v._position._y = pointA._y;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::SolidTriangle);
				vertexArray.emplace_back(v);

				v._position._x = pointB._x;
				v._position._y = pointB._y;
				vertexArray.emplace_back(v);

				v._position._x = pointC._x;
				v._position._y = pointC._y;
				vertexArray.emplace_back(v);
			}
			
			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _triangleRenderer->indexArray();
			{
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);
			}
		}

		void ShapeRendererContext::drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const float halfRadius = radius * 0.5f;

			auto& vertexArray = _triangleRenderer->vertexArray();
			
			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._position = _position;
			v._position._x = -halfRadius;
			v._position._y = -halfRadius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 1.0f;
			v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
			v._info._x = _viewportIndex;
			v._info._y = getShapeInfoAsFloat(ShapeType::Circular);
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

			auto& indexArray = _triangleRenderer->indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);
			
			pushShapeTransform(rotationAngle);
		}

		void ShapeRendererContext::drawQuarterCircle(const float radius, const float rotationAngle)
		{
			const float halfRadius = radius * 0.5f;
			
			drawQuarterCircleInternal(fs::Float2::kZero, halfRadius, _defaultColor);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRendererContext::drawQuarterCircleInternal(const fs::Float2& offset, const float halfRadius, const fs::RenderingBase::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 4;

			auto& vertexArray = _triangleRenderer->vertexArray();
			CppHlsl::VS_INPUT_SHAPE v;
			{
				v._color = color;
				v._position = _position;
				v._position._x = offset._x - halfRadius;
				v._position._y = offset._y - halfRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = 1.0f;
				v._texCoord._w = halfRadius * 2.0f;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::Circular);
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
			auto& indexArray = _triangleRenderer->indexArray();
			{
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);

				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 2);
				indexArray.push_back(vertexOffset + 3);
			}
		}

		void ShapeRendererContext::drawHalfCircle(const float radius, const float rotationAngle)
		{
			const fs::Float4 originalPosition = _position;
			const float halfRadius = radius * 0.5f;

			const fs::Float4& offset = fs::Float4(+halfRadius, -halfRadius, 0.0f, 0.0f);
			const fs::Float4x4& rotationMatrixA = fs::Float4x4::rotationMatrixZ(-rotationAngle);
			const fs::Float4& rotatedOffsetA = rotationMatrixA.mul(offset);
			setPosition(originalPosition + rotatedOffsetA);
			drawQuarterCircle(radius, rotationAngle);

			const fs::Float4x4& rotationMatrixB = fs::Float4x4::rotationMatrixZ(-(rotationAngle + fs::Math::kPiOverTwo));
			const fs::Float4& rotatedOffsetB = rotationMatrixB.mul(offset);
			setPosition(originalPosition + rotatedOffsetB);
			drawQuarterCircle(radius, rotationAngle + fs::Math::kPiOverTwo);

			setPosition(originalPosition);
		}

		void ShapeRendererContext::drawCircle(const float radius, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 4;

			auto& vertexArray = _triangleRenderer->vertexArray();

			CppHlsl::VS_INPUT_SHAPE v;
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -radius;
				v._position._y = -radius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = +1.0f;
				v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
				v._texCoord._w = radius;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::Circle);
				vertexArray.emplace_back(v);

				v._position._x = +radius;
				v._position._y = -radius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = +1.0f;
				vertexArray.emplace_back(v);

				v._position._x = -radius;
				v._position._y = +radius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = -1.0f;
				vertexArray.emplace_back(v);

				v._position._x = +radius;
				v._position._y = +radius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = -1.0f;
				vertexArray.emplace_back(v);
			}

			const uint32 vertexOffset = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			
			auto& indexArray = _triangleRenderer->indexArray();
			{
				// Body left upper
				indexArray.push_back(vertexOffset + 0);
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 2);

				// Body right lower
				indexArray.push_back(vertexOffset + 1);
				indexArray.push_back(vertexOffset + 3);
				indexArray.push_back(vertexOffset + 2);
			}

			pushShapeTransform(0.0f);
		}

		void ShapeRendererContext::drawCircularArc(const float radius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 6;
			const float halfArcAngle = fs::Math::clamp(arcAngle, 0.0f, fs::Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);

			auto& vertexArray = _triangleRenderer->vertexArray();

			CppHlsl::VS_INPUT_SHAPE v;
			
			// Right arc section
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = 0.0f;
				v._position._y = -radius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = 1.0f;
				v._texCoord._w = radius;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::Circular);
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

			auto& indexArray = _triangleRenderer->indexArray();
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

		void ShapeRendererContext::drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 13;
			const float halfArcAngle = fs::Math::clamp(arcAngle, 0.0f, fs::Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);
			const float tanHalfArcAngle = tan(halfArcAngle);

			auto& vertexArray = _triangleRenderer->vertexArray();

			CppHlsl::VS_INPUT_SHAPE v;

			// Right outer arc section
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = 0.0f;
				v._position._y = -outerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = +1.0f; // @IMPORTANT
				v._texCoord._w = outerRadius;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::Circular);
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

			auto& indexArray = _triangleRenderer->indexArray();
			
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

		void ShapeRendererContext::drawRectangle(const fs::Float2& size, const float borderThickness, const float rotationAngle)
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

		void ShapeRendererContext::drawRectangleInternal(const fs::Float2& offset, const fs::Float2& halfSize, const fs::RenderingBase::Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 4;

			auto& vertexArray = _triangleRenderer->vertexArray();

			CppHlsl::VS_INPUT_SHAPE v;
			{
				v._color = color;
				v._position = _position;
				v._position._x = offset._x - halfSize._x;
				v._position._y = offset._y - halfSize._y;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::SolidTriangle);
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

			auto& indexArray = _triangleRenderer->indexArray();

			// Body left upper
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			// Body right lower
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);
		}

		void ShapeRendererContext::drawTaperedRectangle(const fs::Float2& size, const float tapering, const float bias, const float rotationAngle)
		{
			const fs::Float2 halfSize = size * 0.5f;
			const float horizontalSpace = size._x * (1.0f - tapering);
			static constexpr uint32 kDeltaVertexCount = 4;
			const float horizontalOffsetL = horizontalSpace * bias;
			const float horizontalOffsetR = horizontalSpace * (1.0f - bias);

			auto& vertexArray = _triangleRenderer->vertexArray();
			CppHlsl::VS_INPUT_SHAPE v;
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -halfSize._x + horizontalOffsetL;
				v._position._y = -halfSize._y;
				v._info._x = _viewportIndex;
				v._info._y = getShapeInfoAsFloat(ShapeType::SolidTriangle);
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
			auto& indexArray = _triangleRenderer->indexArray();
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

		void ShapeRendererContext::drawRoundedRectangle(const fs::Float2& size, const float roundness, const float borderThickness, const float rotationAngle)
		{
			const float clampedRoundness = fs::Math::saturate(roundness);
			if (clampedRoundness == 0.0f)
			{
				drawRectangle(size, borderThickness, rotationAngle);
				return;
			}

			const float radius = fs::min(size._x, size._y) * 0.5f * clampedRoundness;
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

			drawRoundedRectangleInternal(radius, halfSize, clampedRoundness, _defaultColor);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRendererContext::drawHalfRoundedRectangle(const fs::Float2& size, const float roundness, const float rotationAngle)
		{
			const float clampedRoundness = fs::Math::saturate(roundness);
			if (clampedRoundness == 0.0f)
			{
				drawRectangle(size, 0.0f, rotationAngle);
				return;
			}

			const float radius = fs::min(size._x, size._y) * 0.5f * clampedRoundness;
			const fs::Float2& halfSize = size * 0.5f;

			drawHalfRoundedRectangleInternal(radius, halfSize, clampedRoundness, _defaultColor);

			pushShapeTransform(rotationAngle);
		}

		void ShapeRendererContext::drawRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::RenderingBase::Color& color)
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

		void ShapeRendererContext::drawHalfRoundedRectangleInternal(const float radius, const fs::Float2& halfSize, const float roundness, const fs::RenderingBase::Color& color)
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

			// Left side box
			{
				pointA = fs::Float2(-halfSize._x, -halfSize._y);
				pointB = fs::Float2(-halfCoreSize._x, -halfSize._y);
				pointC = fs::Float2(-halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);

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

			// Right side box
			{
				pointA = fs::Float2(+halfSize._x, -halfSize._y);
				pointB = fs::Float2(+halfCoreSize._x, -halfSize._y);
				pointC = fs::Float2(+halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);

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

		void ShapeRendererContext::drawLine(const fs::Float2& p0, const fs::Float2& p1, const float thickness)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const fs::Float2& dir = fs::Float2::normalize(p1 - p0);
			const fs::Float2& normal = fs::Float2(-dir._y, dir._x);
			const float halfThickness = thickness * 0.5f;

			const fs::Float2 v0 = p0 - normal * halfThickness;
			const fs::Float2 v1 = p1 - normal * halfThickness;
			const fs::Float2 v2 = p0 + normal * halfThickness;
			const fs::Float2 v3 = p1 + normal * halfThickness;

			auto& vertexArray = _triangleRenderer->vertexArray();

			CppHlsl::VS_INPUT_SHAPE v;
			v._color = _defaultColor;
			v._position = _position;
			v._position._x = v0._x;
			v._position._y = v0._y;
			v._info._x = _viewportIndex;
			v._info._y = getShapeInfoAsFloat(ShapeType::SolidTriangle);
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
			auto& indexArray = _triangleRenderer->indexArray();
			indexArray.push_back(vertexOffset + 0);
			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 2);

			indexArray.push_back(vertexOffset + 1);
			indexArray.push_back(vertexOffset + 3);
			indexArray.push_back(vertexOffset + 2);

			pushShapeTransform(0.0f, false);
		}

		void ShapeRendererContext::flushShapeTransform()
		{
			_sbTransformData.clear();
		}

		const float ShapeRendererContext::getShapeInfoAsFloat(const ShapeType shapeType) const noexcept
		{
			union Conversion
			{
				float	_f;
				uint32	_ui;
			};

			// 상위 2bit 로 ShapeType 식별
			Conversion conversion;
			conversion._ui = (static_cast<uint32>(shapeType) << 30) | static_cast<uint32>(_sbTransformData.size());
			return conversion._f;
		}

		void ShapeRendererContext::pushShapeTransform(const float rotationAngle, const bool applyInternalPosition)
		{
			fs::CppHlsl::SB_Transform transform;
			transform._transformMatrix = fs::Float4x4::rotationMatrixZ(-rotationAngle);
			transform._transformMatrix._m[0][3] = (applyInternalPosition == true) ? _position._x : 0.0f;
			transform._transformMatrix._m[1][3] = (applyInternalPosition == true) ? _position._y : 0.0f;
			//transform._transformMatrix._m[2][3] = (applyInternalPosition == true) ? _position._z : 0.0f;
			_sbTransformData.emplace_back(transform);
		}

		void ShapeRendererContext::prepareStructuredBuffer()
		{
			fs::RenderingBase::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();

			const uint32 elementCount = static_cast<uint32>(_sbTransformData.size());
			if (_sbTransformBufferId.isValid() == false && 0 < elementCount)
			{
				_sbTransformBufferId = resourcePool.pushStructuredBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), sizeof(_sbTransformData[0]), elementCount);
			}
			
			if (_sbTransformBufferId.isValid() == true)
			{
				fs::RenderingBase::DxResource& structuredBuffer = resourcePool.getResource(_sbTransformBufferId);
				structuredBuffer.updateBuffer(reinterpret_cast<byte*>(&_sbTransformData[0]), elementCount);
			}
		}

		void ShapeRendererContext::drawColorPallete(const float radius)
		{
			static constexpr uint32 colorCount = 12;
			static const fs::RenderingBase::Color colorArray[colorCount] = {
				// Red => Green
				fs::RenderingBase::Color(1.0f, 0.0f, 0.0f, 1.0f),
				fs::RenderingBase::Color(1.0f, 0.25f, 0.0f, 1.0f),
				fs::RenderingBase::Color(1.0f, 0.5f, 0.0f, 1.0f),
				fs::RenderingBase::Color(1.0f, 0.75f, 0.0f, 1.0f),
				fs::RenderingBase::Color(1.0f, 1.0f, 0.0f, 1.0f),
				fs::RenderingBase::Color(0.5f, 1.0f, 0.0f, 1.0f),

				// Gren => Blue
				fs::RenderingBase::Color(0.0f, 0.875f, 0.125f, 1.0f),
				fs::RenderingBase::Color(0.0f, 0.666f, 1.0f, 1.0f),
				fs::RenderingBase::Color(0.0f, 0.333f, 1.0f, 1.0f),
				fs::RenderingBase::Color(0.0f, 0.0f, 1.0f, 1.0f),

				// Blue => Red
				fs::RenderingBase::Color(0.5f, 0.0f, 1.0f, 1.0f),
				fs::RenderingBase::Color(1.0f, 0.0f, 0.5f, 1.0f),
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
				const fs::RenderingBase::Color& stepsColor = colorArray[colorIndexCorrected];

				// Outer steps
				for (uint32 outerStepIndex = 0; outerStepIndex < outerStepCount; ++outerStepIndex)
				{
					const float outerStepRatio = 1.0f - static_cast<float>(outerStepIndex) / (outerStepCount + outerStepSmoothingOffset);
					setColor(stepsColor * outerStepRatio + fs::RenderingBase::Color(0.0f, 0.0f, 0.0f, 1.0f));

					drawDoubleCircularArc(stepHeight * (innerStepCount + outerStepIndex + 1) + 1.0f, stepHeight * (innerStepCount + outerStepIndex), deltaAngle, deltaAngle * colorIndex);
				}

				// Inner steps
				const fs::RenderingBase::Color deltaColor = fs::RenderingBase::Color(1.0f, 1.0f, 1.0f, 0.0f) / (innerStepCount + innerStepSmoothingOffset);
				for (uint32 innerStepIndex = 0; innerStepIndex < innerStepCount; ++innerStepIndex)
				{
					setColor(stepsColor + deltaColor * static_cast<float>(innerStepCount - innerStepIndex));
				
					drawDoubleCircularArc(stepHeight * (innerStepIndex + 1) + 1.0f, stepHeight * innerStepIndex, deltaAngle, deltaAngle * colorIndex);
				}
			}
		}
	}
}
