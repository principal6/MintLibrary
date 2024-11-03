#include <MintRenderingBase/Include/GraphicsResource.h>

#include <MintContainer/Include/Vector.hpp>
#include <MintContainer/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/GraphicsDevice.h>


namespace mint
{
	namespace Rendering
	{
		GraphicsResource GraphicsResource::s_invalidInstance(GraphicsDevice::GetInvalidInstance());
		DXGI_FORMAT GraphicsResource::GetDXGIFormat(const TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8_UNORM:
				return DXGI_FORMAT::DXGI_FORMAT_R8_UNORM;
			case TextureFormat::R8G8B8A8_UNORM:
				return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			default:
				break;
			}
			MINT_ASSERT(false, "미지원 Texture Format 입니다!!!");
			return DXGI_FORMAT();
		}

		uint32 GraphicsResource::GetColorCount(const TextureFormat format)
		{
			switch (format)
			{
			case TextureFormat::R8_UNORM:
				return 1;
			case TextureFormat::R8G8B8A8_UNORM:
				return 4;
			default:
				break;
			}
			MINT_ASSERT(false, "미지원 Texture Format 입니다!!!");
			return 0;
		}


		GraphicsResource::GraphicsResource(GraphicsDevice& graphicsDevice)
			: GraphicsObject(graphicsDevice, GraphicsObjectType::Resource)
			, _resourceType{ GraphicsResourceType::INVALID }
			, _resourceCapacity{ 0 }
			, _elementStride{ 0 }
			, _elementMaxCount{ 0 }
			, _elementOffset{ 0 }
			, _textureFormat{ TextureFormat::INVALID }
			, _textureWidth{ 0 }
			, _textureHeight{ 0 }
			, _registerIndex{ 0 }
			, _needToBind{ true }
			, _boundSlots{}
		{
			for (GraphicsShaderType shaderType = GraphicsShaderType(0); shaderType != GraphicsShaderType::COUNT; shaderType = (GraphicsShaderType)((uint32)shaderType + 1))
			{
				const uint32 i = (uint32)shaderType;
				_boundSlots[i] = kUint32Max;
			}
		}

		bool GraphicsResource::CreateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			switch (_resourceType)
			{
			case GraphicsResourceType::INVALID:
				break;
			case GraphicsResourceType::ConstantBuffer:
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

				if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					std::swap(_resource, newResource);
					return true;
				}
				break;
			}
			case GraphicsResourceType::VertexBuffer:
			case GraphicsResourceType::IndexBuffer:
			{
				ComPtr<ID3D11Resource> newResource;

				D3D11_BUFFER_DESC bufferDescriptor{};
				bufferDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DYNAMIC;
				bufferDescriptor.ByteWidth = elementStride * elementCount;
				bufferDescriptor.BindFlags = (_resourceType == GraphicsResourceType::VertexBuffer) ? D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER : D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
				bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_WRITE;
				bufferDescriptor.MiscFlags = 0;
				bufferDescriptor.StructureByteStride = 0;

				D3D11_SUBRESOURCE_DATA subresourceData{};
				subresourceData.pSysMem = resourceContent;

				if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					std::swap(_resource, newResource);
					return true;
				}
				break;
			}
			case GraphicsResourceType::StructuredBuffer:
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

				if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
				{
					_resourceCapacity = bufferDescriptor.ByteWidth;
					_elementStride = elementStride;
					_elementMaxCount = elementCount;

					D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
					shaderResourceViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
					shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
					shaderResourceViewDescriptor.Buffer.ElementOffset = 0;
					shaderResourceViewDescriptor.Buffer.ElementWidth = elementCount;
					if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
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

		bool GraphicsResource::CreateTexture(const TextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height)
		{
			switch (_resourceType)
			{
			case GraphicsResourceType::Texture2D:
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

				if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
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
					if (SUCCEEDED(_graphicsDevice.GetDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
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

		bool GraphicsResource::IsValid() const noexcept
		{
			return (_resource.Get() != nullptr);
		}

		void GraphicsResource::UpdateBuffer(const void* const resourceContent, const uint32 elementCount)
		{
			UpdateBuffer(resourceContent, _elementStride, elementCount);
		}

		void GraphicsResource::UpdateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			MINT_ASSERT(_resourceType < GraphicsResourceType::Texture2D, "");

			UpdateContentInternal(resourceContent, elementStride, elementCount, 0);
		}

		void GraphicsResource::UpdateTexture(const void* const resourceContent)
		{
			UpdateTexture(resourceContent, _textureWidth, _textureHeight);
		}

		void GraphicsResource::UpdateTexture(const void* const resourceContent, const uint32 width, const uint32 height)
		{
			MINT_ASSERT(GraphicsResourceType::Texture2D <= _resourceType, "");

			UpdateContentInternal(resourceContent, _elementStride, width * height, width);
		}

		void GraphicsResource::UpdateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width)
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

				if (_resourceType < GraphicsResourceType::Texture2D)
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
				if (_resourceType >= GraphicsResourceType::Texture2D)
				{
					_graphicsDevice.GetDxDeviceContext()->UpdateSubresource(_resource.Get(), 0, nullptr, resourceContent, _elementStride * width, 0);
				}
				else
				{
					SafeResourceMapper safeResourceMapper{ _graphicsDevice, _resource.Get(), 0 };
					if (safeResourceMapper.IsValid() == true)
					{
						safeResourceMapper.Set(resourceContent, _elementStride * elementCount);
					}
				}
			}
		}

		void GraphicsResource::SetOffset(const uint32 elementOffset)
		{
			_elementOffset = elementOffset;
		}

		uint32 GraphicsResource::GetRegisterIndex() const noexcept
		{
			return _registerIndex;
		}

		ID3D11Buffer* const* GraphicsResource::GetBuffer() const noexcept
		{
			return reinterpret_cast<ID3D11Buffer* const*>(_resource.GetAddressOf());
		}

		ID3D11ShaderResourceView* const* GraphicsResource::GetResourceView() const noexcept
		{
			return reinterpret_cast<ID3D11ShaderResourceView* const*>(_view.GetAddressOf());
		}

		bool GraphicsResource::NeedsToBind() const noexcept
		{
			return _needToBind;
		}

		void GraphicsResource::BindAsInput() const noexcept
		{
			if (_resourceType == GraphicsResourceType::VertexBuffer)
			{
				_graphicsDevice.GetStateManager().SetIAVertexBuffers(0, 1, reinterpret_cast<ID3D11Buffer* const*>(_resource.GetAddressOf()), &_elementStride, &_elementOffset);
			}
			else if (_resourceType == GraphicsResourceType::IndexBuffer)
			{
				_graphicsDevice.GetStateManager().SetIAIndexBuffer(reinterpret_cast<ID3D11Buffer*>(_resource.Get()), kIndexBufferFormat, _elementOffset);
			}
			else
			{
				MINT_LOG_ERROR("BindToShader 를 호출해야 합니다!");
			}

			_needToBind = false;
		}

		void GraphicsResource::BindToShader(const GraphicsShaderType shaderType, const uint32 bindingSlot) const noexcept
		{
			if (_resourceType == GraphicsResourceType::ConstantBuffer)
			{
				_graphicsDevice.GetStateManager().SetConstantBuffers(shaderType, this, bindingSlot);
			}
			else if (_resourceType == GraphicsResourceType::StructuredBuffer || GraphicsResourceType::Texture2D <= _resourceType)
			{
				_graphicsDevice.GetStateManager().SetShaderResources(shaderType, this, bindingSlot);
			}
			else
			{
				MINT_LOG_ERROR("bindAsInpt 을 호출해야 합니다!");
			}

			_boundSlots[static_cast<uint32>(shaderType)] = bindingSlot;
			_needToBind = false;
		}

		void GraphicsResource::UnbindFromShader() const noexcept
		{
			for (GraphicsShaderType shaderType = GraphicsShaderType(0); shaderType != GraphicsShaderType::COUNT; shaderType = (GraphicsShaderType)((uint32)shaderType + 1))
			{
				const uint32 i = (uint32)shaderType;
				if (_boundSlots[i] == kUint32Max)
				{
					continue;
				}

				if (_resourceType == GraphicsResourceType::ConstantBuffer)
				{
					_graphicsDevice.GetStateManager().SetConstantBuffers(shaderType, nullptr, _boundSlots[i]);
				}
				else if (_resourceType == GraphicsResourceType::StructuredBuffer || GraphicsResourceType::Texture2D <= _resourceType)
				{
					_graphicsDevice.GetStateManager().SetShaderResources(shaderType, nullptr, _boundSlots[i]);
				}

				_boundSlots[i] = kUint32Max;
			}
		
			_needToBind = true;
		}


		GraphicsResourcePool::GraphicsResourcePool(GraphicsDevice& graphicsDevice)
			: GraphicsObject(graphicsDevice, GraphicsObjectType::Pool)
		{
			__noop;
		}

		GraphicsObjectID GraphicsResourcePool::AddConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex)
		{
			GraphicsResource resource{ _graphicsDevice };
			resource._resourceType = GraphicsResourceType::ConstantBuffer;
			if (resource.CreateBuffer(resourceContent, bufferSize, 1) == true)
			{
				resource.AssignIDXXX();
				resource._registerIndex = registerIndex;

				const GraphicsObjectID graphicsObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicsObject::AscendingComparator());
				return graphicsObjectID;
			}
			MINT_ASSERT(false, "AddConstantBuffer 에 실패했습니다!");
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		GraphicsObjectID GraphicsResourcePool::AddVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
		{
			GraphicsResource resource{ _graphicsDevice };
			resource._resourceType = GraphicsResourceType::VertexBuffer;
			if (resource.CreateBuffer(resourceContent, elementStride, elementCount) == true)
			{
				resource.AssignIDXXX();

				const GraphicsObjectID graphicsObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicsObject::AscendingComparator());
				return graphicsObjectID;
			}

			MINT_ASSERT(false, "AddVertexBuffer 에 실패했습니다!");
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		GraphicsObjectID GraphicsResourcePool::AddIndexBuffer(const void* const resourceContent, const uint32 elementCount)
		{
			GraphicsResource resource{ _graphicsDevice };
			resource._resourceType = GraphicsResourceType::IndexBuffer;
			if (resource.CreateBuffer(resourceContent, GraphicsResource::kIndexBufferElementStride, elementCount) == true)
			{
				resource.AssignIDXXX();

				const GraphicsObjectID graphicsObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicsObject::AscendingComparator());
				return graphicsObjectID;
			}

			MINT_ASSERT(false, "AddIndexBuffer 에 실패했습니다!");
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		GraphicsObjectID GraphicsResourcePool::AddStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex)
		{
			GraphicsResource resource{ _graphicsDevice };
			resource._resourceType = GraphicsResourceType::StructuredBuffer;
			if (resource.CreateBuffer(resourceContent, elementStride, elementCount) == true)
			{
				resource.AssignIDXXX();
				resource._registerIndex = registerIndex;

				const GraphicsObjectID graphicsObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicsObject::AscendingComparator());
				return graphicsObjectID;
			}

			MINT_ASSERT(false, "AddStructuredBuffer 에 실패했습니다!");
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		GraphicsObjectID GraphicsResourcePool::AddTexture2D(const ByteColorImage& byteColorImage)
		{
			return AddTexture2D(TextureFormat::R8G8B8A8_UNORM, byteColorImage.GetBytes(), byteColorImage.GetWidth(), byteColorImage.GetHeight());
		}

		GraphicsObjectID GraphicsResourcePool::AddTexture2D(const TextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height)
		{
			GraphicsResource resource{ _graphicsDevice };
			resource._resourceType = GraphicsResourceType::Texture2D;
			if (resource.CreateTexture(format, textureContent, width, height) == true)
			{
				resource.AssignIDXXX();

				const GraphicsObjectID graphicsObjectID = resource.GetID();
				_resourceArray.PushBack(std::move(resource));
				QuickSort(_resourceArray, GraphicsObject::AscendingComparator());
				return graphicsObjectID;
			}

			MINT_ASSERT(false, "AddTexture2D 에 실패했습니다!");
			return GraphicsObjectID::kInvalidGraphicsObjectID;
		}

		void GraphicsResourcePool::BindAsInput(const GraphicsObjectID& objectID) noexcept
		{
			GraphicsResource& resource = GetResource(objectID);
			if (resource.IsValid() == true)
			{
				resource.BindAsInput();
			}
		}

		void GraphicsResourcePool::BindToShader(const GraphicsObjectID& objectID, const GraphicsShaderType shaderType, const uint32 bindingSlot) noexcept
		{
			GraphicsResource& resource = GetResource(objectID);
			if (resource.IsValid() == true)
			{
				resource.BindToShader(shaderType, bindingSlot);
			}
		}

		GraphicsResource& GraphicsResourcePool::GetResource(const GraphicsObjectID& objectID)
		{
			MINT_ASSERT(objectID.IsObjectType(GraphicsObjectType::Resource) == true, "Invalid parameter - ObjectType !!");

			const int32 index = BinarySearch(_resourceArray, objectID, GraphicsObject::Evaluator());
			if (IsValidIndex(index) == false)
			{
				MINT_ASSERT(false, "Resource 를 찾지 못했습니다!!!");
				return GraphicsResource::s_invalidInstance;
			}
			return _resourceArray[index];
		}
	}
}
