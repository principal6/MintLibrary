#include <stdafx.h>
#include <MintRenderingBase/Include/FontRendererContext.h>

#include <algorithm>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/LowLevelRenderer.hpp>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/BitVector.hpp>
#include <MintContainer/Include/StringUtil.hpp>

#include <MintPlatform/Include/BinaryFile.hpp>
#include <MintPlatform/Include/FileUtil.hpp>

#include <MintMath/Include/Rect.h>


namespace mint
{
    namespace Rendering
    {
        FontRendererContext::FontRendererContext(GraphicDevice& graphicDevice)
            : IRendererContext(graphicDevice)
        {
            __noop;
        }

        FontRendererContext::FontRendererContext(GraphicDevice& graphicDevice, LowLevelRenderer<VS_INPUT_SHAPE>* const nonOwnedLowLevelRenderer)
            : IRendererContext(graphicDevice, nonOwnedLowLevelRenderer)
        {
            __noop;
        }

        FontRendererContext::~FontRendererContext()
        {
            __noop;
        }

        const bool FontRendererContext::initialize(const FontData& fontData)
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

        const FontData& FontRendererContext::getFontData() const noexcept
        {
            return _fontData;
        }

        void FontRendererContext::initializeShaders() noexcept
        {
            _clipRect = _graphicDevice.getFullScreenClipRect();

            DxShaderPool& shaderPool = _graphicDevice.getShaderPool();

            // Compile vertex shader and create input layer
            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>
                    #include <ShaderConstantBuffers>
                    #include <ShaderStructuredBufferDefinitions>
                    
                    StructuredBuffer<SB_Transform> sbTransform : register(t0);
                    
                    VS_OUTPUT_SHAPE main(VS_INPUT_SHAPE input)
                    {
                        const uint packedInfo       = asuint(input._info.y);
                        const uint drawShade        = (packedInfo >> 30) & 3;
                        const uint transformIndex   = packedInfo & 0x3FFFFFFF;
                        
                        float4 transformedPosition = float4(input._position.xyz, 1.0);
                        transformedPosition = float4(mul(transformedPosition, sbTransform[transformIndex]._transformMatrix).xyz, 1.0);
                        transformedPosition = float4(mul(transformedPosition, _cb2DProjectionMatrix).xyz, 1.0);
                        
                        VS_OUTPUT_SHAPE result  = (VS_OUTPUT_SHAPE)0;
                        result._position        = transformedPosition;
                        result._color           = input._color;
                        result._texCoord        = input._texCoord;
                        result._info            = input._info;
                        result._info.x          = (float)drawShade;
                        result._viewportIndex   = 0;
                        return result;
                    }
                    )"
                };

                using namespace Language;
                const TypeMetaData<CppHlsl::TypeCustomData>& typeMetaData = _graphicDevice.getCppHlslSteamData().getTypeMetaData(typeid(VS_INPUT_SHAPE));
                _vertexShaderID = shaderPool.pushVertexShaderFromMemory("FontRendererVS", kShaderString, "main", &typeMetaData);
            }

            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>
                    
                    [maxvertexcount(3)]
                    void main(triangle VS_OUTPUT_SHAPE input[3], inout TriangleStream<VS_OUTPUT_SHAPE> OutputStream)
                    {
                        for (int i = 0; i < 3; ++i)
                        {
                            OutputStream.Append(input[i]);
                        }
                        OutputStream.RestartStrip();
                    }
                    )"
                };
                _geometryShaderID = shaderPool.pushNonVertexShaderFromMemory("FontRendererGS", kShaderString, "main", DxShaderType::GeometryShader);
            }

            // Compile pixel shader
            {
                static constexpr const char kShaderString[]
                {
                    R"(
                    #include <ShaderStructDefinitions>

                    sampler                 g_sampler0;
                    Texture2D<float>        g_texture0;
                
                    float4 main(VS_OUTPUT_SHAPE input) : SV_Target
                    {
                        const float sampled = g_texture0.Sample(g_sampler0, input._texCoord.xy);
                        float4 sampledColor = float4(input._color.xyz * ((sampled > 0.0) ? 1.0 : 0.0), sampled * input._color.a);
                        
                        const bool drawShade = (input._info.y == 1.0);
                        if (drawShade)
                        {
                            const float2 rbCoord = input._texCoord - float2(ddx(input._texCoord.x), ddy(input._texCoord.y));
                            const float rbSampled = g_texture0.Sample(g_sampler0, rbCoord);
                            if (rbSampled > 0.0)
                            {
                                const float3 rbColor = lerp(sampledColor.xyz * 0.25 * max(rbSampled, 0.25), sampledColor.xyz, sampled);
                                return float4(rbColor, saturate(sampled + rbSampled));
                            }
                        }
                        return sampledColor;
                    }
                    )"
                };
                _pixelShaderID = shaderPool.pushNonVertexShaderFromMemory("FontRendererPS", kShaderString, "main", DxShaderType::PixelShader);
            }
        }

        void FontRendererContext::flush() noexcept
        {
            _lowLevelRenderer->flush();

            flushTransformBuffer();
        }

        void FontRendererContext::render() noexcept
        {
            if (_lowLevelRenderer->isRenderable() == false)
            {
                return;
            }

            prepareTransformBuffer();

            _graphicDevice.getResourcePool().bindToShader(_fontData._fontTextureID, DxShaderType::PixelShader, 0);

            DxShaderPool& shaderPool = _graphicDevice.getShaderPool();
            shaderPool.bindShaderIfNot(DxShaderType::VertexShader, _vertexShaderID);

            if (isUsingMultipleViewports())
            {
                shaderPool.bindShaderIfNot(DxShaderType::GeometryShader, _geometryShaderID);
            }

            shaderPool.bindShaderIfNot(DxShaderType::PixelShader, _pixelShaderID);

            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            DxResource& sbTransformBuffer = resourcePool.getResource(_graphicDevice.getCommonSBTransformID());
            sbTransformBuffer.bindToShader(DxShaderType::VertexShader, sbTransformBuffer.getRegisterIndex());

            _lowLevelRenderer->executeRenderCommands();

            if (isUsingMultipleViewports())
            {
                shaderPool.unbindShader(DxShaderType::GeometryShader);
            }
        }

        void FontRendererContext::drawDynamicText(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption)
        {
            const uint32 textLength = StringUtil::length(wideText);
            drawDynamicText(wideText, textLength, position, fontRenderingOption);
        }

        void FontRendererContext::drawDynamicText(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption)
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
            pushTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
        }

        void FontRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
        {
            const uint32 textLength = StringUtil::length(wideText);
            drawDynamicTextBitFlagged(wideText, textLength, position, fontRenderingOption, bitFlags);
        }

        void FontRendererContext::drawDynamicTextBitFlagged(const wchar_t* const wideText, const uint32 textLength, const Float4& position, const FontRenderingOption& fontRenderingOption, const BitVector& bitFlags)
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
            pushTransformToBuffer(preTranslation, fontRenderingOption._transformMatrix, postTranslation);
        }

        void FontRendererContext::drawGlyph(const wchar_t wideChar, Float2& glyphPosition, const float scale, const bool drawShade, const bool leaveOnlySpace)
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
                        v._color = _defaultColor;
                        v._texCoord._x = glyphInfo._uv0._x;
                        v._texCoord._y = glyphInfo._uv0._y;
                        //v._info._x = _viewportIndex;
                        v._info._y = IRendererContext::packBits2_30AsFloat(drawShade, _sbTransformData.size());
                        v._info._z = 1.0f; // used by ShapeFontRendererContext
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

        void FontRendererContext::pushTransformToBuffer(const Float4& preTranslation, Float4x4 transformMatrix, const Float4& postTranslation)
        {
            SB_Transform transform;
            transform._transformMatrix.preTranslate(preTranslation.getXyz());
            transform._transformMatrix.postTranslate(postTranslation.getXyz());
            transform._transformMatrix *= transformMatrix;
            _sbTransformData.push_back(transform);
        }
    }
}
