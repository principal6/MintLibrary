#include <stdafx.h>
#include <FsLibrary/SimpleRendering/DxBuffer.h>

#include <FsLibrary/Algorithm.hpp>
#include <FsLibrary/SimpleRendering/GraphicDevice.h>


namespace fs
{
	namespace SimpleRendering
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


		DxBuffer DxBuffer::s_invalidInstance(nullptr);
		DxBuffer::DxBuffer(GraphicDevice* const graphicDevice)
			: IDxObject(graphicDevice, DxObjectType::Buffer)
			, _bufferType{ DxBufferType::INVALID }
			, _bufferSize{ 0 }
			, _elementStride{ 0 }
			, _elementCount{ 0 }
			, _elementOffset{ 0 }
		{
			__noop;
		}

		const bool DxBuffer::create(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount)
		{
			switch (_bufferType)
			{
			case fs::SimpleRendering::DxBufferType::INVALID:
				break;
			case fs::SimpleRendering::DxBufferType::ConstantBuffer:
			{
				ComPtr<ID3D11Buffer> newInternalBuffer;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = 0;
				bufferDescriptor.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = bufferContent;

				if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, (nullptr != bufferContent) ? &subresourceData : nullptr, newInternalBuffer.ReleaseAndGetAddressOf())))
				{
					_bufferSize = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementCount = elementCount;

					std::swap(_internalBuffer, newInternalBuffer);
					return true;
				}
				break;
			}
			case fs::SimpleRendering::DxBufferType::VertexBuffer:
			case fs::SimpleRendering::DxBufferType::IndexBuffer:
			{
				ComPtr<ID3D11Buffer> newInternalBuffer;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = (_bufferType == DxBufferType::VertexBuffer) ? D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = 0;
				bufferDescriptor.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = bufferContent;

				if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, (nullptr != bufferContent) ? &subresourceData : nullptr, newInternalBuffer.ReleaseAndGetAddressOf())))
				{
					_bufferSize = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementCount = elementCount;

					std::swap(_internalBuffer, newInternalBuffer);
					return true;
				}
				break;
			}
			case fs::SimpleRendering::DxBufferType::StructuredBuffer:
			{
				ComPtr<ID3D11Buffer> newInternalBuffer;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
				bufferDescriptor.CPUAccessFlags = 0;
				bufferDescriptor.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				bufferDescriptor.StructureByteStride = elementStride;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = bufferContent;

				if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateBuffer(&bufferDescriptor, (nullptr != bufferContent) ? &subresourceData : nullptr, newInternalBuffer.ReleaseAndGetAddressOf())))
				{
					_bufferSize = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementCount = elementCount;

					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
					shaderResourceViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
					shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
					shaderResourceViewDescriptor.Buffer.ElementOffset = 0;
					shaderResourceViewDescriptor.Buffer.ElementWidth = elementCount;
					if (SUCCEEDED(_graphicDevice->getDxDevice()->CreateShaderResourceView(newInternalBuffer.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
					{
						std::swap(_internalBuffer, newInternalBuffer);
						return true;
					}
				}
				break;
			}
			default:
				break;
			}
			return false;
		}

		const bool DxBuffer::isValid() const noexcept
		{
			return (_internalBuffer.Get() != nullptr);
		}

		void DxBuffer::updateContent(const byte* const bufferContent)
		{
			updateContent(bufferContent, _elementStride, _elementCount);
		}

		void DxBuffer::updateContent(const byte* const bufferContent, const uint32 elementCount)
		{
			updateContent(bufferContent, _elementStride, elementCount);
		}

		void DxBuffer::updateContent(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount)
		{
			if (bufferContent == nullptr)
			{
				FS_LOG_ERROR("김장원", "nullptr 인 데이터가 들어오면 안 됩니다!");
				return;
			}

			const bool needToReset = (_elementStride != elementStride) || (_elementCount < elementCount);
			if (needToReset == true)
			{
				// elementStride => 자료형이 달라진 경우
				// elementCount => 데이터가 늘어난 경우
				// 버퍼 재생성
				create(bufferContent, elementStride, elementCount);
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE mappedVertexResource{};
				if (_graphicDevice->getDxDeviceContext()->Map(_internalBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &mappedVertexResource) == S_OK)
				{
					memcpy(mappedVertexResource.pData, bufferContent, _bufferSize);

					_graphicDevice->getDxDeviceContext()->Unmap(_internalBuffer.Get(), 0);
				}
			}
		}

		void DxBuffer::setOffset(const uint32 elementOffset)
		{
			_elementOffset = elementOffset;
		}

		void DxBuffer::bind() const noexcept
		{
			if (_bufferType == DxBufferType::VertexBuffer)
			{
				_graphicDevice->getDxDeviceContext()->IASetVertexBuffers(0, 1, _internalBuffer.GetAddressOf(), &_elementStride, &_elementOffset);
			}
			else if (_bufferType == DxBufferType::IndexBuffer)
			{
				_graphicDevice->getDxDeviceContext()->IASetIndexBuffer(_internalBuffer.Get(), kIndexBufferFormat, _elementOffset);
			}
		}

		void DxBuffer::bindToShader(const DxShaderType shaderType, const uint32 bindSlot) const noexcept
		{
			if (_bufferType == DxBufferType::ConstantBuffer)
			{
				if (shaderType == DxShaderType::VertexShader)
				{
					_graphicDevice->getDxDeviceContext()->VSSetConstantBuffers(bindSlot, 1, _internalBuffer.GetAddressOf());
				}
				else if (shaderType == DxShaderType::PixelShader)
				{
					_graphicDevice->getDxDeviceContext()->PSSetConstantBuffers(bindSlot, 1, _internalBuffer.GetAddressOf());
				}
				else
				{
					FS_LOG_ERROR("김장원", "미지원 ShaderType 입니다!");
				}
			}
		}


		DxBufferPool::DxBufferPool(GraphicDevice* const graphicDevice)
			: IDxObject(graphicDevice, DxObjectType::Pool)
		{
			__noop;
		}

		const DxObjectId& DxBufferPool::pushConstantBuffer(const byte* const bufferContent, const uint32 bufferSize)
		{
			DxBuffer buffer{ _graphicDevice };
			buffer._bufferType = DxBufferType::ConstantBuffer;
			if (buffer.create(bufferContent, bufferSize, 1) == true)
			{
				buffer.assignIdXXX();
				_bufferArray.emplace_back(std::move(buffer));
				return _bufferArray.back().getId();
			}

			FS_ASSERT("김장원", false, "pushConstantBuffer 에 실패했습니다!");
			return DxObjectId::kInvalidObjectId;
		}

		const DxObjectId& DxBufferPool::pushVertexBuffer(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount)
		{
			DxBuffer buffer{ _graphicDevice };
			buffer._bufferType = DxBufferType::VertexBuffer;
			if (buffer.create(bufferContent, elementStride, elementCount) == true)
			{
				buffer.assignIdXXX();
				_bufferArray.emplace_back(std::move(buffer));
				return _bufferArray.back().getId();
			}

			FS_ASSERT("김장원", false, "pushVertexBuffer 에 실패했습니다!");
			return DxObjectId::kInvalidObjectId;
		}

		const DxObjectId& DxBufferPool::pushIndexBuffer(const byte* const bufferContent, const uint32 elementCount)
		{
			DxBuffer buffer{ _graphicDevice };
			buffer._bufferType = DxBufferType::IndexBuffer;
			if (buffer.create(bufferContent, DxBuffer::kIndexBufferElementStride, elementCount) == true)
			{
				buffer.assignIdXXX();
				_bufferArray.emplace_back(std::move(buffer));
				return _bufferArray.back().getId();
			}

			FS_ASSERT("김장원", false, "pushIndexBuffer 에 실패했습니다!");
			return DxObjectId::kInvalidObjectId;
		}

		const DxObjectId& DxBufferPool::pushStructuredBuffer(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount)
		{
			DxBuffer buffer{ _graphicDevice };
			buffer._bufferType = DxBufferType::IndexBuffer;
			if (buffer.create(bufferContent, elementStride, elementCount) == true)
			{
				buffer.assignIdXXX();
				_bufferArray.emplace_back(std::move(buffer));
				return _bufferArray.back().getId();
			}

			FS_ASSERT("김장원", false, "pushStructuredBuffer 에 실패했습니다!");
			return DxObjectId::kInvalidObjectId;
		}

		DxBuffer& DxBufferPool::getBuffer(const DxObjectId& objectId)
		{
			FS_ASSERT("김장원", objectId.isObjectType(DxObjectType::Buffer) == true, "Invalid parameter - ObjectType !!");

			const int32 index = fs::binarySearch(_bufferArray, objectId);
			if (0 <= index)
			{
				return _bufferArray[index];
			}

			FS_ASSERT("김장원", false, "Buffer 를 찾지 못했습니다!!!");
			return DxBuffer::s_invalidInstance;
		}
	}
}
