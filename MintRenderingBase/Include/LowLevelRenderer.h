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
    namespace Rendering
    {
        class GraphicDevice;
        struct MeshData;


        enum class RenderingPrimitive
        {
            INVALID,
            LineList,
            TriangleList,
        };

        static constexpr uint32 kVertexOffsetZero = 0;


        template <typename T>
        class LowLevelRenderer
        {
            struct RenderCommand
            {
                bool                _isValid = true;
                RenderingPrimitive  _primitive;
                Rect                _clipRect;
                uint32              _vertexOffset = 0;
                uint32              _vertexCount = 0;
                uint32              _indexOffset = 0;
                uint32              _indexCount = 0;
            };

        public:
                                            LowLevelRenderer(GraphicDevice& graphicDevice);
                                            ~LowLevelRenderer() = default;

        public:
            Vector<T>&                      vertices() noexcept;
            Vector<IndexElementType>&       indices() noexcept;
            const uint32                    getVertexCount() const noexcept;
            const uint32                    getIndexCount() const noexcept;

        public:
            void                            pushMesh(const MeshData& meshData) noexcept;

        public:
            void                            setIndexBaseXXX(const IndexElementType base) noexcept;
            const IndexElementType          getIndexBaseXXX() const noexcept;

        public:
            void                            flush() noexcept;
            const bool                      isRenderable() const noexcept;
            void                            render(const RenderingPrimitive renderingPrimitive) noexcept;

        public:
            void                            pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept;
            void                            executeRenderCommands() noexcept;

        private:
            void                            prepareBuffers() noexcept;
            void                            optimizeRenderCommands() noexcept;

        private:
            GraphicDevice&                  _graphicDevice;
            const uint32                    _vertexStride;
        
        private:
            Vector<T>                       _vertices;
            DxObjectID                      _vertexBufferID;

        private:
            Vector<IndexElementType>        _indices;
            IndexElementType                _indexBase;
            DxObjectID                      _indexBufferID;

        private:
            Vector<RenderCommand>           _renderCommands;
        };
    }
}


#endif // !MINT_LOW_LEVEL_RENDERER_H
