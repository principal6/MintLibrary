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
        struct MeshData;


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
            mint::Vector<T>&                            vertices() noexcept;
            mint::Vector<IndexElementType>&             indices() noexcept;
        
        public:
            void                                        pushMesh(const mint::RenderingBase::MeshData& meshData) noexcept;

        public:
            void                                        setIndexBase(const IndexElementType base) noexcept;
            const IndexElementType                      getIndexBase() const noexcept;

        public:
            void                                        flush() noexcept;
            const bool                                  isRenderable() const noexcept;
            void                                        render(const RenderingPrimitive renderingPrimitive) noexcept;

        private:
            void                                        prepareBuffers() noexcept;

        private:
            mint::RenderingBase::GraphicDevice* const   _graphicDevice;
            const uint32                                _vertexStride;
        
        private:
            mint::Vector<T>                             _vertices;
            DxObjectId                                  _vertexBufferId;

        private:
            mint::Vector<IndexElementType>              _indices;
            IndexElementType                            _indexBase;
            DxObjectId                                  _indexBufferId;
        };
    }
}


#endif // !MINT_LOW_LEVEL_RENDERER_H
