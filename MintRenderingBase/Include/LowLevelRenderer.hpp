#pragma once

#include <stdafx.h>
#include <MintRenderingBase/Include/LowLevelRenderer.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>
#include <MintRenderingBase/Include/MeshData.h>


namespace mint
{
    namespace RenderingBase
    {
        template <typename T>
        inline LowLevelRenderer<T>::LowLevelRenderer(mint::RenderingBase::GraphicDevice* const graphicDevice)
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
        MINT_INLINE void LowLevelRenderer<T>::pushMesh(const mint::RenderingBase::MeshData& meshData) noexcept
        {
            const uint32 vertexCount = meshData.getVertexCount();
            const uint32 indexCount = meshData.getIndexCount();
            const mint::RenderingBase::VS_INPUT* const meshVertices = meshData.getVertices();
            const mint::RenderingBase::IndexElementType* const meshIndices = meshData.getIndices();
            for (uint32 vertexIter = 0; vertexIter < vertexCount; vertexIter++)
            {
                _vertices.push_back(meshVertices[vertexIter]);
            }

            // 여러 메시가 push 될 경우, 추가되는 메시의 vertex index 가
            // 바로 이전 메시의 마지막 vertex index 이후부터 시작되도록 보장한다.
            mint::RenderingBase::IndexElementType indexBase = getIndexBaseXXX();
            for (uint32 indexIter = 0; indexIter < indexCount; indexIter++)
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
            switch (renderingPrimitive)
            {
            case mint::RenderingBase::RenderingPrimitive::LineList:
                _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
                _graphicDevice->getDxDeviceContext()->Draw(vertexCount, 0);
                break;
            case mint::RenderingBase::RenderingPrimitive::TriangleList:
                _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                _graphicDevice->getDxDeviceContext()->DrawIndexed(indexCount, 0, 0);
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

            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
            vertexBuffer.bindAsInput();
            indexBuffer.bindAsInput();

            const uint32 renderCommandCount = _renderCommands.size();
            for (uint32 renderCommandIndex = 0; renderCommandIndex < renderCommandCount; ++renderCommandIndex)
            {
                const RenderCommand& renderCommand = _renderCommands[renderCommandIndex];

                D3D11_RECT scissorRect = mint::RenderingBase::rectToD3dRect(renderCommand._clipRect);
                _graphicDevice->getDxDeviceContext()->RSSetScissorRects(1, &scissorRect);

                switch (renderCommand._primitive)
                {
                case mint::RenderingBase::RenderingPrimitive::LineList:
                    _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
                    _graphicDevice->getDxDeviceContext()->Draw(renderCommand._vertexCount, renderCommand._vertexOffset);
                    break;
                case mint::RenderingBase::RenderingPrimitive::TriangleList:
                    _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                    _graphicDevice->getDxDeviceContext()->DrawIndexed(renderCommand._indexCount, renderCommand._indexOffset, renderCommand._vertexOffset);
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
    }
}
