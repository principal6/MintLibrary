#pragma once

#include <stdafx.h>
#include <FsRenderingBase/Include/TriangleRenderer.h>

#include <FsRenderingBase/Include/GraphicDevice.h>


namespace fs
{
    namespace RenderingBase
    {
        template <typename T>
        inline TriangleRenderer<T>::TriangleRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _vertexStride{ sizeof(T) }
            , _vertexBufferId{}
            , _indexBufferId{}
        {
            __noop;
        }

        template <typename T>
        FS_INLINE std::vector<T>& TriangleRenderer<T>::vertexArray() noexcept
        {
            return _vertexArray;
        }

        template <typename T>
        FS_INLINE std::vector<IndexElementType>& TriangleRenderer<T>::indexArray() noexcept
        {
            return _indexArray;
        }

        template <typename T>
        FS_INLINE void TriangleRenderer<T>::flush()
        {
            _vertexArray.clear();
            _indexArray.clear();
        }

        template<typename T>
        FS_INLINE const bool TriangleRenderer<T>::isRenderable() const noexcept
        {
            return _vertexArray.empty() == false;
        }

        template <typename T>
        inline void TriangleRenderer<T>::render()
        {
            if (isRenderable() == false)
            {
                return;
            }

            prepareBuffer();

            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
            DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
            vertexBuffer.bindAsInput();
            indexBuffer.bindAsInput();

            const uint32 indexCount = static_cast<uint32>(_indexArray.size());
            _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _graphicDevice->getDxDeviceContext()->DrawIndexed(indexCount, 0, 0);
        }
        
        template <typename T>
        inline void TriangleRenderer<T>::prepareBuffer()
        {
            DxResourcePool& resourcePool = _graphicDevice->getResourcePool();
            
            const uint32 vertexCount = static_cast<uint32>(_vertexArray.size());
            if (_vertexBufferId.isValid() == false && 0 < vertexCount)
            {
                _vertexBufferId = resourcePool.pushVertexBuffer(reinterpret_cast<byte*>(&_vertexArray[0]), _vertexStride, vertexCount);
            }

            if (_vertexBufferId.isValid() == true)
            {
                DxResource& vertexBuffer = resourcePool.getResource(_vertexBufferId);
                vertexBuffer.updateBuffer(reinterpret_cast<byte*>(&_vertexArray[0]), vertexCount);
            }

            const uint32 indexCount = static_cast<uint32>(_indexArray.size());
            if (_indexBufferId.isValid() == false && 0 < indexCount)
            {
                _indexBufferId = resourcePool.pushIndexBuffer(reinterpret_cast<byte*>(&_indexArray[0]), indexCount);
            }

            if (_indexBufferId.isValid() == true)
            {
                DxResource& indexBuffer = resourcePool.getResource(_indexBufferId);
                indexBuffer.updateBuffer(reinterpret_cast<byte*>(&_indexArray[0]), indexCount);
            }
        }
    }
}
