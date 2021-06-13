#pragma once


#ifndef MINT_RECTANGLE_RENDERER_CONTEXT_H
#define MINT_RECTANGLE_RENDERER_CONTEXT_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/IRendererContext.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>


namespace mint
{
    namespace RenderingBase
    {
        class RectangleRendererContext final : public IRendererContext
        {
            static constexpr uint32             kVertexCountPerRectangle = 4;

        public:
                                                RectangleRendererContext(mint::RenderingBase::GraphicDevice* const graphicDevice);
            virtual                             ~RectangleRendererContext() = default;

        public:
            virtual void                        initializeShaders() noexcept override final;
            virtual const bool                  hasData() const noexcept override final;
            virtual void                        flush() noexcept override final;
            virtual void                        render() noexcept final;
            virtual void                        renderAndFlush() noexcept final;

        public:
            MINT_INLINE void                      setSize(const mint::Float2& size) { _size = size; }

        public:
            void                                drawColored();
            void                                drawTextured(const mint::Float2& texturePosition, const mint::Float2& textureSize);
            void                                drawColoredTextured(const mint::Float2& texturePosition, const mint::Float2& textureSize);

        private:
            void                                prepareIndexArray();

        private:
            mint::Float2                          _size;
        
        private:
            LowLevelRenderer<VS_INPUT_SHAPE>    _lowLevelRenderer;
            DxObjectId                          _vertexShaderId;
            DxObjectId                          _pixelShaderId;
        };
    }
}


#endif // !MINT_RECTANGLE_RENDERER_CONTEXT_H
