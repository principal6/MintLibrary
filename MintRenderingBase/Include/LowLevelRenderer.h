#pragma once


#ifndef MINT_LOW_LEVEL_RENDERER_H
#define MINT_LOW_LEVEL_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>

#include <MintRenderingBase/Include/DxResource.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;


        enum class RenderingPrimitive
        {
            LineList,
            TriangleList,
        };


        template <typename T>
        class LowLevelRenderer
        {
        public:
                                                        LowLevelRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~LowLevelRenderer() = default;

        public:
            mint::Vector<T>&                              vertexArray() noexcept;
            mint::Vector<IndexElementType>&               indexArray() noexcept;

        public:
            void                                        flush() noexcept;
            const bool                                  isRenderable() const noexcept;
            void                                        render(const RenderingPrimitive renderingPrimitive) noexcept;

        private:
            void                                        prepareBuffers() noexcept;

        private:
            mint::RenderingBase::GraphicDevice* const     _graphicDevice;
            const uint32                                _vertexStride;
        
        private:
            mint::Vector<T>                               _vertexArray;
            DxObjectId                                  _vertexBufferId;

        private:
            mint::Vector<IndexElementType>                _indexArray;
            DxObjectId                                  _indexBufferId;
        };
    }
}


#endif // !MINT_LOW_LEVEL_RENDERER_H
