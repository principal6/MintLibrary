#pragma once


#ifndef MINT_LOW_LEVEL_RENDERER_H
#define MINT_LOW_LEVEL_RENDERER_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Vector.h>

#include <Assets/Include/CppHlsl/CppHlslStreamData.h>

#include <MintRenderingBase/Include/IDxObject.h>

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
                bool                _isOrdinal = true;
                RenderingPrimitive  _primitive = RenderingPrimitive::INVALID;
                Rect                _clipRect;
                uint32              _vertexOffset = 0;
                uint32              _vertexCount = 0;
                uint32              _indexOffset = 0;
                uint32              _indexCount = 0;
            };

            struct OrdinalRenderCommandGroup
            {
                uint64              _key = 0;
                uint32              _priority = 0;
                uint32              _startRenderCommandIndex = 0;
                uint32              _endRenderCommandIndex = 0;

            public:
                bool operator==(const uint64 key) const noexcept
                {
                    return _key == key;
                }

                struct Evaluator
                {
                    uint64 operator()(const OrdinalRenderCommandGroup& rhs) const noexcept
                    {
                        return rhs._key;
                    }
                };

                struct KeyComparator
                {
                    bool operator()(const OrdinalRenderCommandGroup& lhs, const OrdinalRenderCommandGroup& rhs) const noexcept
                    {
                        return lhs._key < rhs._key;
                    }
                };

                // Priority 가 작을 수록 먼저 그려진다.
                // Priority 가 크면 화면 상 제일 위에 와야 하기 때문!
                struct PriorityComparator
                {
                    bool operator()(const OrdinalRenderCommandGroup& lhs, const OrdinalRenderCommandGroup& rhs) const noexcept
                    {
                        return lhs._priority < rhs._priority;
                    }
                };
            };

        public:
                                            LowLevelRenderer(GraphicDevice& graphicDevice);
                                            ~LowLevelRenderer() = default;

        public:
            Vector<T>&                      vertices() noexcept;
            Vector<IndexElementType>&       indices() noexcept;
            uint32                          getVertexCount() const noexcept;
            uint32                          getIndexCount() const noexcept;

        public:
            void                            pushMesh(const MeshData& meshData) noexcept;

        public:
            void                            setIndexBaseXXX(const IndexElementType base) noexcept;
            IndexElementType                getIndexBaseXXX() const noexcept;

        public:
            void                            flush() noexcept;
            bool                            isRenderable() const noexcept;
            void                            render(const RenderingPrimitive renderingPrimitive) noexcept;

        public:
            void                            pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept;
            void                            beginOrdinalRenderCommands(const uint64 key) noexcept;
            void                            endOrdinalRenderCommands() noexcept;
            void                            setOrdinalRenderCommandGroupPriority(const uint64 key, const uint32 priority) noexcept;
            void                            executeRenderCommands() noexcept;

        private:
            bool                            mergeNewRenderCommand(const RenderCommand& newRenderCommand) noexcept;
            void                            prepareBuffers() noexcept;
            void                            executeRenderCommands_draw(const RenderCommand& renderCommand) const noexcept;

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
            bool                                _isOrdinalMode = false;
            Vector<RenderCommand>               _renderCommands;
            Vector<OrdinalRenderCommandGroup>   _ordinalRenderCommandGroups;
            bool                                _isOrdinalRenderCommandGroupsSorted = false;
        };
    }
}


#endif // !MINT_LOW_LEVEL_RENDERER_H
