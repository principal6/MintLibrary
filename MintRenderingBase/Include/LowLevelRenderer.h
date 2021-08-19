#pragma once


#ifndef MINT_LOW_LEVEL_RENDERER_H
#define MINT_LOW_LEVEL_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <Assets/CppHlsl/CppHlslStreamData.h>

#include <MintRenderingBase/Include/DxResource.h>

#include <MintMath/Include/Rect.h>


namespace mint
{
    namespace RenderingBase
    {
        class GraphicDevice;
        struct MeshData;


        enum class RenderingPrimitive
        {
            INVALID,
            LineList,
            TriangleList,
        };


        template <typename T>
        class LowLevelRenderer
        {
            struct RenderCommand
            {
                bool                _isValid = true;
                RenderingPrimitive  _primitive;
                mint::Rect          _clipRect;
                uint32              _vertexOffset = 0;
                uint32              _vertexCount = 0;
                uint32              _indexOffset = 0;
                uint32              _indexCount = 0;
            };

        public:
                                                        LowLevelRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice);
                                                        ~LowLevelRenderer() = default;

        public:
            mint::Vector<T>&                            vertices() noexcept;
            mint::Vector<IndexElementType>&             indices() noexcept;
            const uint32                                getVertexCount() const noexcept;
            const uint32                                getIndexCount() const noexcept;
        
        public:
            void                                        pushMesh(const mint::RenderingBase::MeshData& meshData) noexcept;

        public:
            void                                        setIndexBaseXXX(const IndexElementType base) noexcept;
            const IndexElementType                      getIndexBaseXXX() const noexcept;

        public:
            void                                        flush() noexcept;
            const bool                                  isRenderable() const noexcept;
            void                                        render(const RenderingPrimitive renderingPrimitive) noexcept;

        public:
            void                                        pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const mint::Rect& clipRect) noexcept;
            void                                        executeRenderCommands() noexcept;

        private:
            void                                        prepareBuffers() noexcept;
            void                                        optimizeRenderCommands() noexcept;

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

        private:
            mint::Vector<RenderCommand>                 _renderCommands;
        };
    }
}


#endif // !MINT_LOW_LEVEL_RENDERER_H
