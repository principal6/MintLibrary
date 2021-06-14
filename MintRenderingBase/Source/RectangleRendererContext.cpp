#include <stdafx.h>
#include <MintRenderingBase/Include/RectangleRendererContext.h>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>


namespace mint
{
    namespace RenderingBase
    {
        MINT_INLINE mint::Float4 getVertexPosition(const uint32 vertexIndex, const mint::Float4& position, const mint::Float2& size)
        {
            return mint::Float4(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y, position._z, position._w);
        }

        MINT_INLINE mint::Float2 getVertexTexturePosition(const uint32 vertexIndex, const mint::Float2& position, const mint::Float2& size)
        {
            return mint::Float2(position._x + (vertexIndex & 1) * size._x, position._y + ((vertexIndex & 2) >> 1) * size._y);
        }

        RectangleRendererContext::RectangleRendererContext(mint::RenderingBase::GraphicDevice* const graphicDevice)
            : IRendererContext(graphicDevice)
            , _lowLevelRenderer{ graphicDevice }
        {
            __noop;
        }

        void RectangleRendererContext::initializeShaders() noexcept
        {
            mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();

            // Compile vertex shader and create input layer
            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>
                    #include <ShaderConstantBuffers>

                    VS_OUTPUT_SHAPE main(VS_INPUT_SHAPE input)
                    {
                        VS_OUTPUT_SHAPE result    = (VS_OUTPUT_SHAPE)0;
                        result._position        = mul(float4(input._position.xyz, 1.0), _cb2DProjectionMatrix);
                        result._color            = input._color;
                        result._texCoord        = input._texCoord;
                        result._info            = input._info;
                        return result;
                    }
                    )"
                };
                const Language::CppHlslTypeInfo& typeInfo = _graphicDevice->getCppHlslSteamData().getTypeInfo(typeid(mint::RenderingBase::VS_INPUT_SHAPE));
                _vertexShaderId = shaderPool.pushVertexShaderFromMemory("RectangleRendererVS", kShaderString, "main", &typeInfo);
            }

            // Compile pixel shader
            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>

                    sampler        sampler0;
                    Texture2D    texture0;
                
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

        const bool RectangleRendererContext::hasData() const noexcept
        {
            return _lowLevelRenderer.isRenderable();
        }

        void RectangleRendererContext::flush() noexcept
        {
            _lowLevelRenderer.flush();

            flushTransformBuffer();
        }

        void RectangleRendererContext::render() noexcept
        {
            if (_lowLevelRenderer.isRenderable() == true)
            {
                mint::RenderingBase::DxShaderPool& shaderPool = _graphicDevice->getShaderPool();
                shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vertexShaderId);
                shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _pixelShaderId);
                _lowLevelRenderer.render(mint::RenderingBase::RenderingPrimitive::TriangleList);
            }
        }

        void RectangleRendererContext::renderAndFlush() noexcept
        {
            render();

            flush();
        }

        void RectangleRendererContext::drawColored()
        {
            auto& vertexArray = _lowLevelRenderer.vertices();
            mint::RenderingBase::VS_INPUT_SHAPE vertex;
            for (uint32 iter = 0; iter < kVertexCountPerRectangle; iter++)
            {
                vertex._position = getVertexPosition(iter, _position, _size);
                vertex._color = getColorInternal(iter);
                vertexArray.push_back(vertex);
            }
            prepareIndexArray();
        }

        void RectangleRendererContext::drawTextured(const mint::Float2& texturePosition, const mint::Float2& textureSize)
        {
            auto& vertexArray = _lowLevelRenderer.vertices();
            mint::RenderingBase::VS_INPUT_SHAPE vertex;
            for (uint32 iter = 0; iter < kVertexCountPerRectangle; iter++)
            {
                vertex._position = getVertexPosition(iter, _position, _size);
                const mint::Float2& texCoord = getVertexTexturePosition(iter, texturePosition, textureSize);
                vertex._texCoord._x = texCoord._x;
                vertex._texCoord._y = texCoord._y;
                vertex._info._x = 1.0f;
                vertexArray.push_back(vertex);
            }
            prepareIndexArray();
        }

        void RectangleRendererContext::drawColoredTextured(const mint::Float2& texturePosition, const mint::Float2& textureSize)
        {
            auto& vertexArray = _lowLevelRenderer.vertices();
            mint::RenderingBase::VS_INPUT_SHAPE vertex;
            for (uint32 iter = 0; iter < kVertexCountPerRectangle; iter++)
            {
                vertex._position = getVertexPosition(iter, _position, _size);
                vertex._color = getColorInternal(iter);
                const mint::Float2& texCoord = getVertexTexturePosition(iter, texturePosition, textureSize);
                vertex._texCoord._x = texCoord._x;
                vertex._texCoord._y = texCoord._y;
                vertex._info._x = 2.0f;
                vertexArray.push_back(vertex);
            }
            prepareIndexArray();
        }

        void RectangleRendererContext::prepareIndexArray()
        {
            const auto& vertexArray = _lowLevelRenderer.vertices();
            const uint32 currentTotalTriangleVertexCount = static_cast<uint32>(vertexArray.size());

            auto& indexArray = _lowLevelRenderer.indices();
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 0);
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 1);
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 2);
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 1);
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 3);
            indexArray.push_back((currentTotalTriangleVertexCount - kVertexCountPerRectangle) + 2);
        }
    }
}
