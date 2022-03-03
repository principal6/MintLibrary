#pragma once

#include <stdafx.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/MeshData.h>


namespace mint
{
    namespace Rendering
    {
        template <typename T>
        inline LowLevelRenderer<T>::LowLevelRenderer(GraphicDevice& graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _vertexStride{ sizeof(T) }
            , _vertexBufferID{}
            , _indexBase{ 0 }
            , _indexBufferID{}
        {
            __noop;
        }

        template <typename T>
        MINT_INLINE Vector<T>& LowLevelRenderer<T>::vertices() noexcept
        {
            return _vertices;
        }

        template <typename T>
        MINT_INLINE Vector<IndexElementType>& LowLevelRenderer<T>::indices() noexcept
        {
            return _indices;
        }

        template<typename T>
        MINT_INLINE const uint32 LowLevelRenderer<T>::getVertexCount() const noexcept
        {
            return _vertices.size();
        }

        template<typename T>
        MINT_INLINE const uint32 LowLevelRenderer<T>::getIndexCount() const noexcept
        {
            return _indices.size();
        }

        template<typename T>
        MINT_INLINE void LowLevelRenderer<T>::pushMesh(const MeshData& meshData) noexcept
        {
            const uint32 vertexCount = meshData.getVertexCount();
            const uint32 indexCount = meshData.getIndexCount();
            const VS_INPUT* const meshVertices = meshData.getVertices();
            const IndexElementType* const meshIndices = meshData.getIndices();
            for (uint32 vertexIter = 0; vertexIter < vertexCount; ++vertexIter)
            {
                _vertices.push_back(meshVertices[vertexIter]);
            }

            // 여러 메시가 push 될 경우, 추가되는 메시의 vertex index 가
            // 바로 이전 메시의 마지막 vertex index 이후부터 시작되도록 보장한다.
            IndexElementType indexBase = getIndexBaseXXX();
            for (uint32 indexIter = 0; indexIter < indexCount; ++indexIter)
            {
                _indices.push_back(indexBase + meshIndices[indexIter]);
            }
            setIndexBaseXXX(indexBase + vertexCount);
        }

        template<typename T>
        MINT_INLINE void LowLevelRenderer<T>::setIndexBaseXXX(const IndexElementType base) noexcept
        {
            _indexBase = base;
        }

        template<typename T>
        MINT_INLINE const IndexElementType LowLevelRenderer<T>::getIndexBaseXXX() const noexcept
        {
            return _indexBase;
        }

        template <typename T>
        MINT_INLINE void LowLevelRenderer<T>::flush() noexcept
        {
            _vertices.clear();
            _indices.clear();
            _indexBase = 0;
        }

        template<typename T>
        MINT_INLINE const bool LowLevelRenderer<T>::isRenderable() const noexcept
        {
            return _vertices.empty() == false;
        }

        template <typename T>
        inline void LowLevelRenderer<T>::render(const RenderingPrimitive renderingPrimitive) noexcept
        {
            if (isRenderable() == false)
            {
                return;
            }

            prepareBuffers();

            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
            vertexBuffer.bindAsInput();
            indexBuffer.bindAsInput();

            const uint32 vertexCount = static_cast<uint32>(_vertices.size());
            const uint32 indexCount = static_cast<uint32>(_indices.size());

            _graphicDevice.getStateManager().setIaRenderingPrimitive(renderingPrimitive);

            switch (renderingPrimitive)
            {
            case RenderingPrimitive::LineList:
                _graphicDevice.draw(vertexCount, 0);
                break;
            case RenderingPrimitive::TriangleList:
                _graphicDevice.drawIndexed(indexCount, 0, 0);

                break;
            default:
                break;
            }
        }
        
        template<typename T>
        MINT_INLINE void LowLevelRenderer<T>::pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const Rect& clipRect) noexcept
        {
            RenderCommand newRenderCommand;
            newRenderCommand._primitive = primitive;
            newRenderCommand._clipRect = clipRect;
            newRenderCommand._vertexOffset = vertexOffset;
            newRenderCommand._vertexCount = 0;
            newRenderCommand._indexOffset = indexOffset;
            newRenderCommand._indexCount = indexCount;
            _renderCommands.push_back(newRenderCommand);
        }

        template<typename T>
        MINT_INLINE void LowLevelRenderer<T>::executeRenderCommands() noexcept
        {
            if (isRenderable() == false)
            {
                return;
            }

            prepareBuffers();
            
            optimizeRenderCommands();

            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
            vertexBuffer.bindAsInput();
            indexBuffer.bindAsInput();

            const uint32 renderCommandCount = _renderCommands.size();
            for (uint32 renderCommandIndex = 0; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
            {
                const RenderCommand& renderCommand = _renderCommands[renderCommandIndex];
                if (renderCommand._isValid == false)
                {
                    continue;
                }

                D3D11_RECT scissorRect = rectToD3dRect(renderCommand._clipRect);
                _graphicDevice.getStateManager().setRsScissorRectangle(scissorRect);

                _graphicDevice.getStateManager().setIaRenderingPrimitive(renderCommand._primitive);

                switch (renderCommand._primitive)
                {
                case RenderingPrimitive::LineList:
                    _graphicDevice.draw(renderCommand._vertexCount, renderCommand._vertexOffset);
                    break;
                case RenderingPrimitive::TriangleList:
                    _graphicDevice.drawIndexed(renderCommand._indexCount, renderCommand._indexOffset, renderCommand._vertexOffset);
                    break;
                default:
                    break;
                }
            }

            _renderCommands.clear();
        }

        template <typename T>
        inline void LowLevelRenderer<T>::prepareBuffers() noexcept
        {
            DxResourcePool& resourcePool = _graphicDevice.getResourcePool();
            
            const uint32 vertexCount = static_cast<uint32>(_vertices.size());
            if (_vertexBufferID.isValid() == false && vertexCount > 0)
            {
                _vertexBufferID = resourcePool.pushVertexBuffer(&_vertices[0], _vertexStride, vertexCount);
            }

            if (_vertexBufferID.isValid() == true)
            {
                DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferID);
                vertexBuffer.updateBuffer(&_vertices[0], vertexCount);
            }

            const uint32 indexCount = static_cast<uint32>(_indices.size());
            if (_indexBufferID.isValid() == false && indexCount > 0)
            {
                _indexBufferID = resourcePool.pushIndexBuffer(&_indices[0], indexCount);
            }

            if (_indexBufferID.isValid() == true)
            {
                DxResource& indexBuffer = resourcePool.getResource(_indexBufferID);
                indexBuffer.updateBuffer(&_indices[0], indexCount);
            }
        }

        template<typename T>
        inline void LowLevelRenderer<T>::optimizeRenderCommands() noexcept
        {
            uint32 mergeBeginIndex = 0;
            const uint32 renderCommandCount = _renderCommands.size();
            for (uint32 renderCommandIndex = 1; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
            {
                RenderCommand& mergeDestRenderCommand = _renderCommands[mergeBeginIndex];
                RenderCommand& currentRenderCommand = _renderCommands[renderCommandIndex];
                bool isMerged = false;
                if (currentRenderCommand._primitive == mergeDestRenderCommand._primitive && currentRenderCommand._clipRect == mergeDestRenderCommand._clipRect)
                {
                    switch (mergeDestRenderCommand._primitive)
                    {
                    case RenderingPrimitive::LineList:
                        if (mergeDestRenderCommand._vertexOffset + mergeDestRenderCommand._vertexCount == currentRenderCommand._vertexOffset)
                        {
                            mergeDestRenderCommand._vertexCount += currentRenderCommand._vertexCount;
                            isMerged = true;
                        }
                        break;
                    case RenderingPrimitive::TriangleList:
                        if (mergeDestRenderCommand._indexOffset + mergeDestRenderCommand._indexCount == currentRenderCommand._indexOffset)
                        {
                            mergeDestRenderCommand._indexCount += currentRenderCommand._indexCount;
                            isMerged = true;
                        }
                        break;
                    default:
                        break;
                    }
                }

                if (isMerged)
                {
                    currentRenderCommand._isValid = false;
                }
                else
                {
                    mergeBeginIndex = renderCommandIndex;
                }
            }
        }
    }
}
