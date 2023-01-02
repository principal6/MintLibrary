#include <MintRenderingBase/Include/ShapeRendererContext.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StackVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Float3x3.h>


namespace mint
{
	namespace Rendering
	{
		ShapeRendererContext::ShapeRendererContext(GraphicDevice& graphicDevice)
			: IRendererContext(graphicDevice)
			, _shapeBorderColor{ Color(1.0f, 1.0f, 1.0f) }
		{
			__noop;
		}

		ShapeRendererContext::~ShapeRendererContext()
		{
			__noop;
		}

		const char* ShapeRendererContext::getDefaultVertexShaderString() const
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
					const uint packedInfo = asuint(input._info.x);
					const uint shapeType = (packedInfo >> 28) & 0xF;
					const uint transformIndex = packedInfo & 0x3FFFFFFF;
						
					float4 transformedPosition = float4(input._position.xyz, 1.0);
					transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
						
					VS_OUTPUT_SHAPE result = (VS_OUTPUT_SHAPE)0;
					result._position = float4(mul(transformedPosition, _cb2DProjectionMatrix).xyz, 1.0);
					result._color = input._color;
					result._texCoord = input._texCoord;
					result._info.x = (float)shapeType;
					result._info.y = input._info.y;
					result._viewportIndex = 0;
						
					return result;
				}
				)"
			};
			return kShaderString;
		}

		const char* ShapeRendererContext::getDefaultGeometryShaderString() const
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
			return kShaderString;
		}

		void ShapeRendererContext::initializeShaders() noexcept
		{
			_clipRect = _graphicDevice.getFullScreenClipRect();

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();

			{
				if (_vertexShaderID.isValid())
				{
					shaderPool.removeShader(_vertexShaderID);
				}
				_vertexShaderID = shaderPool.addShaderFromMemory("ShapeRendererVS", getDefaultVertexShaderString(), "main_shape", GraphicShaderType::VertexShader);

				if (_inputLayoutID.isValid())
				{
					shaderPool.removeInputLayout(_inputLayoutID);
				}
				using namespace Language;
				const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.getCppHlslSteamData().getTypeMetaData(typeid(VS_INPUT_SHAPE));
				_inputLayoutID = shaderPool.addInputLayout(_vertexShaderID, typeMetaData);
			}

			{
				if (_geometryShaderID.isValid())
				{
					shaderPool.removeShader(_geometryShaderID);
				}
				_geometryShaderID = shaderPool.addShaderFromMemory("ShapeRendererGS", getDefaultGeometryShaderString(), "main_shape", GraphicShaderType::GeometryShader);
			}

			{
				static constexpr const char kShaderString[]
				{
					R"(
					#include <ShaderStructDefinitions>
					#include <ShaderConstantBuffers>
					
					sampler				 g_sampler0;
					Texture2D<float4>	   g_texture0;
					
					static const float kDeltaDoublePixel = _cb2DProjectionMatrix[0][0];
					static const float kDeltaPixel = kDeltaDoublePixel * 0.5;
					static const float kDeltaHalfPixel = kDeltaPixel * 0.5;
					
					float4 main_shape(VS_OUTPUT_SHAPE input) : SV_Target
					{
						const float u	   = input._texCoord.x;
						const float v	   = input._texCoord.y;
						const float flipped = input._texCoord.z;
						const float scale   = input._texCoord.w;
						
						float signedDistance = 0.0;
						if (input._info.x == 0.0)
						{
							// Quadratic Bezier
							signedDistance = -(u * u - v);
						}
						else if (input._info.x == 1.0)
						{
							// Solid triangle
							return input._color;
						}
						else if (input._info.x == 2.0)
						{
							// Circular
							signedDistance = flipped * (1.0 - sqrt(u * u + v * v));
						}
						else if (input._info.x == 3.0)
						{
							// Double Circular
							const float outerRadius = scale;
							const float innerRadius = input._info.y;
							const float innerRadiusRelative = 1.0 - innerRadius / outerRadius;
							signedDistance = 1.0 - sqrt(u * u + v * v);
							if (signedDistance > innerRadiusRelative)
							{
								signedDistance = innerRadiusRelative - signedDistance;
							}
							signedDistance *= flipped;
						}
						else if (input._info.x == 4.0)
						{
							// Textured triangle
							return g_texture0.Sample(g_sampler0, input._texCoord.xy);
						}
						else
						{
							// Font triangle
							const float sampled = g_texture0.Sample(g_sampler0, input._texCoord.xy);
							const float4 sampled4 = float4(input._color.xyz * ((sampled > 0.0) ? 1.0 : 0.0), sampled * input._color.a);
							const bool drawShade = (input._info.y == 1.0);
							if (drawShade)
							{
								const float2 rbCoord = input._texCoord - float2(ddx(input._texCoord.x), ddy(input._texCoord.y));
								const float rbSampled = g_texture0.Sample(g_sampler0, rbCoord);
								if (rbSampled > 0.0)
								{
									const float3 rbColor = lerp(sampled4.xyz * 0.25 * max(rbSampled, 0.25), sampled4.xyz, sampled);
									return float4(rbColor, saturate(sampled + rbSampled));
								}
							}
							return sampled4;
						}
						
						// Apply scale to the signed distance for more consistent anti-aliasing
						if (scale > 0.0)
						{
							signedDistance *= (scale * kDeltaPixel);
						}
						
						const float signedDistanceAlpha = (kDeltaHalfPixel < signedDistance) ? 1.0 : 1.0 - saturate(abs(signedDistance - kDeltaHalfPixel) / kDeltaPixel);
						return float4(input._color.xyz, input._color.w * signedDistanceAlpha);
					}
					)"
				};
				if (_pixelShaderID.isValid())
				{
					shaderPool.removeShader(_pixelShaderID);
				}
				_pixelShaderID = shaderPool.addShaderFromMemory("ShapeRendererPS", kShaderString, "main_shape", GraphicShaderType::PixelShader);
			}
		}

		void ShapeRendererContext::flush() noexcept
		{
			_lowLevelRenderer->flush();

			flushTransformBuffer();
		}

		void ShapeRendererContext::render() noexcept
		{
			if (_lowLevelRenderer->isRenderable() == false)
			{
				return;
			}

			prepareTransformBuffer();

			// TODO : Slot 처리...
			if (_fontData._fontTextureID.isValid())
			{
				_graphicDevice.getResourcePool().bindToShader(_fontData._fontTextureID, GraphicShaderType::PixelShader, 0);
			}

			DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
			shaderPool.bindInputLayoutIfNot(_inputLayoutID);
			shaderPool.bindShaderIfNot(GraphicShaderType::VertexShader, _vertexShaderID);

			if (isUsingMultipleViewports())
			{
				shaderPool.bindShaderIfNot(GraphicShaderType::GeometryShader, _geometryShaderID);
			}

			shaderPool.bindShaderIfNot(GraphicShaderType::PixelShader, _pixelShaderID);

			DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
			DxResource& sbTransformBuffer = resourcePool.getResource(_graphicDevice.getCommonSBTransformID());
			sbTransformBuffer.bindToShader(GraphicShaderType::VertexShader, sbTransformBuffer.getRegisterIndex());

			_lowLevelRenderer->executeRenderCommands();

			if (isUsingMultipleViewports())
			{
				shaderPool.unbindShader(GraphicShaderType::GeometryShader);
			}
		}

		bool ShapeRendererContext::initializeFontData(const FontData& fontData)
		{
			if (fontData._fontTextureID.isValid() == false)
			{
				MINT_LOG_ERROR("FontData 의 FontTexture 가 Invalid 합니다!");
				return false;
			}

			if (fontData._glyphInfoArray.empty() == true)
			{
				MINT_LOG_ERROR("FontData 의 GlyphInfo 가 비어 있습니다!");
				return false;
			}

			_fontData = fontData;

			return true;
		}

		void ShapeRendererContext::setShapeBorderColor(const Color& shapeBorderColor) noexcept
		{
			_shapeBorderColor = shapeBorderColor;
		}

		void ShapeRendererContext::setTextColor(const Color& textColor) noexcept
		{
			_textColor = textColor;
		}

		void ShapeRendererContext::testDraw(Float2&& screenOffset)
		{
			testDraw(screenOffset);
		}

		void ShapeRendererContext::testDraw(Float2& screenOffset)
		{
			const float kSize = 40.0f;
			const float kHalfSize = kSize * 0.5f;

			setColor(Color(20, 200, 80));

			// First row
			drawQuadraticBezier(screenOffset + Float2(-kHalfSize, +kHalfSize), screenOffset + Float2(+kHalfSize, +kHalfSize), screenOffset + Float2(0, -kHalfSize));

			drawSolidTriangle(screenOffset + Float2(60 - kHalfSize, -kHalfSize), screenOffset + Float2(60 + kHalfSize, -kHalfSize), screenOffset + Float2(60, +kHalfSize));

			setPosition(Float4(screenOffset + Float2(120, 0)));
			drawCircularTriangle(kHalfSize, 0.0f, false);

			setPosition(Float4(screenOffset + Float2(180, 0)));
			drawQuarterCircle(kHalfSize, 0.0f);

			setPosition(Float4(screenOffset + Float2(240, 20)));
			drawHalfCircle(kHalfSize, 0.0f);

			// Second row
			screenOffset._y += 100.0f;
			setPosition(Float4(screenOffset + Float2(0, 0)));
			drawCircle(kHalfSize);

			setPosition(Float4(screenOffset + Float2(60, 0)));
			drawDoughnut(kHalfSize, kHalfSize * 0.75f);

			setPosition(Float4(screenOffset + Float2(120, 0)));
			drawCircularArc(kHalfSize, Math::kPiOverFour, 0.0f);

			setPosition(Float4(screenOffset + Float2(180, 0)));
			drawDoubleCircularArc(kHalfSize, 10.0f, Math::kPiOverFour, 0.0f);

			setPosition(Float4(screenOffset + Float2(240, 0)));
			drawRectangle(Float2(kSize), 1.0f, 0.0f);

			// Third row
			screenOffset._y += 100.0f;
			setPosition(Float4(screenOffset + Float2(0, 0)));
			drawRoundedRectangle(Float2(kSize), 4.0f, 1.0f, 0.0f);

			setPosition(Float4(screenOffset + Float2(60, 0)));
			drawHalfRoundedRectangle(Float2(kSize), 4.0f, 0.0f);

			drawLine(screenOffset + Float2(120 - kHalfSize, -kHalfSize), screenOffset + Float2(120 + kHalfSize, kHalfSize), 4.0f);

			setPosition(Float4(screenOffset + Float2(180, 0)));
			drawTaperedRectangle(Float2(kSize), 0.5f, 0.25f, 0.0f);

			// Fourth row
			screenOffset._y += 100.0f;
			setPosition(Float4(screenOffset + Float2(60, kHalfSize)));
			drawColorPalleteXXX(kSize);

			screenOffset._y += 100.0f;

			// Font
			{
				setTextColor(Color(0, 40, 80));

				FontRenderingOption fontRenderingOption;
				drawDynamicText(L"Testing`!@#$%^&*()_+ 검사 중...", Float4(screenOffset), fontRenderingOption);
			}
		}

		void ShapeRendererContext::drawLine(const Float2& p0, const Float2& p1, const float thickness)
		{
			drawLineInternal(p0, p1, thickness);
			pushShapeTransformToBuffer(0.0f, false);
		}

		bool ShapeRendererContext::drawLineStrip(const Vector<Float2>& points, const float thickness)
		{
			const uint32 pointCount = points.size();
			MINT_ASSURE(pointCount > 1);

			for (uint32 pointIndex = 1; pointIndex < pointCount; ++pointIndex)
			{
				const Float2& p0 = points[pointIndex - 1];
				const Float2& p1 = points[pointIndex];
				drawLineInternal(p0, p1, thickness);
			}
			pushShapeTransformToBuffer(0.0f, false);
			return true;
		}

		void ShapeRendererContext::drawSolidTriangle(const Float2& pointA, const Float2& pointB, const Float2& pointC)
		{
			drawSolidTriangleInternal(pointA, pointB, pointC, _defaultColor);

			pushShapeTransformToBuffer(0.0f, false);
		}

		void ShapeRendererContext::drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const float halfRadius = radius * 0.5f;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			v._color = _defaultColor;
			v._position = _position;
			v._position._x = -halfRadius;
			v._position._y = -halfRadius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 1.0f;
			v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
			v._info._x = packInfoAsFloat(ShapeType::Circular);
			vertexArray.push_back(v);

			v._position._y += radius;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			vertexArray.push_back(v);

			v._position._x += radius;
			v._position._y = -halfRadius;
			v._texCoord._x = 1.0f;
			v._texCoord._y = 1.0f;
			vertexArray.push_back(v);

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _lowLevelRenderer->indices();
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 1);
			indexArray.push_back(vertexBase + 2);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawRectangle(const Float2& size, const float borderThickness, const float rotationAngle)
		{
			const Float2 halfSize = size * 0.5f;
			const ShapeType shapeType = ShapeType::SolidTriangle;
			if (borderThickness >= 1.0f)
			{
				drawRectangleInternal(Float2(0.0f, -halfSize._y - borderThickness * 0.5f), Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _shapeBorderColor, shapeType);

				drawRectangleInternal(Float2(0.0f, +halfSize._y + borderThickness * 0.5f), Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _shapeBorderColor, shapeType);

				drawRectangleInternal(Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), Float2(borderThickness * 0.5f, halfSize._y), _shapeBorderColor, shapeType);

				drawRectangleInternal(Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), Float2(borderThickness * 0.5f, halfSize._y), _shapeBorderColor, shapeType);
			}

			drawRectangleInternal(Float2::kZero, halfSize, _defaultColor, shapeType);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawTexturedRectangle(const Float2& size, const float rotationAngle)
		{
			const Float2 halfSize = size * 0.5f;
			const ShapeType shapeType = ShapeType::TexturedTriangle;
			drawRectangleInternal(Float2::kZero, halfSize, _defaultColor, shapeType);
			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawTaperedRectangle(const Float2& size, const float tapering, const float bias, const float rotationAngle)
		{
			const Float2 halfSize = size * 0.5f;
			const float horizontalSpace = size._x * (1.0f - tapering);
			static constexpr uint32 kDeltaVertexCount = 4;
			const float horizontalOffsetL = horizontalSpace * bias;
			const float horizontalOffsetR = horizontalSpace * (1.0f - bias);
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -halfSize._x + horizontalOffsetL;
				v._position._y = -halfSize._y;
				v._info._x = packInfoAsFloat(ShapeType::SolidTriangle);
				vertexArray.push_back(v);

				v._position._x = +halfSize._x - horizontalOffsetR;
				v._position._y = -halfSize._y;
				vertexArray.push_back(v);

				v._position._x = -halfSize._x;
				v._position._y = +halfSize._y;
				vertexArray.push_back(v);

				v._position._x = +halfSize._x;
				v._position._y = +halfSize._y;
				vertexArray.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _lowLevelRenderer->indices();
			{
				indexArray.push_back(vertexBase + 0);
				indexArray.push_back(vertexBase + 3);
				indexArray.push_back(vertexBase + 1);

				indexArray.push_back(vertexBase + 0);
				indexArray.push_back(vertexBase + 2);
				indexArray.push_back(vertexBase + 3);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawRoundedRectangle(const Float2& size, const float roundness, const float borderThickness, const float rotationAngle)
		{
			const float normalizedRoundness = Math::saturate(roundness);
			if (normalizedRoundness == 0.0f)
			{
				drawRectangle(size, borderThickness, rotationAngle);
				return;
			}

			const float radius = min(size._x, size._y) * 0.5f * normalizedRoundness;
			const Float2& halfSize = size * 0.5f;
			const Float2& halfCoreSize = halfSize - Float2(radius);

			if (borderThickness >= 1.0f)
			{
				Float2 pointA;
				Float2 pointB;
				Float2 pointC;

				// Left top
				{
					pointA = Float2(-halfSize._x - borderThickness, -halfCoreSize._y);
					pointB = Float2(-halfCoreSize._x, -halfSize._y - borderThickness);
					drawQuadraticBezierInternal(pointA, pointB, -halfSize - Float2(borderThickness), _shapeBorderColor);

					pointC = Float2(-halfCoreSize._x, -halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _shapeBorderColor);
				}

				// Right top
				{
					pointA = Float2(+halfCoreSize._x, -halfSize._y - borderThickness);
					pointB = Float2(+halfSize._x + borderThickness, -halfCoreSize._y);
					drawQuadraticBezierInternal(pointA, pointB, Float2(+halfSize._x, -halfSize._y) + Float2(+borderThickness, -borderThickness), _shapeBorderColor);

					pointC = Float2(+halfCoreSize._x, -halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _shapeBorderColor);
				}

				// Left bottom
				{
					pointA = Float2(-halfCoreSize._x, +halfSize._y + borderThickness);
					pointB = Float2(-halfSize._x - borderThickness, +halfCoreSize._y);
					drawQuadraticBezierInternal(pointA, pointB, Float2(-halfSize._x, +halfSize._y) + Float2(-borderThickness, +borderThickness), _shapeBorderColor);

					pointC = Float2(-halfCoreSize._x, +halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _shapeBorderColor);
				}

				// Right bottom
				{
					pointA = Float2(+halfSize._x + borderThickness, +halfCoreSize._y);
					pointB = Float2(+halfCoreSize._x, +halfSize._y + borderThickness);
					drawQuadraticBezierInternal(pointA, pointB, halfSize + Float2(borderThickness), _shapeBorderColor);

					pointC = Float2(+halfCoreSize._x, +halfCoreSize._y);
					drawSolidTriangleInternal(pointA, pointB, pointC, _shapeBorderColor);
				}

				// Top
				const ShapeType shapeType = ShapeType::SolidTriangle;
				drawRectangleInternal(Float2(0.0f, -halfSize._y - borderThickness * 0.5f), Float2(halfCoreSize._x, borderThickness * 0.5f), _shapeBorderColor, shapeType);

				// Bottom
				drawRectangleInternal(Float2(0.0f, +halfSize._y + borderThickness * 0.5f), Float2(halfCoreSize._x, borderThickness * 0.5f), _shapeBorderColor, shapeType);

				// Left
				drawRectangleInternal(Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), Float2(borderThickness * 0.5f, halfCoreSize._y), _shapeBorderColor, shapeType);

				// Right
				drawRectangleInternal(Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), Float2(borderThickness * 0.5f, halfCoreSize._y), _shapeBorderColor, shapeType);
			}

			drawRoundedRectangleInternal(radius, halfSize, _defaultColor);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawRoundedRectangleVertSplit(const Float2& size, const float roundnessInPixel, const StackVector<Split, 3>& splits, const float rotationAngle)
		{
			if (splits.size() < 2)
			{
				MINT_NEVER;
				return;
			}

			const bool hasMiddleShape = splits.size() == 3;
			const Color& upperColor = splits.front()._color;
			const Color& lowerColor = splits.back()._color;
			const Float2 upperShapeSize = Float2(size._x, size._y * splits[0]._ratio);
			const Float2 middleShapeSize = (hasMiddleShape ? Float2(size._x, size._y * (splits[1]._ratio - splits[0]._ratio)) : Float2::kZero);
			const Float2 lowerShapeSize = Float2(size._x, size._y - upperShapeSize._y - middleShapeSize._y);
			const float upperShapeRoundness = computeNormalizedRoundness(upperShapeSize.minElement(), roundnessInPixel);
			const float lowerShapeRoundness = computeNormalizedRoundness(lowerShapeSize.minElement(), roundnessInPixel);
			drawUpperHalfRoundedRectangleInternal(Float2(0.0f, (-lowerShapeSize._y - middleShapeSize._y) * 0.5f), upperShapeSize, upperShapeRoundness, upperColor);
			if (hasMiddleShape)
			{
				const ShapeType shapeType = ShapeType::SolidTriangle;
				drawRectangleInternal(Float2(0.0f, -middleShapeSize._y * 0.5f), middleShapeSize * 0.5f, splits[1]._color, shapeType);
			}
			drawLowerHalfRoundedRectangleInternal(Float2(0.0f, (upperShapeSize._y + middleShapeSize._y) * 0.5f), lowerShapeSize, lowerShapeRoundness, lowerColor);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawHalfRoundedRectangle(const Float2& size, const float roundness, const float rotationAngle)
		{
			const float normalizedRoundness = Math::saturate(roundness);
			if (normalizedRoundness == 0.0f)
			{
				drawRectangle(size, 0.0f, rotationAngle);
				return;
			}

			drawLowerHalfRoundedRectangleInternal(Float2::kZero, size, normalizedRoundness, _defaultColor);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawQuadraticBezier(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const bool validate)
		{
			drawQuadraticBezierInternal(pointA, pointB, controlPoint, _defaultColor, validate);

			pushShapeTransformToBuffer(0.0f, false);
		}

		void ShapeRendererContext::drawQuarterCircle(const float radius, const float rotationAngle)
		{
			const float halfRadius = radius * 0.5f;

			drawQuarterCircleInternal(Float2::kZero, halfRadius, _defaultColor);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawHalfCircle(const float radius, const float rotationAngle)
		{
			const Float4 originalPosition = _position;
			const float halfRadius = radius * 0.5f;

			const Float4& offset = Float4(+halfRadius, -halfRadius, 0.0f, 0.0f);
			const Float4x4& rotationMatrixA = Float4x4::rotationMatrixZ(-rotationAngle);
			const Float4& rotatedOffsetA = rotationMatrixA.mul(offset);
			setPosition(originalPosition + rotatedOffsetA);
			drawQuarterCircle(radius, rotationAngle);

			const Float4x4& rotationMatrixB = Float4x4::rotationMatrixZ(-(rotationAngle + Math::kPiOverTwo));
			const Float4& rotatedOffsetB = rotationMatrixB.mul(offset);
			setPosition(originalPosition + rotatedOffsetB);
			drawQuarterCircle(radius, rotationAngle + Math::kPiOverTwo);

			setPosition(originalPosition);
		}

		void ShapeRendererContext::drawCircle(const float radius, const bool insideOut)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertices = _lowLevelRenderer->vertices();
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -radius;
				v._position._y = -radius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = +1.0f;
				v._texCoord._z = (insideOut == true) ? -1.0f : +1.0f;
				v._texCoord._w = radius;
				v._info._x = packInfoAsFloat(ShapeType::Circular);
				vertices.push_back(v);

				v._position._x = +radius;
				v._position._y = -radius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = +1.0f;
				vertices.push_back(v);

				v._position._x = -radius;
				v._position._y = +radius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);

				v._position._x = +radius;
				v._position._y = +radius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertices.size()) - kDeltaVertexCount;

			auto& indices = _lowLevelRenderer->indices();
			{
				// Body left upper
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 3);
				indices.push_back(vertexBase + 1);

				// Body right lower
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 2);
				indices.push_back(vertexBase + 3);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(0.0f);
		}

		void ShapeRendererContext::drawEllipse(const float xRadius, const float yRadius, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertices = _lowLevelRenderer->vertices();
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -xRadius;
				v._position._y = -yRadius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = +1.0f;
				v._texCoord._z = +1.0f;
				v._texCoord._w = 0.0f;
				v._info._x = packInfoAsFloat(ShapeType::Circular);
				vertices.push_back(v);

				v._position._x = +xRadius;
				v._position._y = -yRadius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = +1.0f;
				vertices.push_back(v);

				v._position._x = -xRadius;
				v._position._y = +yRadius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);

				v._position._x = +xRadius;
				v._position._y = +yRadius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertices.size()) - kDeltaVertexCount;

			auto& indices = _lowLevelRenderer->indices();
			{
				// Body left upper
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 3);
				indices.push_back(vertexBase + 1);

				// Body right lower
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 2);
				indices.push_back(vertexBase + 3);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(0.0f);
		}

		void ShapeRendererContext::drawDoughnut(const float outerRadius, const float innerRadius)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertices = _lowLevelRenderer->vertices();
			{
				v._color = _defaultColor;
				v._position = _position;
				v._position._x = -outerRadius;
				v._position._y = -outerRadius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = +1.0f;
				v._texCoord._z = +1.0f;
				v._texCoord._w = outerRadius;
				v._info._x = packInfoAsFloat(ShapeType::DoubleCircular);
				v._info._y = innerRadius;
				vertices.push_back(v);

				v._position._x = +outerRadius;
				v._position._y = -outerRadius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = +1.0f;
				vertices.push_back(v);

				v._position._x = -outerRadius;
				v._position._y = +outerRadius;
				v._texCoord._x = -1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);

				v._position._x = +outerRadius;
				v._position._y = +outerRadius;
				v._texCoord._x = +1.0f;
				v._texCoord._y = -1.0f;
				vertices.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertices.size()) - kDeltaVertexCount;

			auto& indices = _lowLevelRenderer->indices();
			{
				// Body left upper
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 3);
				indices.push_back(vertexBase + 1);

				// Body right lower
				indices.push_back(vertexBase + 0);
				indices.push_back(vertexBase + 2);
				indices.push_back(vertexBase + 3);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(0.0f);
		}

		void ShapeRendererContext::drawCircularArc(const float radius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 6;
			const float halfArcAngle = Math::clamp(arcAngle, 0.0f, Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();

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
				v._info._x = packInfoAsFloat(ShapeType::Circular);
				vertexArray.push_back(v);

				v._position._x = +radius * sinHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._y = -radius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			// Left arc section
			{
				v._position._x = -radius * sinHalfArcAngle;
				v._position._y = -radius;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._y = -radius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			// Center
			{
				v._position._x = 0.0f;
				v._position._y = 0.0f;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				vertexArray.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			auto& indexArray = _lowLevelRenderer->indices();
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 2);
			indexArray.push_back(vertexBase + 1);

			indexArray.push_back(vertexBase + 3);
			indexArray.push_back(vertexBase + 4);
			indexArray.push_back(vertexBase + 0);

			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 5);
			indexArray.push_back(vertexBase + 2);

			indexArray.push_back(vertexBase + 4);
			indexArray.push_back(vertexBase + 5);
			indexArray.push_back(vertexBase + 0);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle)
		{
			static constexpr uint32 kDeltaVertexCount = 13;
			const float halfArcAngle = Math::clamp(arcAngle, 0.0f, Math::kPi) * 0.5f;
			const float sinHalfArcAngle = sin(halfArcAngle);
			const float cosHalfArcAngle = cos(halfArcAngle);
			const float tanHalfArcAngle = tan(halfArcAngle);
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();

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
				v._info._x = packInfoAsFloat(ShapeType::Circular);
				vertexArray.push_back(v);

				v._position._x = +outerRadius * tanHalfArcAngle;
				v._texCoord._x = +tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._x = +outerRadius * sinHalfArcAngle;
				v._position._y = -outerRadius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			// Left outer arc section
			{
				v._position._x = -outerRadius * tanHalfArcAngle;
				v._position._y = -outerRadius;
				v._texCoord._x = -tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._x = -outerRadius * sinHalfArcAngle;
				v._position._y = -outerRadius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			const float innerRatio = innerRadius / outerRadius;
			// Middle
			{
				v._position._x = 0.0f;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = innerRatio;
				vertexArray.push_back(v);
			}

			// Right inner arc section
			{
				v._position._x = 0.0f;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = -1.0f; // @IMPORTANT
				vertexArray.push_back(v);

				v._position._x = +innerRadius * tanHalfArcAngle;
				v._texCoord._x = +tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._x = +innerRadius * sinHalfArcAngle;
				v._position._y = -innerRadius * cosHalfArcAngle;
				v._texCoord._x = +sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			// Left inner arc section
			{
				v._position._x = -innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				v._texCoord._x = -tanHalfArcAngle;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._x = -innerRadius * sinHalfArcAngle;
				v._position._y = -innerRadius * cosHalfArcAngle;
				v._texCoord._x = -sinHalfArcAngle;
				v._texCoord._y = +cosHalfArcAngle;
				vertexArray.push_back(v);
			}

			// Right side
			{
				v._position._x = +innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				v._texCoord._z = +1.0f; // @IMPORTANT
				vertexArray.push_back(v);
			}

			// Left side
			{
				v._position._x = -innerRadius * tanHalfArcAngle;
				v._position._y = -innerRadius;
				vertexArray.push_back(v);
			}


			auto& indexArray = _lowLevelRenderer->indices();
			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			// Right outer arc section
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 2);
			indexArray.push_back(vertexBase + 1);

			// Left outer arc section
			indexArray.push_back(vertexBase + 3);
			indexArray.push_back(vertexBase + 4);
			indexArray.push_back(vertexBase + 0);

			// Middle-right
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 5);
			indexArray.push_back(vertexBase + 2);

			// Middle-left
			indexArray.push_back(vertexBase + 4);
			indexArray.push_back(vertexBase + 5);
			indexArray.push_back(vertexBase + 0);

			// Right inner arc section
			indexArray.push_back(vertexBase + 6);
			indexArray.push_back(vertexBase + 8);
			indexArray.push_back(vertexBase + 7);

			// Left inner arc section
			indexArray.push_back(vertexBase + 9);
			indexArray.push_back(vertexBase + 10);
			indexArray.push_back(vertexBase + 6);

			// Right side
			indexArray.push_back(vertexBase + 5);
			indexArray.push_back(vertexBase + 11);
			indexArray.push_back(vertexBase + 2);

			// Left side
			indexArray.push_back(vertexBase + 4);
			indexArray.push_back(vertexBase + 12);
			indexArray.push_back(vertexBase + 5);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			pushShapeTransformToBuffer(rotationAngle);
		}

		void ShapeRendererContext::drawColorPalleteXXX(const float radius)
		{
			static constexpr uint32 colorCount = 12;
			static const Color colorArray[colorCount] = {
				// Red => Green
				Color(1.0f, 0.0f, 0.0f, 1.0f),
				Color(1.0f, 0.25f, 0.0f, 1.0f),
				Color(1.0f, 0.5f, 0.0f, 1.0f),
				Color(1.0f, 0.75f, 0.0f, 1.0f),
				Color(1.0f, 1.0f, 0.0f, 1.0f),
				Color(0.5f, 1.0f, 0.0f, 1.0f),

				// Gren => Blue
				Color(0.0f, 0.875f, 0.125f, 1.0f),
				Color(0.0f, 0.666f, 1.0f, 1.0f),
				Color(0.0f, 0.333f, 1.0f, 1.0f),
				Color(0.0f, 0.0f, 1.0f, 1.0f),

				// Blue => Red
				Color(0.5f, 0.0f, 1.0f, 1.0f),
				Color(1.0f, 0.0f, 0.5f, 1.0f),
			};

			static constexpr uint32 outerStepSmoothingOffset = 4;
			static constexpr uint32 innerStepSmoothingOffset = 0;
			const uint32 outerStepCount = 5;
			const uint32 innerStepCount = 4;
			const float stepHeight = radius / (innerStepCount + outerStepCount);

			const float deltaAngle = Math::kTwoPi / colorCount;
			const float halfDeltaAngle = deltaAngle * 0.5f;
			for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
			{
				const float rgbDenom = (colorCount / 3.0f);
				const uint32 rgb = static_cast<uint32>(colorIndex / rgbDenom);

				int32 colorIndexCorrected = colorIndex;
				const Color& stepsColor = colorArray[colorIndexCorrected];

				// Outer steps
				for (uint32 outerStepIndex = 0; outerStepIndex < outerStepCount; ++outerStepIndex)
				{
					const float outerStepRatio = 1.0f - static_cast<float>(outerStepIndex) / (outerStepCount + outerStepSmoothingOffset);
					setColor(stepsColor * outerStepRatio + Color(0.0f, 0.0f, 0.0f, 1.0f));

					drawDoubleCircularArc(stepHeight * (innerStepCount + outerStepIndex + 1) + 1.0f, stepHeight * (innerStepCount + outerStepIndex), deltaAngle, deltaAngle * colorIndex);
				}

				// Inner steps
				const Color deltaColor = Color(1.0f, 1.0f, 1.0f, 0.0f) / (innerStepCount + innerStepSmoothingOffset);
				for (uint32 innerStepIndex = 0; innerStepIndex < innerStepCount; ++innerStepIndex)
				{
					setColor(stepsColor + deltaColor * static_cast<float>(innerStepCount - innerStepIndex));

					drawDoubleCircularArc(stepHeight * (innerStepIndex + 1) + 1.0f, stepHeight * innerStepIndex, deltaAngle, deltaAngle * colorIndex);
				}
			}
		}

		void ShapeRendererContext::drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption)
		{
			const uint32 textLength = StringUtil::length(wideText);
			drawDynamicText(wideText, textLength, position, fontRenderingOption);
		}

		void ShapeRendererContext::drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption)
		{
			const float scaledTextWidth = _fontData.computeTextWidth(wideText, textLength) * fontRenderingOption._scale;
			const float scaledFontSize = _fontData._fontSize * fontRenderingOption._scale;

			Float4 postTranslation;
			if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Rightward)
			{
				postTranslation._x -= (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Centered) ? scaledTextWidth * 0.5f : scaledTextWidth;
			}
			if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
			{
				postTranslation._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
			}
			postTranslation._y += (-scaledFontSize * 0.5f - 1.0f);

			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			Float2 glyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				drawGlyph(wideText[at], glyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, false);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			const Float4& preTranslation = position;
			pushFontTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		void ShapeRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const uint32 textLength = StringUtil::length(wideText);
			drawDynamicTextBitFlagged(wideText, textLength, position, fontRenderingOption, bitFlags);
		}

		void ShapeRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
		{
			const float scaledTextWidth = _fontData.computeTextWidth(wideText, textLength) * fontRenderingOption._scale;
			const float scaledFontSize = _fontData._fontSize * fontRenderingOption._scale;

			Float4 postTranslation;
			if (fontRenderingOption._directionHorz != TextRenderDirectionHorz::Rightward)
			{
				postTranslation._x -= (fontRenderingOption._directionHorz == TextRenderDirectionHorz::Centered) ? scaledTextWidth * 0.5f : scaledTextWidth;
			}
			if (fontRenderingOption._directionVert != TextRenderDirectionVert::Centered)
			{
				postTranslation._y += (fontRenderingOption._directionVert == TextRenderDirectionVert::Upward) ? -scaledFontSize * 0.5f : +scaledFontSize * 0.5f;
			}
			postTranslation._y += (-scaledFontSize * 0.5f - 1.0f);

			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			Float2 glyphPosition = Float2(0.0f, 0.0f);
			for (uint32 at = 0; at < textLength; ++at)
			{
				drawGlyph(wideText[at], glyphPosition, fontRenderingOption._scale, fontRenderingOption._drawShade, !bitFlags.get(at));
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);

			const Float4& preTranslation = position;
			pushFontTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
		}

		float ShapeRendererContext::computeNormalizedRoundness(const float minSize, const float roundnessInPixel) const
		{
			return Math::clamp((roundnessInPixel * 2.0f) / minSize, 0.0f, 1.0f);
		}

		void ShapeRendererContext::drawLineInternal(const Float2& p0, const Float2& p1, const float thickness)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const Float2& dir = Float2::normalize(p1 - p0);
			const Float2& normal = Float2(-dir._y, dir._x);
			const float halfThickness = thickness * 0.5f;

			const Float2 v0 = p0 - normal * halfThickness;
			const Float2 v1 = p1 - normal * halfThickness;
			const Float2 v2 = p0 + normal * halfThickness;
			const Float2 v3 = p1 + normal * halfThickness;

			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			v._color = _defaultColor;
			v._position = _position;
			v._position._x = v0._x;
			v._position._y = v0._y;
			v._info._x = packInfoAsFloat(ShapeType::SolidTriangle);
			vertexArray.push_back(v);

			v._position._x = v1._x;
			v._position._y = v1._y;
			vertexArray.push_back(v);

			v._position._x = v2._x;
			v._position._y = v2._y;
			vertexArray.push_back(v);

			v._position._x = v3._x;
			v._position._y = v3._y;
			vertexArray.push_back(v);

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _lowLevelRenderer->indices();
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 3);
			indexArray.push_back(vertexBase + 1);

			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 2);
			indexArray.push_back(vertexBase + 3);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);
		}

		void ShapeRendererContext::drawSolidTriangleInternal(const Float2& pointA, const Float2& pointB, const Float2& pointC, const Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			{
				v._color = color;
				v._position = _position;
				v._position._x = pointA._x;
				v._position._y = pointA._y;
				v._info._x = packInfoAsFloat(ShapeType::SolidTriangle);
				vertexArray.push_back(v);

				v._position._x = pointC._x;
				v._position._y = pointC._y;
				vertexArray.push_back(v);

				v._position._x = pointB._x;
				v._position._y = pointB._y;
				vertexArray.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _lowLevelRenderer->indices();
			{
				indexArray.push_back(vertexBase + 0);
				indexArray.push_back(vertexBase + 1);
				indexArray.push_back(vertexBase + 2);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);
		}

		void ShapeRendererContext::drawRectangleInternal(const Float2& offset, const Float2& halfSize, const Color& color, const ShapeType shapeType)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			{
				v._color = color;
				v._position = _position;
				v._position._x = offset._x - halfSize._x;
				v._position._y = offset._y - halfSize._y;
				v._info._x = packInfoAsFloat(shapeType);
				v._texCoord._x = _uv0._x;
				v._texCoord._y = _uv0._y;
				vertexArray.push_back(v);

				v._position._x = offset._x + halfSize._x;
				v._position._y = offset._y - halfSize._y;
				v._texCoord._x = _uv1._x;
				v._texCoord._y = _uv0._y;
				vertexArray.push_back(v);

				v._position._x = offset._x - halfSize._x;
				v._position._y = offset._y + halfSize._y;
				v._texCoord._x = _uv0._x;
				v._texCoord._y = _uv1._y;
				vertexArray.push_back(v);

				v._position._x = offset._x + halfSize._x;
				v._position._y = offset._y + halfSize._y;
				v._texCoord._x = _uv1._x;
				v._texCoord._y = _uv1._y;
				vertexArray.push_back(v);
			}


			auto& indexArray = _lowLevelRenderer->indices();
			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

			// Body left upper
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 3);
			indexArray.push_back(vertexBase + 1);

			// Body right lower
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 2);
			indexArray.push_back(vertexBase + 3);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);
		}

		void ShapeRendererContext::drawRoundedRectangleInternal(const float radius, const Float2& halfSize, const Color& color)
		{
			const Float2& halfCoreSize = halfSize - Float2(radius);

			Float2 pointA;
			Float2 pointB;
			Float2 pointC;

			// Center box
			{
				pointA = Float2(-halfCoreSize._x, -halfSize._y);
				pointB = Float2(+halfCoreSize._x, -halfSize._y);
				pointC = Float2(-halfCoreSize._x, +halfSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);

				pointA = Float2(+halfCoreSize._x, +halfSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);
			}

			// Left top corner
			{
				pointA = Float2(-halfSize._x, -halfCoreSize._y);
				pointB = Float2(-halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, -halfSize, color, false);
			}

			// Left side box
			{
				pointC = Float2(-halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);

				pointA = Float2(-halfSize._x, -halfCoreSize._y);
				pointB = Float2(-halfSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);
			}

			// Left bottom corner
			{
				pointA = Float2(-halfSize._x, +halfCoreSize._y);
				pointB = Float2(-halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, Float2(-halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointB, pointA, Float2(-halfCoreSize._x, +halfCoreSize._y), color);
			}

			// Right top corner
			{
				pointA = Float2(+halfSize._x, -halfCoreSize._y);
				pointB = Float2(+halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, Float2(+halfSize._x, -halfSize._y), color, false);
			}

			// Right side box
			{
				pointC = Float2(+halfCoreSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointC, pointB, pointA, color);

				pointA = Float2(+halfSize._x, -halfCoreSize._y);
				pointB = Float2(+halfSize._x, +halfCoreSize._y);
				drawSolidTriangleInternal(pointA, pointB, pointC, color);
			}

			// Right bottom corner
			{
				pointA = Float2(+halfSize._x, +halfCoreSize._y);
				pointB = Float2(+halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, Float2(+halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointA, pointB, Float2(+halfCoreSize._x, +halfCoreSize._y), color);
			}
		}

		void ShapeRendererContext::drawUpperHalfRoundedRectangleInternal(const Float2& offset, const Float2& size, const float normalizedRoundness, const Color& color)
		{
			const float radius = min(size._x, size._y) * 0.5f * normalizedRoundness;
			const Float2 halfSize = size * 0.5f;
			const Float2 halfCoreSize = halfSize - Float2(radius);
			const ShapeType shapeType = ShapeType::SolidTriangle;

			// Center box
			drawRectangleInternal(offset, Float2(halfCoreSize._x, halfSize._y), color, shapeType);

			// Left side box
			const float halfSquareSize = (halfSize._x - halfCoreSize._x) * 0.5f;
			drawRectangleInternal(offset + Float2(-halfCoreSize._x - halfSquareSize, +halfSquareSize), Float2(halfSquareSize, halfSize._y - halfSquareSize), color, shapeType);

			// Right side box
			drawRectangleInternal(offset + Float2(halfCoreSize._x + halfSquareSize, +halfSquareSize), Float2(halfSquareSize, halfSize._y - halfSquareSize), color, shapeType);

			Float2 pointA;
			Float2 pointB;
			// Left top corner
			{
				pointA = offset + Float2(-halfSize._x, -halfCoreSize._y);
				pointB = offset + Float2(-halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, offset + Float2(-halfSize._x, -halfSize._y), color, false);
				drawSolidTriangleInternal(pointA, pointB, offset + Float2(-halfCoreSize._x, -halfCoreSize._y), color);
			}

			// Right top corner
			{
				pointA = offset + Float2(+halfSize._x, -halfCoreSize._y);
				pointB = offset + Float2(+halfCoreSize._x, -halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, offset + Float2(+halfSize._x, -halfSize._y), color, false);
				drawSolidTriangleInternal(offset + Float2(+halfCoreSize._x, -halfCoreSize._y), pointB, pointA, color);
			}
		}

		void ShapeRendererContext::drawLowerHalfRoundedRectangleInternal(const Float2& offset, const Float2& size, const float normalizedRoundness, const Color& color)
		{
			const float radius = min(size._x, size._y) * 0.5f * normalizedRoundness;
			const Float2 halfSize = size * 0.5f;
			const Float2 halfCoreSize = halfSize - Float2(radius);
			const ShapeType shapeType = ShapeType::SolidTriangle;

			// Center box
			drawRectangleInternal(offset, Float2(halfCoreSize._x, halfSize._y), color, shapeType);

			// Left side box
			const float halfSquareSize = (halfSize._x - halfCoreSize._x) * 0.5f;
			drawRectangleInternal(offset + Float2(-halfCoreSize._x - halfSquareSize, -halfSquareSize), Float2(halfSquareSize, halfSize._y - halfSquareSize), color, shapeType);

			// Right side box
			drawRectangleInternal(offset + Float2(halfCoreSize._x + halfSquareSize, -halfSquareSize), Float2(halfSquareSize, halfSize._y - halfSquareSize), color, shapeType);

			Float2 pointA;
			Float2 pointB;
			// Left bottom corner
			{
				pointA = offset + Float2(-halfSize._x, +halfCoreSize._y);
				pointB = offset + Float2(-halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointB, pointA, offset + Float2(-halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointB, pointA, offset + Float2(-halfCoreSize._x, +halfCoreSize._y), color);
			}

			// Right bottom corner
			{
				pointA = offset + Float2(+halfSize._x, +halfCoreSize._y);
				pointB = offset + Float2(+halfCoreSize._x, +halfSize._y);
				drawQuadraticBezierInternal(pointA, pointB, offset + Float2(+halfSize._x, +halfSize._y), color, false);
				drawSolidTriangleInternal(pointA, pointB, offset + Float2(+halfCoreSize._x, +halfCoreSize._y), color);
			}
		}

		void ShapeRendererContext::drawQuadraticBezierInternal(const Float2& pointA, const Float2& pointB, const Float2& controlPoint, const Color& color, const bool validate)
		{
			static constexpr uint32 kDeltaVertexCount = 3;
			const Float2(&pointArray)[2] = { pointA, pointB };
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			uint8 flip = 0;
			if (validate == true)
			{
				// The control point must be on the left side of the AB segment!
				const Float3 ac = Float3(controlPoint - pointA);
				const Float3 ab = Float3(pointB - pointA);
				const Float3& cross = Float3::cross(ab, ac);
				flip = (cross._z > 0.0f) ? 1 : 0; // y 좌표계가 (아래가 + 방향으로) 뒤집혀 있어서 z 값 비교도 뒤집혔다.
			}

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			v._color = color;
			v._position = _position;
			v._position._x = pointArray[0 ^ flip]._x;
			v._position._y = pointArray[0 ^ flip]._y;
			v._texCoord._x = 0.0f;
			v._texCoord._y = 0.0f;
			v._texCoord._w = abs(pointA._x - pointB._x);
			v._info._x = packInfoAsFloat(ShapeType::QuadraticBezierTriangle);
			vertexArray.push_back(v);

			v._position._x = pointArray[1 ^ flip]._x;
			v._position._y = pointArray[1 ^ flip]._y;
			v._texCoord._x = 1.0f;
			v._texCoord._y = 1.0f;
			vertexArray.push_back(v);

			v._position._x = controlPoint._x;
			v._position._y = controlPoint._y;
			v._texCoord._x = 0.5f;
			v._texCoord._y = 0.0f;
			vertexArray.push_back(v);

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
			auto& indexArray = _lowLevelRenderer->indices();
			indexArray.push_back(vertexBase + 0);
			indexArray.push_back(vertexBase + 1);
			indexArray.push_back(vertexBase + 2);

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);
		}

		void ShapeRendererContext::drawQuarterCircleInternal(const Float2& offset, const float halfRadius, const Color& color)
		{
			static constexpr uint32 kDeltaVertexCount = 4;
			const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
			const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

			VS_INPUT_SHAPE v;
			auto& vertexArray = _lowLevelRenderer->vertices();
			{
				v._color = color;
				v._position = _position;
				v._position._x = offset._x - halfRadius;
				v._position._y = offset._y - halfRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 1.0f;
				v._texCoord._z = 1.0f;
				v._texCoord._w = halfRadius * 2.0f;
				v._info._x = packInfoAsFloat(ShapeType::Circular);
				vertexArray.push_back(v);

				v._position._x = offset._x + halfRadius;
				v._texCoord._x = 1.0f;
				v._texCoord._y = 1.0f;
				vertexArray.push_back(v);

				v._position._x = offset._x - halfRadius;
				v._position._y = offset._y + halfRadius;
				v._texCoord._x = 0.0f;
				v._texCoord._y = 0.0f;
				vertexArray.push_back(v);

				v._position._x = offset._x + halfRadius;
				v._texCoord._x = 1.0f;
				v._texCoord._y = 0.0f;
				vertexArray.push_back(v);
			}

			const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - 4;
			auto& indexArray = _lowLevelRenderer->indices();
			{
				indexArray.push_back(vertexBase + 0);
				indexArray.push_back(vertexBase + 3);
				indexArray.push_back(vertexBase + 1);

				indexArray.push_back(vertexBase + 0);
				indexArray.push_back(vertexBase + 2);
				indexArray.push_back(vertexBase + 3);
			}

			const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
			_lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, kVertexOffsetZero, indexOffset, indexCount, _clipRect);
		}

		void ShapeRendererContext::drawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace)
		{
			const uint32 glyphIndex = _fontData.getSafeGlyphIndex(wideChar);
			const GlyphInfo& glyphInfo = _fontData._glyphInfoArray[glyphIndex];
			if (leaveOnlySpace == false)
			{
				const float scaledFontHeight = static_cast<float>(_fontData._fontSize) * scale;

				Rect glyphRect;
				glyphRect.left(glyphPosition._x + static_cast<float>(glyphInfo._horiBearingX) * scale);
				glyphRect.right(glyphRect.left() + static_cast<float>(glyphInfo._width) * scale);
				glyphRect.top(glyphPosition._y + scaledFontHeight - static_cast<float>(glyphInfo._horiBearingY) * scale);
				glyphRect.bottom(glyphRect.top() + static_cast<float>(glyphInfo._height) * scale);
				if (glyphRect.right() >= 0.0f && glyphRect.left() <= _graphicDevice.getWindowSize()._x
					&& glyphRect.bottom() >= 0.0f && glyphRect.top() <= _graphicDevice.getWindowSize()._y) // 화면을 벗어나면 렌더링 할 필요가 없으므로
				{
					Vector<VS_INPUT_SHAPE>& vertices = _lowLevelRenderer->vertices();

					// Vertices
					{
						VS_INPUT_SHAPE v;
						v._position._x = glyphRect.left();
						v._position._y = glyphRect.top();
						v._position._z = 0.0f;
						v._color = _textColor;
						v._texCoord._x = glyphInfo._uv0._x;
						v._texCoord._y = glyphInfo._uv0._y;
						v._info._x = packInfoAsFloat(ShapeType::FontTriangle);
						v._info._y = (drawShade ? 1.0f : 0.0f);
						vertices.push_back(v);

						v._position._x = glyphRect.right();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv0._y;
						vertices.push_back(v);

						v._position._x = glyphRect.left();
						v._position._y = glyphRect.bottom();
						v._texCoord._x = glyphInfo._uv0._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.push_back(v);

						v._position._x = glyphRect.right();
						v._texCoord._x = glyphInfo._uv1._x;
						v._texCoord._y = glyphInfo._uv1._y;
						vertices.push_back(v);
					}

					// Indices
					{
						Vector<IndexElementType>& indices = _lowLevelRenderer->indices();
						const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertices.size());
						// 오른손 좌표계
						indices.push_back((currentTotalTriangleVertexCount - 4) + 0);
						indices.push_back((currentTotalTriangleVertexCount - 4) + 3);
						indices.push_back((currentTotalTriangleVertexCount - 4) + 1);

						indices.push_back((currentTotalTriangleVertexCount - 4) + 0);
						indices.push_back((currentTotalTriangleVertexCount - 4) + 2);
						indices.push_back((currentTotalTriangleVertexCount - 4) + 3);
					}
				}
			}

			glyphPosition._x += static_cast<float>(glyphInfo._horiAdvance) * scale;
		}

		void ShapeRendererContext::pushShapeTransformToBuffer(const float rotationAngle, const bool applyInternalPosition)
		{
			SB_Transform transform;
			transform._transformMatrix = Float4x4::rotationMatrixZ(-rotationAngle);
			transform._transformMatrix._m[0][3] = (applyInternalPosition == true) ? _position._x : 0.0f;
			transform._transformMatrix._m[1][3] = (applyInternalPosition == true) ? _position._y : 0.0f;
			//transform._transformMatrix._m[2][3] = (applyInternalPosition == true) ? _position._z : 0.0f;
			_sbTransformData.push_back(transform);
		}

		void ShapeRendererContext::pushFontTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation)
		{
			SB_Transform transform;
			transform._transformMatrix.preTranslate(preTranslation.getXyz());
			transform._transformMatrix.postTranslate(postTranslation.getXyz());
			transform._transformMatrix *= transformMatrix;
			_sbTransformData.push_back(transform);
		}

		float ShapeRendererContext::packInfoAsFloat(const ShapeType shapeType) const noexcept
		{
			return packBits4_28AsFloat(static_cast<uint32>(shapeType), _sbTransformData.size());
		}
	}
}
