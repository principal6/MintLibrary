#pragma once

#include "stdafx.h"
#include <FsLibrary/SimpleRendering/TriangleBuffer.h>

#include <FsLibrary/SimpleRendering/GraphicDevice.h>


namespace fs
{
	namespace SimpleRendering
	{
		template <typename T>
		inline TriangleBuffer<T>::TriangleBuffer(fs::SimpleRendering::GraphicDevice* const graphicDevice)
			: _graphicDevice{ graphicDevice }
			, _vertexStride{ sizeof(T) }
			, _cachedVertexCount{ 0 }
			, _vertexOffset{ 0 }
			, _cachedIndexCount{ 0 }
			, _indexOffset{ 0 }
		{
			__noop;
		}

		template <typename T>
		FS_INLINE std::vector<T>& TriangleBuffer<T>::vertexArray() noexcept
		{
			return _vertexArray;
		}

		template <typename T>
		FS_INLINE std::vector<TriangleIndexType>& TriangleBuffer<T>::indexArray() noexcept
		{
			return _indexArray;
		}

		template <typename T>
		FS_INLINE void TriangleBuffer<T>::flush()
		{
			_vertexArray.clear();
			_indexArray.clear();
		}

		template<typename T>
		FS_INLINE const bool TriangleBuffer<T>::isReady() const noexcept
		{
			return _vertexArray.empty() == false;
		}

		template <typename T>
		inline void TriangleBuffer<T>::render()
		{
			if (isReady() == false)
			{
				return;
			}

			prepareBuffer();

			_graphicDevice->getDxDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			_graphicDevice->getDxDeviceContext()->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &_vertexStride, &_vertexOffset);
			_graphicDevice->getDxDeviceContext()->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, _indexOffset);
			_graphicDevice->getDxDeviceContext()->DrawIndexed(_cachedIndexCount, _indexOffset, _indexOffset);
		}
		
		template <typename T>
		inline void TriangleBuffer<T>::prepareBuffer()
		{
			const uint32 vertexCount = static_cast<uint32>(_vertexArray.size());
			if (_vertexBuffer.Get() == nullptr || _cachedVertexCount < vertexCount)
			{
				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage			= D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth		= static_cast<UINT>(vertexCount * _vertexStride);
				bufferDescriptor.BindFlags		= D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
				bufferDescriptor.CPUAccessFlags	= D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags		= 0;
				_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, nullptr, _vertexBuffer.ReleaseAndGetAddressOf());

				_cachedVertexCount = vertexCount;
			}

			if (false == _vertexArray.empty())
			{
				D3D11_MAPPED_SUBRESOURCE mappedVertexResource{};
				if (_graphicDevice->getDxDeviceContext()->Map(_vertexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource) == S_OK)
				{
					memcpy(mappedVertexResource.pData, &_vertexArray[0], static_cast<size_t>(vertexCount) * _vertexStride);

					_graphicDevice->getDxDeviceContext()->Unmap(_vertexBuffer.Get(), 0);
				}
			}

			const uint32 indexCount = static_cast<uint32>(_indexArray.size());
			if (_indexBuffer.Get() == nullptr || _cachedIndexCount < indexCount)
			{
				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage			= D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth		= static_cast<UINT>(indexCount * sizeof(TriangleIndexType));
				bufferDescriptor.BindFlags		= D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
				bufferDescriptor.CPUAccessFlags	= D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags		= 0;
				_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, nullptr, _indexBuffer.ReleaseAndGetAddressOf());

				_cachedIndexCount = indexCount;
			}

			if (false == _indexArray.empty())
			{
				D3D11_MAPPED_SUBRESOURCE mappedIndexResource{};
				if (_graphicDevice->getDxDeviceContext()->Map(_indexBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedIndexResource) == S_OK)
				{
					memcpy(mappedIndexResource.pData, &_indexArray[0], static_cast<size_t>(indexCount) * sizeof(TriangleIndexType));

					_graphicDevice->getDxDeviceContext()->Unmap(_indexBuffer.Get(), 0);
				}
			}
		}
	}
}
