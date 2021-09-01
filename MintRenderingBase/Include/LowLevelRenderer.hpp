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
        inline LowLevelRenderer<T>::LowLevelRenderer(mint::Rendering::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _vertexStride{ sizeof(T) }
            , _vertexBufferId{}
            , _indexBase{ 0 }
            , _indexBufferId{}
        {
            __noop;
        }

        template <typename T>
        MINT_INLINE mint::Vector<T>& LowLevelRenderer<T>::vertices() noexcept
        {
            return _vertices;
        }

        template <typename T>
        MINT_INLINE mint::Vector<IndexElementType>& LowLevelRenderer<T>::indices() noexcept
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
        MINT_INLINE void LowLevelRenderer<T>::pushMesh(const mint::Rendering::MeshData& meshData) noexcept
        {
            const uint32 vertexCount = meshData.getVertexCount();
            const uint32 indexCount = meshData.getIndexCount();
            const mint::Rendering::VS_INPUT* const meshVertices = meshData.getVertices();
            const mint::Rendering::IndexElementType* const meshIndices = meshData.getIndices();
            for (uint32 vertexIter = 0; vertexIter < vertexCount; ++vertexIter)
            {
                _vertices.push_back(meshVertices[vertexIter]);
            }

            // ���� �޽ð� push �� ���, �߰��Ǵ� �޽��� vertex index ��
            // �ٷ� ���� �޽��� ������ vertex index ���ĺ��� ���۵ǵ��� �����Ѵ�.
            mint::Rendering::IndexElementType indexBase = getIndexBaseXXX();
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

            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
            vertexBuffer.bindAsInput();
            indexBuffer.bindAsInput();

            const uint32 vertexCount = static_cast<uint32>(_vertices.size());
            const uint32 indexCount = static_cast<uint32>(_indices.size());

            _graphicDevice->getStateManager().setIaRenderingPrimitive(renderingPrimitive);

            switch (renderingPrimitive)
            {
            case mint::Rendering::RenderingPrimitive::LineList:
                _graphicDevice->draw(vertexCount, 0);
                break;
            case mint::Rendering::RenderingPrimitive::TriangleList:
                _graphicDevice->drawIndexed(indexCount, 0, 0);

                break;
            default:
                break;
            }
        }
        
        template<typename T>
        MINT_INLINE void LowLevelRenderer<T>::pushRenderCommandIndexed(const RenderingPrimitive primitive, const uint32 vertexOffset, const uint32 indexOffset, const uint32 indexCount, const mint::Rect& clipRect) noexcept
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
            prepareBuffers();
            
            optimizeRenderCommands();

            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
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

                D3D11_RECT scissorRect = mint::Rendering::rectToD3dRect(renderCommand._clipRect);
                _graphicDevice->getStateManager().setRsScissorRectangle(scissorRect);

                _graphicDevice->getStateManager().setIaRenderingPrimitive(renderCommand._primitive);

                switch (renderCommand._primitive)
                {
                case mint::Rendering::RenderingPrimitive::LineList:
                    _graphicDevice->draw(renderCommand._vertexCount, renderCommand._vertexOffset);
                    break;
                case mint::Rendering::RenderingPrimitive::TriangleList:
                    _graphicDevice->drawIndexed(renderCommand._indexCount, renderCommand._indexOffset, renderCommand._vertexOffset);
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
            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            
            const uint32 vertexCount = static_cast<uint32>(_vertices.size());
            if (_vertexBufferId.isValid() == false && 0 < vertexCount)
            {
                _vertexBufferId = resourcePool.pushVertexBuffer(&_vertices[0], _vertexStride, vertexCount);
            }

            if (_vertexBufferId.isValid() == true)
            {
                DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
                vertexBuffer.updateBuffer(&_vertices[0], vertexCount);
            }

            const uint32 indexCount = static_cast<uint32>(_indices.size());
            if (_indexBufferId.isValid() == false && 0 < indexCount)
            {
                _indexBufferId = resourcePool.pushIndexBuffer(&_indices[0], indexCount);
            }

            if (_indexBufferId.isValid() == true)
            {
                DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
                indexBuffer.updateBuffer(&_indices[0], indexCount);
            }
        }

        template<typename T>
        inline void LowLevelRenderer<T>::optimizeRenderCommands() noexcept
        {
            uint32 mergeBeginIndex = 0;
            uint32 mergePrimitiveCount = 0;
            const uint32 renderCommandCount = _renderCommands.size();
            for (uint32 renderCommandIndex = 1; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
            {
                const RenderCommand& prevRenderCommand = _renderCommands[renderCommandIndex - 1];
                const RenderCommand& currRenderCommand = _renderCommands[renderCommandIndex];

                bool areTwoCommandsHomogeneous = false;
                if (prevRenderCommand._primitive == currRenderCommand._primitive
                    && prevRenderCommand._clipRect == currRenderCommand._clipRect)
                {
                    switch (prevRenderCommand._primitive)
                    {
                    case mint::Rendering::RenderingPrimitive::LineList:
                        if (prevRenderCommand._vertexOffset + prevRenderCommand._vertexCount == currRenderCommand._vertexOffset)
                        {
                            areTwoCommandsHomogeneous = true;
                            mergePrimitiveCount += prevRenderCommand._vertexCount;
                        }
                        break;
                    case mint::Rendering::RenderingPrimitive::TriangleList:
                        if (prevRenderCommand._indexOffset + prevRenderCommand._indexCount == currRenderCommand._indexOffset)
                        {
                            areTwoCommandsHomogeneous = true;
                            mergePrimitiveCount += prevRenderCommand._indexCount;
                        }
                        break;
                    default:
                        break;
                    }
                }

                if (areTwoCommandsHomogeneous == false)
                {
                    const uint32 mergeEndIndex = renderCommandIndex - 1;
                    if (mergeBeginIndex < mergeEndIndex)
                    {
                        // merge previous homogeneous render commands !!!

                        RenderCommand& mergeBeginRenderCommand = _renderCommands[mergeBeginIndex];
                        const RenderCommand& mergeEndRenderCommand = _renderCommands[mergeEndIndex];
                        switch (mergeBeginRenderCommand._primitive)
                        {
                        case mint::Rendering::RenderingPrimitive::LineList:
                            mergeBeginRenderCommand._vertexCount = mergePrimitiveCount + mergeEndRenderCommand._vertexCount;
                            break;
                        case mint::Rendering::RenderingPrimitive::TriangleList:
                            mergeBeginRenderCommand._indexCount = mergePrimitiveCount + mergeEndRenderCommand._indexCount;
                            break;
                        default:
                            break;
                        }

                        for (uint32 mergedRenderCommandIndex = mergeBeginIndex + 1; mergedRenderCommandIndex <= mergeEndIndex; ++mergedRenderCommandIndex)
                        {
                            RenderCommand& mergedRenderCommand = _renderCommands[mergedRenderCommandIndex];
                            mergedRenderCommand._isValid = false;
                        }
                    }

                    mergeBeginIndex = renderCommandIndex;
                    mergePrimitiveCount = 0;
                }
            }
        }
    }
}
