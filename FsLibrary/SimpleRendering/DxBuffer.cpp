#include <stdafx.h>
#include <SimpleRendering/DxBuffer.h>

#include <Algorithm.hpp>
#include <SimpleRendering\GraphicDevice.h>


namespace fs
{
#pragma region Static function definitions
	FS_INLINE const D3D11_BIND_FLAG convertBufferTypeToDxBindFlag(const DxBufferType bufferType)
	{
		if (bufferType == DxBufferType::VertexBuffer)
		{ 
			return D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		}
		else if (bufferType == DxBufferType::IndexBuffer)
		{
			return D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		}
		else if (bufferType == DxBufferType::ConstantBuffer)
		{
			return D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
		}
		return D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
	}
#pragma endregion


	const DxBuffer DxBuffer::kNullInstance(nullptr);
	DxBuffer::DxBuffer(GraphicDevice* const graphicDevice)
		: IDxObject(graphicDevice, DxObjectType::Buffer)
	{
		__noop;
	}

	void DxBuffer::update()
	{
		D3D11_MAPPED_SUBRESOURCE mappedVertexResource{};
		if (_graphicDevice->getDxDeviceContext()->Map(_buffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource) == S_OK)
		{
			memcpy(mappedVertexResource.pData, _bufferContent, _bufferSize);

			_graphicDevice->getDxDeviceContext()->Unmap(_buffer.Get(), 0);
		}
	}

	void DxBuffer::reset(const byte* const bufferContent, const uint32 bufferSize)
	{
		ComPtr<ID3D11Buffer> newBuffer;

		D3D11_BUFFER_DESC bufferDescriptor{};
		bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		bufferDescriptor.ByteWidth = bufferSize;
		bufferDescriptor.BindFlags = fs::convertBufferTypeToDxBindFlag(_bufferType);
		bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bufferDescriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = bufferContent;

		if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, (nullptr != bufferContent) ? &subresourceData : nullptr, newBuffer.ReleaseAndGetAddressOf())))
		{
			_bufferContent = bufferContent;
			_bufferSize = bufferSize;

			std::swap(_buffer, newBuffer);
		}
	}

	void DxBuffer::bind() const noexcept
	{
		if (_bufferType == DxBufferType::VertexBuffer)
		{

		}
		else if (_bufferType == DxBufferType::IndexBuffer)
		{

		}
	}

	void DxBuffer::bindToShader(const DxShaderType shaderType, const uint32 bindSlot) const noexcept
	{
		if (_bufferType == DxBufferType::ConstantBuffer)
		{
			if (shaderType == DxShaderType::VertexShader)
			{
				_graphicDevice->getDxDeviceContext()->VSSetConstantBuffers(bindSlot, 1, _buffer.GetAddressOf());
			}
			else if (shaderType == DxShaderType::PixelShader)
			{
				_graphicDevice->getDxDeviceContext()->PSSetConstantBuffers(bindSlot, 1, _buffer.GetAddressOf());
			}
		}
	}


	DxBufferPool::DxBufferPool(GraphicDevice* const graphicDevice)
		: IDxObject(graphicDevice, DxObjectType::Pool)
	{
	}

	const DxObjectId& DxBufferPool::pushBuffer(const DxBufferType bufferType, const byte* const bufferContent, const uint32 bufferSize)
	{
		DxBuffer buffer{ _graphicDevice };

		D3D11_BUFFER_DESC bufferDescriptor{};
		bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
		bufferDescriptor.ByteWidth = bufferSize;
		bufferDescriptor.BindFlags = fs::convertBufferTypeToDxBindFlag(bufferType);
		bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
		bufferDescriptor.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subresourceData{};
		subresourceData.pSysMem = bufferContent;

		if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, (nullptr != bufferContent) ? &subresourceData : nullptr, buffer._buffer.ReleaseAndGetAddressOf())))
		{
			buffer._bufferType = bufferType;
			buffer._bufferContent = bufferContent;
			buffer._bufferSize = bufferSize;

			buffer.assignIdXXX();
			_bufferArray.emplace_back(std::move(buffer));
			return _bufferArray.back().getId();
		}
		return DxObjectId::kInvalidObjectId;
	}

	const DxBuffer& DxBufferPool::getBuffer(const DxObjectId& objectId)
	{
		FS_ASSERT("±èÀå¿ø", objectId.getObjectStype() == DxObjectType::Buffer, "Invalid parameter - ObjectType !!");

		const uint32 index = fs::binarySearch(_bufferArray, objectId);
		if (index != kUint32Max)
		{
			return _bufferArray[index];
		}
		return DxBuffer::kNullInstance;
	}

}
