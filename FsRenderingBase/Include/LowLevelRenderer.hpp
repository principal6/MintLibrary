#pragma once

#include <stdafx.h>
#include <FsRenderingBase/Include/LowLevelRenderer.h>

#include <FsContainer/Include/Vector.hpp>

#include <FsRenderingBase/Include/GraphicDevice.h>


namespace fs
{
    namespace RenderingBase
    {
        template <typename T>
        inline LowLevelRenderer<T>::LowLevelRenderer(fs::RenderingBase::GraphicDevice* const graphicDevice)
            : _graphicDevice{ graphicDevice }
            , _vertexStride{ sizeof(T) }
            , _vertexBufferId{}
            , _indexBufferId{}
        {
            __noop;
        }

        template <typename T>
        FS_INLINE fs::Vector<T>& LowLevelRenderer<T>::vertexArray() noexcept
        {
            return _vertexArray;
        }

        template <typename T>
        FS_INLINE fs::Vector<IndexElementType>& LowLevelRenderer<T>::indexArray() noexcept
        {
            return _indexArray;
        }

        template <typename T>
        FS_INLINE void LowLevelRenderer<T>::flush() noexcept
        {
            _vertexArray.clear();
            _indexArray.clear();
        }

        template<typename T>
        FS_INLINE const bool LowLevelRenderer<T>::isRenderable() const noexcept
        {
            return _vertexArray.empty() == false;
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

            const uint32 vertexCount = static_cast<uint32>(_vertexArray.size());
            const uint32 indexCount = static_cast<uint32>(_indexArray.size());
            switch (renderingPrimitive)
            {
            case fs::RenderingBase::RenderingPrimitive::LineList:
                _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
                _graphicDevice->getDxDeviceContext()->Draw(vertexCount, 0);
                break;
            case fs::RenderingBase::RenderingPrimitive::TriangleList:
                _graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                _graphicDevice->getDxDeviceContext()->DrawIndexed(indexCount, 0, 0);
                break;
            default:
                break;
            }
        }
        
        template <typename T>
        inline void LowLevelRenderer<T>::prepareBuffers() noexcept
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
