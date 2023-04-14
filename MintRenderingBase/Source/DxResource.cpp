#include <MintRenderingBase/Include/DxResource.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>


namespace mint
{
	namespace Rendering
	{
		DxResource DxResource::s_invalidInstance(GraphicDevice::GetInvalidInstance());
		DXGI_FORMAT DxResource::GetDXGIFormat(const DxTextureFormat format)
		{
			switch (format)
			{
			case DxTextureFormat::R8_UNORM:
				return DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
			case DxTextureFormat::R8G8B8A8_UNORM:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			default:
				break;
			}
			MINT_ASSERT(false, "미지원 Texture Format 입니다!!!");
			return DXGI_FORMAT();
		}

		uint32 DxResource::GetColorCount(const DxTextureFormat format)
		{
			switch (format)
			{
			case DxTextureFormat::R8_UNORM:
				return 1;
			case DxTextureFormat::R8G8B8A8_UNORM:
				return 4;
			default:
				break;
			}
			MINT_ASSERT(false, "미지원 Texture Format 입니다!!!");
			return 0;
		}


		DxResource::DxResource(GraphicDevice& graphicDevice)
			: GraphicObject(graphicDevice, GraphicObjectType::Resource)
			, _resourceType{ DxResourceType::INVALID }
			, _resourceCapacity{ 0 }
			, _elementStride{ 0 }
			, _elementMaxCount{ 0 }
			, _elementOffset{ 0 }
			, _textureFormat{ DxTextureFormat::INVALID }
			, _textureWidth{ 0 }
			, _textureHeight{ 0 }
			, _registerIndex{ 0 }
			, _needToBind{ true }
		{
			__noop;
		}

		bool DxResource::CreateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			switch (_resourceType)
			{
			case DxResourceType::INVALID:
				break;
			case DxResourceType::ConstantBuffer:
			{
				ComPtr<ID3D11Resource> newResource;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = 0;
				bufferDescriptor.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = resourceContent;

				if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					std::swap(_resource, newResource);
					return true;
				}
				break;
			}
			case DxResourceType::VertexBuffer:
			case DxResourceType::IndexBuffer:
			{
				ComPtr<ID3D11Resource> newResource;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = (_resourceType == DxResourceType::VertexBuffer) ? D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = 0;
				bufferDescriptor.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = resourceContent;

				if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					std::swap(_resource, newResource);
					return true;
				}
				break;
			}
			case DxResourceType::StructuredBuffer:
			{
				ComPtr<ID3D11Resource> newResource;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = D3D11_RESOURCE_MISC_FLAG::D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
				bufferDescriptor.StructureByteStride = elementStride;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = resourceContent;

				if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
					shaderResourceViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
					shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
					shaderResourceViewDescriptor.Buffer.ElementOffset = 0;
					shaderResourceViewDescriptor.Buffer.ElementWidth = elementCount;
					if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
					{
						std::swap(_resource, newResource);
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

		bool DxResource::CreateTexture(const DxTextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height)
		{
			switch (_resourceType)
			{
			case DxResourceType::Texture2D:
			{
				ComPtr<ID3D11Resource> newResource;

				D3D11_TEXTURE2D_DESC texture2DDescriptor{};
				texture2DDescriptor.Width = width;
				texture2DDescriptor.Height = height;
				texture2DDescriptor.MipLevels = 1;
				texture2DDescriptor.ArraySize = 1;
				texture2DDescriptor.Format = GetDXGIFormat(format);
				texture2DDescriptor.SampleDesc.Count = 1;
				texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
				texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
				texture2DDescriptor.CPUAccessFlags = 0;

				const uint32 colorCount = GetColorCount(format);
				D3D11_SUBRESOURCE_DATA subResource{};
				subResource.pSysMem = resourceContent;
				subResource.SysMemPitch = texture2DDescriptor.Width * colorCount;
				subResource.SysMemSlicePitch = 0;

				if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_elementStride = colorCount;
					_elementMaxCount = texture2DDescriptor.Width * texture2DDescriptor.Height;
					_resourceCapacity = _elementStride * _elementMaxCount;

					_textureFormat = format;
					_textureWidth = width;
					_textureHeight = height;

					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
					shaderResourceViewDescriptor.Format = texture2DDescriptor.Format;
					shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
					shaderResourceViewDescriptor.Texture2D.MipLevels = texture2DDescriptor.MipLevels;
					shaderResourceViewDescriptor.Texture2D.MostDetailedMip = 0; // TODO?
					if (SUCCEEDED(_graphicDevice.GetDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
					{
						std::swap(_resource, newResource);
						return true;
					}
				}
				break;
			}
			}
			return false;
		}

		bool DxResource::IsValid() const noexcept
		{
			return (_resource.Get() != nullptr);
		}

		void DxResource::UpdateBuffer(const void* const resourceContent, const uint32 elementCount)
		{
			UpdateBuffer(resourceContent, _elementStride, elementCount);
		}

		void DxResource::UpdateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			MINT_ASSERT(_resourceType < DxResourceType::Texture2D, "");

			UpdateContentInternal(resourceContent, elementStride, elementCount, 0);
		}

		void DxResource::UpdateTexture(const void* const resourceContent)
		{
			UpdateTexture(resourceContent, _textureWidth, _textureHeight);
		}

		void DxResource::UpdateTexture(const void* const resourceContent, const uint32 width, const uint32 height)
		{
			MINT_ASSERT(DxResourceType::Texture2D <= _resourceType, "");

			UpdateContentInternal(resourceContent, _elementStride, width * height, width);
		}

		void DxResource::UpdateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width)
		{
			if (resourceContent == nullptr)
			{
				MINT_LOG_ERROR("nullptr 인 데이터가 들어오면 안 됩니다!");
				return;
			}

			const bool needToReset = (_elementStride != elementStride) || (_elementMaxCount < elementCount);
			if (needToReset == true)
			{
				// elementStride => 자료형이 달라진 경우
				// elementCount => 데이터가 늘어난 경우
				// 버퍼 재생성

				if (_resourceType < DxResourceType::Texture2D)
				{
					CreateBuffer(resourceContent, elementStride, elementCount);
				}
				else
				{
					if (width == 0)
					{
						MINT_LOG_ERROR("Texture 의 Width 가 0 이면 안 됩니다!!!");
						return;
					}

					CreateTexture(_textureFormat, resourceContent, width, elementCount / width);
				}

				_needToBind = true;
			}
			else
			{
				if (_resourceType >= DxResourceType::Texture2D)
				{
					_graphicDevice.GetDxDeviceContext()->UpdateSubresource(_resource.Get(), 0, nullptr, resourceContent, _elementStride * width, 0);
				}
				else
				{
					SafeResourceMapper safeResourceMapper{ _graphicDevice, _resource.Get(), 0 };
					if (safeResourceMapper.IsValid() == true)
					{
						safeResourceMapper.Set(resourceContent, _elementStride * elementCount);
					}
				}
			}
		}

		void DxResource::SetOffset(const uint32 elementOffset)
		{
			_elementOffset = elementOffset;
		}

		uint32 DxResource::GetRegisterIndex() const noexcept
		{
			return _registerIndex;
		}

		ID3D11Buffer* const* DxResource::GetBuffer() const noexcept
		{
			return reinterpret_cast<ID3D11Buffer* const*>(_resource.GetAddressOf());
		}

		ID3D11ShaderResourceView* const* DxResource::GetResourceView() const noexcept
		{
			return reinterpret_cast<ID3D11ShaderResourceView* const*>(_view.GetAddressOf());
		}

		bool DxResource::NeedsToBind() const noexcept
		{
			return _needToBind;
		}

		void DxResource::BindAsInput() const noexcept
		{
			if (_resourceType == DxResourceType::VertexBuffer)
			{
				_graphicDevice.GetStateManager().SetIAVertexBuffers(0, 1, reinterpret_cast<ID3D11Buffer* const*>(_resource.GetAddressOf()), &_elementStride, &_elementOffset);
			}
			else if (_resourceType == DxResourceType::IndexBuffer)
			{
				_graphicDevice.GetStateManager().SetIAIndexBuffer(reinterpret_cast<ID3D11Buffer*>(_resource.Get()), kIndexBufferFormat, _elementOffset);
			}
			else
			{
				MINT_LOG_ERROR("BindToShader 를 호출해야 합니다!");
			}

			_needToBind = false;
		}

		void DxResource::BindToShader(const GraphicShaderType shaderType, const uint32 bindingSlot) const noexcept
		{
			if (_resourceType == DxResourceType::ConstantBuffer)
			{
				if (shaderType == GraphicShaderType::VertexShader)
				{
					_graphicDevice.GetStateManager().SetVSConstantBuffers(*this, bindingSlot);
				}
				else if (shaderType == GraphicShaderType::GeometryShader)
				{
					_graphicDevice.GetStateManager().SetGSConstantBuffers(*this, bindingSlot);
				}
				else if (shaderType == GraphicShaderType::PixelShader)
				{
					_graphicDevice.GetStateManager().SetPSConstantBuffers(*this, bindingSlot);
				}
				else
				{
					MINT_LOG_ERROR("미지원 ShaderType 입니다!");
				}
			}
			else if (_resourceType == DxResourceType::StructuredBuffer || DxResourceType::Texture2D <= _resourceType)
			{
				if (shaderType == GraphicShaderType::VertexShader)
				{
					_graphicDevice.GetStateManager().SetVSResources(*this, bindingSlot);
				}
				else if (shaderType == GraphicShaderType::GeometryShader)
				{
					_graphicDevice.GetStateManager().SetGSResources(*this, bindingSlot);
				}
				else if (shaderType == GraphicShaderType::PixelShader)
				{
					_graphicDevice.GetStateManager().SetPSResources(*this, bindingSlot);
				}
				else
				{
					MINT_LOG_ERROR("미지원 ShaderType 입니다!");
				}
			}
			else
			{
				MINT_LOG_ERROR("bindAsInpt 을 호출해야 합니다!");
			}

			_needToBind = false;
		}


		DxResourcePool::DxResourcePool(GraphicDevice& graphicDevice)
			: GraphicObject(graphicDevice, GraphicObjectType::Pool)
		{
			__noop;
		}

		GraphicObjectID DxResourcePool::AddConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex)
		{
			DxResource resource{ _graphicDevice };
			resource._resourceType = DxResourceType::ConstantBuffer;
			if (resource.CreateBuffer(resourceContent, bufferSize, 1) == true)
			{
				resource.AssignIDXXX();
				resource._registerIndex = registerIndex;

				const GraphicObjectID graphicObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicObject::AscendingComparator());
				return graphicObjectID;
			}
			MINT_ASSERT(false, "AddConstantBuffer 에 실패했습니다!");
			return GraphicObjectID::kInvalidGraphicObjectID;
		}

		GraphicObjectID DxResourcePool::AddVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			DxResource resource{ _graphicDevice };
			resource._resourceType = DxResourceType::VertexBuffer;
			if (resource.CreateBuffer(resourceContent, elementStride, elementCount) == true)
			{
				resource.AssignIDXXX();

				const GraphicObjectID graphicObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicObject::AscendingComparator());
				return graphicObjectID;
			}

			MINT_ASSERT(false, "AddVertexBuffer 에 실패했습니다!");
			return GraphicObjectID::kInvalidGraphicObjectID;
		}

		GraphicObjectID DxResourcePool::AddIndexBuffer(const void* const resourceContent, const uint32 elementCount)
		{
			DxResource resource{ _graphicDevice };
			resource._resourceType = DxResourceType::IndexBuffer;
			if (resource.CreateBuffer(resourceContent, DxResource::kIndexBufferElementStride, elementCount) == true)
			{
				resource.AssignIDXXX();

				const GraphicObjectID graphicObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicObject::AscendingComparator());
				return graphicObjectID;
			}

			MINT_ASSERT(false, "AddIndexBuffer 에 실패했습니다!");
			return GraphicObjectID::kInvalidGraphicObjectID;
		}

		GraphicObjectID DxResourcePool::AddStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex)
		{
			DxResource resource{ _graphicDevice };
			resource._resourceType = DxResourceType::StructuredBuffer;
			if (resource.CreateBuffer(resourceContent, elementStride, elementCount) == true)
			{
				resource.AssignIDXXX();
				resource._registerIndex = registerIndex;

				const GraphicObjectID graphicObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicObject::AscendingComparator());
				return graphicObjectID;
			}

			MINT_ASSERT(false, "AddStructuredBuffer 에 실패했습니다!");
			return GraphicObjectID::kInvalidGraphicObjectID;
		}

		GraphicObjectID DxResourcePool::AddTexture2D(const DxTextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height)
		{
			DxResource resource{ _graphicDevice };
			resource._resourceType = DxResourceType::Texture2D;
			if (resource.CreateTexture(format, textureContent, width, height) == true)
			{
				resource.AssignIDXXX();
				
				const GraphicObjectID graphicObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicObject::AscendingComparator());
				return graphicObjectID;
			}

			MINT_ASSERT(false, "AddTexture2D 에 실패했습니다!");
			return GraphicObjectID::kInvalidGraphicObjectID;
		}

		void DxResourcePool::BindAsInput(const GraphicObjectID& objectID) noexcept
		{
			DxResource& resource = GetResource(objectID);
			if (resource.IsValid() == true)
			{
				resource.BindAsInput();
			}
		}

		void DxResourcePool::BindToShader(const GraphicObjectID& objectID, const GraphicShaderType shaderType, const uint32 bindingSlot) noexcept
		{
			DxResource& resource = GetResource(objectID);
			if (resource.IsValid() == true)
			{
				resource.BindToShader(shaderType, bindingSlot);
			}
		}

		DxResource& DxResourcePool::GetResource(const GraphicObjectID& objectID)
		{
			MINT_ASSERT(objectID.IsObjectType(GraphicObjectType::Resource) == true, "Invalid parameter - ObjectType !!");

			const int32 index = BinarySearch(_resourceArray, objectID, GraphicObject::Evaluator());
			if (IsValidIndex(index) == false)
			{
				MINT_ASSERT(false, "Resource 를 찾지 못했습니다!!!");
				return DxResource::s_invalidInstance;
			}
			return _resourceArray[index];
		}
	}
}
