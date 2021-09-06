#include <stdafx.h>
#include <MintRenderingBase/Include/ShapeRendererContext.h>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintMath/Include/Float2x2.h>
#include <MintMath/Include/Float3x3.h>


namespace mint
{
    namespace Rendering
    {
        ShapeRendererContext::ShapeRendererContext(mint::Rendering::GraphicDevice* const graphicDevice)
            : IRendererContext(graphicDevice)
            , _lowLevelRenderer{ nullptr }
            , _borderColor{ mint::Rendering::Color(1.0f, 1.0f, 1.0f) }
        {
            _lowLevelRenderer = MINT_NEW(Rendering::LowLevelRenderer<Rendering::VS_INPUT_SHAPE>, graphicDevice);
        }

        ShapeRendererContext::~ShapeRendererContext()
        {
            MINT_DELETE(_lowLevelRenderer);
        }

        void ShapeRendererContext::initializeShaders() noexcept
        {
            _clipRect = _graphicDevice->getFullScreenClipRect();

            mint::Rendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

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
                        const uint packedInfo       = asuint(input._info.y);
                        const uint shapeType        = (packedInfo >> 30) & 3;
                        const uint transformIndex   = packedInfo & 0x3FFFFFFF;
                        
                        float4 transformedPosition = float4(input._position.xyz, 1.0);
                        transformedPosition = mul(transformedPosition, sbTransform[transformIndex]._transformMatrix);
                        
                        VS_OUTPUT_SHAPE result  = (VS_OUTPUT_SHAPE)0;
                        result._position        = float4(mul(transformedPosition, _cb2DProjectionMatrix).xyz, 1.0);
                        result._color           = input._color;
                        result._texCoord        = input._texCoord;
                        result._info.x          = (float)shapeType;
                        result._viewportIndex   = 0;
                        
                        return result;
                    }
                    )"
                };

                using namespace Language;
                const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice->getCppHlslSteamData().getTypeMetaData(typeid(mint::Rendering::VS_INPUT_SHAPE));
                _vertexShaderId = shaderPool.pushVertexShaderFromMemory("ShapeRendererVS", kShaderString, "main_shape", &typeMetaData);
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
                _geometryShaderId = shaderPool.pushNonVertexShaderFromMemory("ShapeRendererGS", kShaderString, "main_shape", DxShaderType::GeometryShader);
            }

            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>
                    #include <ShaderConstantBuffers>

                    static const float kDeltaDoublePixel = _cb2DProjectionMatrix[0][0];
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
                _pixelShaderId = shaderPool.pushNonVertexShaderFromMemory("ShapeRendererPS", kShaderString, "main_shape", DxShaderType::PixelShader);
            }
        }

        const bool ShapeRendererContext::hasData() const noexcept
        {
            return _lowLevelRenderer->isRenderable();
        }

        void ShapeRendererContext::flush() noexcept
        {
            _lowLevelRenderer->flush();

            flushTransformBuffer();
        }

        void ShapeRendererContext::render() noexcept
        {
            if (_lowLevelRenderer->isRenderable() == true)
            {
                prepareTransformBuffer();

                mint::Rendering::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
                shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vertexShaderId);

                if (getUseMultipleViewports() == true)
                {
                    shaderPool.bindShaderIfNot(DxShaderType::GeometryShader, _geometryShaderId);
                }

                shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _pixelShaderId);

                mint::Rendering::DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
                mint::Rendering::DxResource& sbTransformBuffer = resourcePool.getResource(_graphicDevice->getCommonSbTransformId());
                sbTransformBuffer.bindToShader(DxShaderType::VertexShader, sbTransformBuffer.getRegisterIndex());

                _lowLevelRenderer->executeRenderCommands();

                if (getUseMultipleViewports() == true)
                {
                    shaderPool.unbindShader(DxShaderType::GeometryShader);
                }
            }
        }

        void ShapeRendererContext::renderAndFlush() noexcept
        {
            render();

            flush();
        }

        void ShapeRendererContext::setBorderColor(const mint::Rendering::Color& borderColor) noexcept
        {
            _borderColor = borderColor;
        }

        void ShapeRendererContext::drawQuadraticBezier(const mint::Float2& pointA, const mint::Float2& pointB, const mint::Float2& controlPoint, const bool validate)
        {
            drawQuadraticBezierInternal(pointA, pointB, controlPoint, _defaultColor, validate);

            pushTransformToBuffer(0.0f, false);
        }

        void ShapeRendererContext::drawQuadraticBezierInternal(const mint::Float2& pointA, const mint::Float2& pointB, const mint::Float2& controlPoint, const mint::Rendering::Color& color, const bool validate)
        {
            static constexpr uint32 kDeltaVertexCount = 3;
            const mint::Float2(&pointArray)[2] = { pointA, pointB };
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();
            
            uint8 flip = 0;
            if (validate == true)
            {
                // The control point must be on the left side of the AB segment!
                const mint::Float3 ac = mint::Float3(controlPoint - pointA);
                const mint::Float3 ab = mint::Float3(pointB - pointA);
                const mint::Float3& cross = mint::Float3::cross(ab, ac);
                flip = (cross._z > 0.0f) ? 1 : 0; // y ��ǥ�谡 (�Ʒ��� + ��������) ������ �־ z �� �񱳵� ��������.
            }

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            v._color = color;
            v._position = _position;
            v._position._x = pointArray[0 ^ flip]._x;
            v._position._y = pointArray[0 ^ flip]._y;
            v._texCoord._x = 0.0f;
            v._texCoord._y = 0.0f;
            v._texCoord._w = abs(pointA._x - pointB._x);
            //v._info._x = _viewportIndex;
            v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::QuadraticBezierTriangle);
            vertexArray.push_back(v);

            v._position._x = controlPoint._x;
            v._position._y = controlPoint._y;
            v._texCoord._x = 0.5f;
            v._texCoord._y = 0.0f;
            vertexArray.push_back(v);

            v._position._x = pointArray[1 ^ flip]._x;
            v._position._y = pointArray[1 ^ flip]._y;
            v._texCoord._x = 1.0f;
            v._texCoord._y = 1.0f;
            vertexArray.push_back(v);

            const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
            auto& indexArray = _lowLevelRenderer->indices();
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);
        }

        void ShapeRendererContext::drawSolidTriangle(const mint::Float2& pointA, const mint::Float2& pointB, const mint::Float2& pointC)
        {
            drawSolidTriangleInternal(pointA, pointB, pointC, _defaultColor);

            pushTransformToBuffer(0.0f, false);
        }

        void ShapeRendererContext::drawSolidTriangleInternal(const mint::Float2& pointA, const mint::Float2& pointB, const mint::Float2& pointC, const mint::Rendering::Color& color)
        {
            static constexpr uint32 kDeltaVertexCount = 3;
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            {
                v._color = color;
                v._position = _position;
                v._position._x = pointA._x;
                v._position._y = pointA._y;
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::SolidTriangle);
                vertexArray.push_back(v);

                v._position._x = pointB._x;
                v._position._y = pointB._y;
                vertexArray.push_back(v);

                v._position._x = pointC._x;
                v._position._y = pointC._y;
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
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);
        }

        void ShapeRendererContext::drawCircularTriangle(const float radius, const float rotationAngle, const bool insideOut)
        {
            static constexpr uint32 kDeltaVertexCount = 3;
            const float halfRadius = radius * 0.5f;
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();
            
            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            v._color = _defaultColor;
            v._position = _position;
            v._position._x = -halfRadius;
            v._position._y = -halfRadius;
            v._texCoord._x = 0.0f;
            v._texCoord._y = 1.0f;
            v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
            //v._info._x = _viewportIndex;
            v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::Circular);
            vertexArray.push_back(v);

            v._position._x += radius;
            v._texCoord._x = 1.0f;
            v._texCoord._y = 1.0f;
            vertexArray.push_back(v);

            v._position._x = -halfRadius;
            v._position._y += radius;
            v._texCoord._x = 0.0f;
            v._texCoord._y = 0.0f;
            vertexArray.push_back(v);

            const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

            auto& indexArray = _lowLevelRenderer->indices();
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);
            
            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawQuarterCircle(const float radius, const float rotationAngle)
        {
            const float halfRadius = radius * 0.5f;
            
            drawQuarterCircleInternal(mint::Float2::kZero, halfRadius, _defaultColor);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawQuarterCircleInternal(const mint::Float2& offset, const float halfRadius, const mint::Rendering::Color& color)
        {
            static constexpr uint32 kDeltaVertexCount = 4;
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
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
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::Circular);
                vertexArray.push_back(v);

                v._position._x = offset._x + halfRadius;
                v._texCoord._x = 1.0f;
                v._texCoord._y = 1.0f;
                vertexArray.push_back(v);

                v._position._y = offset._y + halfRadius;
                v._texCoord._x = 1.0f;
                v._texCoord._y = 0.0f;
                vertexArray.push_back(v);

                v._position._x = offset._x - halfRadius;
                v._texCoord._x = 0.0f;
                v._texCoord._y = 0.0f;
                vertexArray.push_back(v);
            }

            const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - 4;
            auto& indexArray = _lowLevelRenderer->indices();
            {
                indexArray.push_back(vertexBase + 0);
                indexArray.push_back(vertexBase + 1);
                indexArray.push_back(vertexBase + 2);

                indexArray.push_back(vertexBase + 0);
                indexArray.push_back(vertexBase + 2);
                indexArray.push_back(vertexBase + 3);
            }

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);
        }

        void ShapeRendererContext::drawHalfCircle(const float radius, const float rotationAngle)
        {
            const mint::Float4 originalPosition = _position;
            const float halfRadius = radius * 0.5f;

            const mint::Float4& offset = mint::Float4(+halfRadius, -halfRadius, 0.0f, 0.0f);
            const mint::Float4x4& rotationMatrixA = mint::Float4x4::rotationMatrixZ(-rotationAngle);
            const mint::Float4& rotatedOffsetA = rotationMatrixA.mul(offset);
            setPosition(originalPosition + rotatedOffsetA);
            drawQuarterCircle(radius, rotationAngle);

            const mint::Float4x4& rotationMatrixB = mint::Float4x4::rotationMatrixZ(-(rotationAngle + mint::Math::kPiOverTwo));
            const mint::Float4& rotatedOffsetB = rotationMatrixB.mul(offset);
            setPosition(originalPosition + rotatedOffsetB);
            drawQuarterCircle(radius, rotationAngle + mint::Math::kPiOverTwo);

            setPosition(originalPosition);
        }

        void ShapeRendererContext::drawCircle(const float radius, const bool insideOut)
        {
            static constexpr uint32 kDeltaVertexCount = 4;
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            {
                v._color = _defaultColor;
                v._position = _position;
                v._position._x = -radius;
                v._position._y = -radius;
                v._texCoord._x = -1.0f;
                v._texCoord._y = +1.0f;
                v._texCoord._z = (insideOut == true) ? -1.0f : 1.0f;
                v._texCoord._w = radius;
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::Circle);
                vertexArray.push_back(v);

                v._position._x = +radius;
                v._position._y = -radius;
                v._texCoord._x = +1.0f;
                v._texCoord._y = +1.0f;
                vertexArray.push_back(v);

                v._position._x = -radius;
                v._position._y = +radius;
                v._texCoord._x = -1.0f;
                v._texCoord._y = -1.0f;
                vertexArray.push_back(v);

                v._position._x = +radius;
                v._position._y = +radius;
                v._texCoord._x = +1.0f;
                v._texCoord._y = -1.0f;
                vertexArray.push_back(v);
            }

            const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;
            
            auto& indexArray = _lowLevelRenderer->indices();
            {
                // Body left upper
                indexArray.push_back(vertexBase + 0);
                indexArray.push_back(vertexBase + 1);
                indexArray.push_back(vertexBase + 2);

                // Body right lower
                indexArray.push_back(vertexBase + 1);
                indexArray.push_back(vertexBase + 3);
                indexArray.push_back(vertexBase + 2);
            }

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);

            pushTransformToBuffer(0.0f);
        }

        void ShapeRendererContext::drawCircularArc(const float radius, const float arcAngle, const float rotationAngle)
        {
            static constexpr uint32 kDeltaVertexCount = 6;
            const float halfArcAngle = mint::Math::clamp(arcAngle, 0.0f, mint::Math::kPi) * 0.5f;
            const float sinHalfArcAngle = sin(halfArcAngle);
            const float cosHalfArcAngle = cos(halfArcAngle);
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
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
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::Circular);
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
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            indexArray.push_back(vertexBase + 3);
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 4);

            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 2);
            indexArray.push_back(vertexBase + 5);

            indexArray.push_back(vertexBase + 4);
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 5);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawDoubleCircularArc(const float outerRadius, const float innerRadius, const float arcAngle, const float rotationAngle)
        {
            static constexpr uint32 kDeltaVertexCount = 13;
            const float halfArcAngle = mint::Math::clamp(arcAngle, 0.0f, mint::Math::kPi) * 0.5f;
            const float sinHalfArcAngle = sin(halfArcAngle);
            const float cosHalfArcAngle = cos(halfArcAngle);
            const float tanHalfArcAngle = tan(halfArcAngle);
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
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
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::Circular);
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
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            // Left outer arc section
            indexArray.push_back(vertexBase + 3);
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 4);

            // Middle-right
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 2);
            indexArray.push_back(vertexBase + 5);

            // Middle-left
            indexArray.push_back(vertexBase + 4);
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 5);

            // Right inner arc section
            indexArray.push_back(vertexBase + 6);
            indexArray.push_back(vertexBase + 7);
            indexArray.push_back(vertexBase + 8);

            // Left inner arc section
            indexArray.push_back(vertexBase + 9);
            indexArray.push_back(vertexBase + 6);
            indexArray.push_back(vertexBase + 10);

            // Right side
            indexArray.push_back(vertexBase + 5);
            indexArray.push_back(vertexBase + 2);
            indexArray.push_back(vertexBase + 11);

            // Left side
            indexArray.push_back(vertexBase + 4);
            indexArray.push_back(vertexBase + 5);
            indexArray.push_back(vertexBase + 12);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawRectangle(const mint::Float2& size, const float borderThickness, const float rotationAngle)
        {
            const mint::Float2 halfSize = size * 0.5f;
            
            if (1.0f <= borderThickness)
            {
                drawRectangleInternal(mint::Float2(0.0f, -halfSize._y - borderThickness * 0.5f), mint::Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _borderColor);

                drawRectangleInternal(mint::Float2(0.0f, +halfSize._y + borderThickness * 0.5f), mint::Float2(halfSize._x + borderThickness, borderThickness * 0.5f), _borderColor);

                drawRectangleInternal(mint::Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), mint::Float2(borderThickness * 0.5f, halfSize._y), _borderColor);

                drawRectangleInternal(mint::Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), mint::Float2(borderThickness * 0.5f, halfSize._y), _borderColor);
            }

            drawRectangleInternal(mint::Float2::kZero, halfSize, _defaultColor);
            
            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawRectangleInternal(const mint::Float2& offset, const mint::Float2& halfSize, const mint::Rendering::Color& color)
        {
            static constexpr uint32 kDeltaVertexCount = 4;
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            {
                v._color = color;
                v._position = _position;
                v._position._x = offset._x - halfSize._x;
                v._position._y = offset._y - halfSize._y;
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::SolidTriangle);
                vertexArray.push_back(v);

                v._position._x = offset._x + halfSize._x;
                v._position._y = offset._y - halfSize._y;
                vertexArray.push_back(v);

                v._position._x = offset._x - halfSize._x;
                v._position._y = offset._y + halfSize._y;
                vertexArray.push_back(v);

                v._position._x = offset._x + halfSize._x;
                v._position._y = offset._y + halfSize._y;
                vertexArray.push_back(v);
            }


            auto& indexArray = _lowLevelRenderer->indices();
            const uint32 vertexBase = static_cast<uint32>(vertexArray.size()) - kDeltaVertexCount;

            // Body left upper
            indexArray.push_back(vertexBase + 0);
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            // Body right lower
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 3);
            indexArray.push_back(vertexBase + 2);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);
        }

        void ShapeRendererContext::drawTaperedRectangle(const mint::Float2& size, const float tapering, const float bias, const float rotationAngle)
        {
            const mint::Float2 halfSize = size * 0.5f;
            const float horizontalSpace = size._x * (1.0f - tapering);
            static constexpr uint32 kDeltaVertexCount = 4;
            const float horizontalOffsetL = horizontalSpace * bias;
            const float horizontalOffsetR = horizontalSpace * (1.0f - bias);
            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            {
                v._color = _defaultColor;
                v._position = _position;
                v._position._x = -halfSize._x + horizontalOffsetL;
                v._position._y = -halfSize._y;
                //v._info._x = _viewportIndex;
                v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::SolidTriangle);
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
                indexArray.push_back(vertexBase + 1);
                indexArray.push_back(vertexBase + 2);

                indexArray.push_back(vertexBase + 1);
                indexArray.push_back(vertexBase + 3);
                indexArray.push_back(vertexBase + 2);
            }

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawRoundedRectangle(const mint::Float2& size, const float roundness, const float borderThickness, const float rotationAngle)
        {
            const float clampedRoundness = mint::Math::saturate(roundness);
            if (clampedRoundness == 0.0f)
            {
                drawRectangle(size, borderThickness, rotationAngle);
                return;
            }

            const float radius = mint::min(size._x, size._y) * 0.5f * clampedRoundness;
            const mint::Float2& halfSize = size * 0.5f;
            const mint::Float2& halfCoreSize = halfSize - mint::Float2(radius);

            if (1.0f <= borderThickness)
            {
                mint::Float2 pointA;
                mint::Float2 pointB;
                mint::Float2 pointC;
                
                // Left top
                {
                    pointA = mint::Float2(-halfSize._x - borderThickness, -halfCoreSize._y);
                    pointB = mint::Float2(-halfCoreSize._x, -halfSize._y - borderThickness);
                    drawQuadraticBezierInternal(pointA, pointB, -halfSize - mint::Float2(borderThickness), _borderColor);

                    pointC = mint::Float2(-halfCoreSize._x, -halfCoreSize._y);
                    drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
                }
                
                // Right top
                {
                    pointA = mint::Float2(+halfCoreSize._x, -halfSize._y - borderThickness);
                    pointB = mint::Float2(+halfSize._x + borderThickness, -halfCoreSize._y);
                    drawQuadraticBezierInternal(pointA, pointB, mint::Float2(+halfSize._x, -halfSize._y) + mint::Float2(+borderThickness, -borderThickness), _borderColor);

                    pointC = mint::Float2(+halfCoreSize._x, -halfCoreSize._y);
                    drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
                }

                // Left bottom
                {
                    pointA = mint::Float2(-halfCoreSize._x, +halfSize._y + borderThickness);
                    pointB = mint::Float2(-halfSize._x - borderThickness, +halfCoreSize._y);
                    drawQuadraticBezierInternal(pointA, pointB, mint::Float2(-halfSize._x, +halfSize._y) + mint::Float2(-borderThickness, +borderThickness), _borderColor);

                    pointC = mint::Float2(-halfCoreSize._x, +halfCoreSize._y);
                    drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
                }

                // Right bottom
                {
                    pointA = mint::Float2(+halfSize._x + borderThickness, +halfCoreSize._y);
                    pointB = mint::Float2(+halfCoreSize._x, +halfSize._y + borderThickness);
                    drawQuadraticBezierInternal(pointA, pointB, halfSize + mint::Float2(borderThickness), _borderColor);

                    pointC = mint::Float2(+halfCoreSize._x, +halfCoreSize._y);
                    drawSolidTriangleInternal(pointA, pointB, pointC, _borderColor);
                }

                // Top
                drawRectangleInternal(mint::Float2(0.0f, -halfSize._y - borderThickness * 0.5f), mint::Float2(halfCoreSize._x, borderThickness * 0.5f), _borderColor);

                // Bottom
                drawRectangleInternal(mint::Float2(0.0f, +halfSize._y + borderThickness * 0.5f), mint::Float2(halfCoreSize._x, borderThickness * 0.5f), _borderColor);

                // Left
                drawRectangleInternal(mint::Float2(-halfSize._x - borderThickness * 0.5f, 0.0f), mint::Float2(borderThickness * 0.5f, halfCoreSize._y), _borderColor);

                // Right
                drawRectangleInternal(mint::Float2(+halfSize._x + borderThickness * 0.5f, 0.0f), mint::Float2(borderThickness * 0.5f, halfCoreSize._y), _borderColor);
            }

            drawRoundedRectangleInternal(radius, halfSize, clampedRoundness, _defaultColor);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawHalfRoundedRectangle(const mint::Float2& size, const float roundness, const float rotationAngle)
        {
            const float clampedRoundness = mint::Math::saturate(roundness);
            if (clampedRoundness == 0.0f)
            {
                drawRectangle(size, 0.0f, rotationAngle);
                return;
            }

            const float radius = mint::min(size._x, size._y) * 0.5f * clampedRoundness;
            const mint::Float2& halfSize = size * 0.5f;

            drawHalfRoundedRectangleInternal(radius, halfSize, clampedRoundness, _defaultColor);

            pushTransformToBuffer(rotationAngle);
        }

        void ShapeRendererContext::drawRoundedRectangleInternal(const float radius, const mint::Float2& halfSize, const float roundness, const mint::Rendering::Color& color)
        {
            const mint::Float2& halfCoreSize = halfSize - mint::Float2(radius);

            mint::Float2 pointA;
            mint::Float2 pointB;
            mint::Float2 pointC;

            // Center box
            {
                pointA = mint::Float2(-halfCoreSize._x, -halfSize._y);
                pointB = mint::Float2(+halfCoreSize._x, -halfSize._y);
                pointC = mint::Float2(-halfCoreSize._x, +halfSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);

                pointA = mint::Float2(+halfCoreSize._x, +halfSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);
            }
            
            // Left top corner
            {
                pointA = mint::Float2(-halfSize._x, -halfCoreSize._y);
                pointB = mint::Float2(-halfCoreSize._x, -halfSize._y);
                drawQuadraticBezierInternal(pointA, pointB, -halfSize, color, false);
            }

            // Left side box
            {
                pointC = mint::Float2(-halfCoreSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);

                pointA = mint::Float2(-halfSize._x, -halfCoreSize._y);
                pointB = mint::Float2(-halfSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);
            }

            // Left bottom corner
            {
                pointA = mint::Float2(-halfSize._x, +halfCoreSize._y);
                pointB = mint::Float2(-halfCoreSize._x, +halfSize._y);
                drawQuadraticBezierInternal(pointB, pointA, mint::Float2(-halfSize._x, +halfSize._y), color, false);
                drawSolidTriangleInternal(pointB, pointA, mint::Float2(-halfCoreSize._x, +halfCoreSize._y), color);
            }

            // Right top corner
            {
                pointA = mint::Float2(+halfSize._x, -halfCoreSize._y);
                pointB = mint::Float2(+halfCoreSize._x, -halfSize._y);
                drawQuadraticBezierInternal(pointB, pointA, mint::Float2(+halfSize._x, -halfSize._y), color, false);
            }

            // Right side box
            {
                pointC = mint::Float2(+halfCoreSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);

                pointA = mint::Float2(+halfSize._x, -halfCoreSize._y);
                pointB = mint::Float2(+halfSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);
            }

            // Right bottom corner
            {
                pointA = mint::Float2(+halfSize._x, +halfCoreSize._y);
                pointB = mint::Float2(+halfCoreSize._x, +halfSize._y);
                drawQuadraticBezierInternal(pointA, pointB, mint::Float2(+halfSize._x, +halfSize._y), color, false);
                drawSolidTriangleInternal(pointA, pointB, mint::Float2(+halfCoreSize._x, +halfCoreSize._y), color);
            }
        }

        void ShapeRendererContext::drawHalfRoundedRectangleInternal(const float radius, const mint::Float2& halfSize, const float roundness, const mint::Rendering::Color& color)
        {
            const mint::Float2& halfCoreSize = halfSize - mint::Float2(radius);

            mint::Float2 pointA;
            mint::Float2 pointB;
            mint::Float2 pointC;

            // Center box
            {
                pointA = mint::Float2(-halfCoreSize._x, -halfSize._y);
                pointB = mint::Float2(+halfCoreSize._x, -halfSize._y);
                pointC = mint::Float2(-halfCoreSize._x, +halfSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);

                pointA = mint::Float2(+halfCoreSize._x, +halfSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);
            }

            // Left side box
            {
                pointA = mint::Float2(-halfSize._x, -halfSize._y);
                pointB = mint::Float2(-halfCoreSize._x, -halfSize._y);
                pointC = mint::Float2(-halfCoreSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);

                pointB = mint::Float2(-halfSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);
            }

            // Left bottom corner
            {
                pointA = mint::Float2(-halfSize._x, +halfCoreSize._y);
                pointB = mint::Float2(-halfCoreSize._x, +halfSize._y);
                drawQuadraticBezierInternal(pointB, pointA, mint::Float2(-halfSize._x, +halfSize._y), color, false);
                drawSolidTriangleInternal(pointB, pointA, mint::Float2(-halfCoreSize._x, +halfCoreSize._y), color);
            }

            // Right side box
            {
                pointA = mint::Float2(+halfSize._x, -halfSize._y);
                pointB = mint::Float2(+halfCoreSize._x, -halfSize._y);
                pointC = mint::Float2(+halfCoreSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointC, pointB, pointA, color);

                pointB = mint::Float2(+halfSize._x, +halfCoreSize._y);
                drawSolidTriangleInternal(pointA, pointB, pointC, color);
            }

            // Right bottom corner
            {
                pointA = mint::Float2(+halfSize._x, +halfCoreSize._y);
                pointB = mint::Float2(+halfCoreSize._x, +halfSize._y);
                drawQuadraticBezierInternal(pointA, pointB, mint::Float2(+halfSize._x, +halfSize._y), color, false);
                drawSolidTriangleInternal(pointA, pointB, mint::Float2(+halfCoreSize._x, +halfCoreSize._y), color);
            }
        }

        void ShapeRendererContext::drawLine(const mint::Float2& p0, const mint::Float2& p1, const float thickness)
        {
            static constexpr uint32 kDeltaVertexCount = 4;
            const mint::Float2& dir = mint::Float2::normalize(p1 - p0);
            const mint::Float2& normal = mint::Float2(-dir._y, dir._x);
            const float halfThickness = thickness * 0.5f;

            const mint::Float2 v0 = p0 - normal * halfThickness;
            const mint::Float2 v1 = p1 - normal * halfThickness;
            const mint::Float2 v2 = p0 + normal * halfThickness;
            const mint::Float2 v3 = p1 + normal * halfThickness;

            const uint32 vertexOffset = _lowLevelRenderer->getVertexCount();
            const uint32 indexOffset = _lowLevelRenderer->getIndexCount();

            Rendering::VS_INPUT_SHAPE v;
            auto& vertexArray = _lowLevelRenderer->vertices();
            v._color = _defaultColor;
            v._position = _position;
            v._position._x = v0._x;
            v._position._y = v0._y;
            //v._info._x = _viewportIndex;
            v._info._y = packShapeTypeAndTransformDataIndexAsFloat(ShapeType::SolidTriangle);
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
            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 2);

            indexArray.push_back(vertexBase + 1);
            indexArray.push_back(vertexBase + 3);
            indexArray.push_back(vertexBase + 2);

            const uint32 indexCount = _lowLevelRenderer->getIndexCount() - indexOffset;
            _lowLevelRenderer->pushRenderCommandIndexed(RenderingPrimitive::TriangleList, 0, indexOffset, indexCount, _clipRect);

            pushTransformToBuffer(0.0f, false);
        }

        const float ShapeRendererContext::packShapeTypeAndTransformDataIndexAsFloat(const ShapeType shapeType) const noexcept
        {
            return packBits2_30AsFloat(static_cast<uint32>(shapeType), _sbTransformData.size());
        }

        void ShapeRendererContext::pushTransformToBuffer(const float rotationAngle, const bool applyInternalPosition)
        {
            mint::Rendering::SB_Transform transform;
            transform._transformMatrix = mint::Float4x4::rotationMatrixZ(-rotationAngle);
            transform._transformMatrix._m[0][3] = (applyInternalPosition == true) ? _position._x : 0.0f;
            transform._transformMatrix._m[1][3] = (applyInternalPosition == true) ? _position._y : 0.0f;
            //transform._transformMatrix._m[2][3] = (applyInternalPosition == true) ? _position._z : 0.0f;
            _sbTransformData.push_back(transform);
        }

        void ShapeRendererContext::drawColorPallete(const float radius)
        {
            static constexpr uint32 colorCount = 12;
            static const mint::Rendering::Color colorArray[colorCount] = {
                // Red => Green
                mint::Rendering::Color(1.0f, 0.0f, 0.0f, 1.0f),
                mint::Rendering::Color(1.0f, 0.25f, 0.0f, 1.0f),
                mint::Rendering::Color(1.0f, 0.5f, 0.0f, 1.0f),
                mint::Rendering::Color(1.0f, 0.75f, 0.0f, 1.0f),
                mint::Rendering::Color(1.0f, 1.0f, 0.0f, 1.0f),
                mint::Rendering::Color(0.5f, 1.0f, 0.0f, 1.0f),

                // Gren => Blue
                mint::Rendering::Color(0.0f, 0.875f, 0.125f, 1.0f),
                mint::Rendering::Color(0.0f, 0.666f, 1.0f, 1.0f),
                mint::Rendering::Color(0.0f, 0.333f, 1.0f, 1.0f),
                mint::Rendering::Color(0.0f, 0.0f, 1.0f, 1.0f),

                // Blue => Red
                mint::Rendering::Color(0.5f, 0.0f, 1.0f, 1.0f),
                mint::Rendering::Color(1.0f, 0.0f, 0.5f, 1.0f),
            };

            static constexpr uint32 outerStepSmoothingOffset = 4;
            static constexpr uint32 innerStepSmoothingOffset = 0;
            const uint32 outerStepCount = 5;
            const uint32 innerStepCount = 4;
            const float stepHeight = radius / (innerStepCount + outerStepCount);
            
            const float deltaAngle = mint::Math::kTwoPi / colorCount;
            const float halfDeltaAngle = deltaAngle * 0.5f;
            for (uint32 colorIndex = 0; colorIndex < colorCount; ++colorIndex)
            {
                const float rgbDenom = (colorCount / 3.0f);
                const uint32 rgb = static_cast<uint32>(colorIndex / rgbDenom);
                
                int32 colorIndexCorrected = colorIndex;
                const mint::Rendering::Color& stepsColor = colorArray[colorIndexCorrected];

                // Outer steps
                for (uint32 outerStepIndex = 0; outerStepIndex < outerStepCount; ++outerStepIndex)
                {
                    const float outerStepRatio = 1.0f - static_cast<float>(outerStepIndex) / (outerStepCount + outerStepSmoothingOffset);
                    setColor(stepsColor * outerStepRatio + mint::Rendering::Color(0.0f, 0.0f, 0.0f, 1.0f));

                    drawDoubleCircularArc(stepHeight * (innerStepCount + outerStepIndex + 1) + 1.0f, stepHeight * (innerStepCount + outerStepIndex), deltaAngle, deltaAngle * colorIndex);
                }

                // Inner steps
                const mint::Rendering::Color deltaColor = mint::Rendering::Color(1.0f, 1.0f, 1.0f, 0.0f) / (innerStepCount + innerStepSmoothingOffset);
                for (uint32 innerStepIndex = 0; innerStepIndex < innerStepCount; ++innerStepIndex)
                {
                    setColor(stepsColor + deltaColor * static_cast<float>(innerStepCount - innerStepIndex));
                
                    drawDoubleCircularArc(stepHeight * (innerStepIndex + 1) + 1.0f, stepHeight * innerStepIndex, deltaAngle, deltaAngle * colorIndex);
                }
            }
        }
    }
}
