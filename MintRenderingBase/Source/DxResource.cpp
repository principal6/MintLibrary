#include <stdafx.h>
#include <MintRenderingBase/Include/DxResource.h>

#include <MintContainer/Include/Vector.hpp>

#include <MintLibrary/Include/Algorithm.hpp>

#include <MintRenderingBase/Include/GraphicDevice.h>


namespace mint
{
    namespace Rendering
    {
        DxResource DxResource::s_invalidInstance(GraphicDevice::getInvalidInstance());
        DXGI_FORMAT DxResource::getDxgiFormat(const DxTextureFormat format)
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
            MINT_ASSERT("�����", false, "������ Texture Format �Դϴ�!!!");
            return DXGI_FORMAT();
        }

        const uint32 DxResource::getColorCount(const DxTextureFormat format)
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
            MINT_ASSERT("�����", false, "������ Texture Format �Դϴ�!!!");
            return 0;
        }


        DxResource::DxResource(GraphicDevice& graphicDevice)
            : IDxObject(graphicDevice, DxObjectType::Resource)
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

        const bool DxResource::createBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
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

                if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
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

                if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
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

                if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateBuffer(&bufferDescriptor, (resourceContent != nullptr) ? &subresourceData : nullptr, reinterpret_cast<ID3D11Buffer**>(newResource.ReleaseAndGetAddressOf()))))
                {
                    _resourceCapacity = bufferDescriptor.ByteWidth;
                    _elementStride = elementStride;
                    _elementMaxCount = elementCount;

                    D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDescriptor;
                    shaderResourceViewDescriptor.Format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
                    shaderResourceViewDescriptor.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_BUFFER;
                    shaderResourceViewDescriptor.Buffer.ElementOffset = 0;
                    shaderResourceViewDescriptor.Buffer.ElementWidth = elementCount;
                    if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
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

        const bool DxResource::createTexture(const DxTextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height)
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
                texture2DDescriptor.Format = getDxgiFormat(format);
                texture2DDescriptor.SampleDesc.Count = 1;
                texture2DDescriptor.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
                texture2DDescriptor.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
                texture2DDescriptor.CPUAccessFlags = 0;

                const uint32 colorCount = getColorCount(format);
                D3D11_SUBRESOURCE_DATA subResource{};
                subResource.pSysMem = resourceContent;
                subResource.SysMemPitch = texture2DDescriptor.Width * colorCount;
                subResource.SysMemSlicePitch = 0;

                if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateTexture2D(&texture2DDescriptor, &subResource, reinterpret_cast<ID3D11Texture2D**>(newResource.ReleaseAndGetAddressOf()))))
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
                    if (SUCCEEDED(_graphicDevice.getDxDevice()->CreateShaderResourceView(newResource.Get(), &shaderResourceViewDescriptor, reinterpret_cast<ID3D11ShaderResourceView**>(_view.ReleaseAndGetAddressOf()))))
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

        const bool DxResource::isValid() const noexcept
        {
            return (_resource.Get() != nullptr);
        }

        void DxResource::updateBuffer(const void* const resourceContent, const uint32 elementCount)
        {
            updateBuffer(resourceContent, _elementStride, elementCount);
        }

        void DxResource::updateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
        {
            MINT_ASSERT("�����", _resourceType < DxResourceType::Texture2D, "");

            updateContentInternal(resourceContent, elementStride, elementCount, 0);
        }

        void DxResource::updateTexture(const void* const resourceContent)
        {
            updateTexture(resourceContent, _textureWidth, _textureHeight);
        }

        void DxResource::updateTexture(const void* const resourceContent, const uint32 width, const uint32 height)
        {
            MINT_ASSERT("�����", DxResourceType::Texture2D <= _resourceType, "");

            updateContentInternal(resourceContent, _elementStride, width * height, width);
        }

        void DxResource::updateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width)
        {
            if (resourceContent == nullptr)
            {
                MINT_LOG_ERROR("�����", "nullptr �� �����Ͱ� ������ �� �˴ϴ�!");
                return;
            }

            const bool needToReset = (_elementStride != elementStride) || (_elementMaxCount < elementCount);
            if (needToReset == true)
            {
                // elementStride => �ڷ����� �޶��� ���
                // elementCount => �����Ͱ� �þ ���
                // ���� �����

                if (_resourceType < DxResourceType::Texture2D)
                {
                    createBuffer(resourceContent, elementStride, elementCount);
                }
                else
                {
                    if (width == 0)
                    {
                        MINT_LOG_ERROR("�����", "Texture �� Width �� 0 �̸� �� �˴ϴ�!!!");
                        return;
                    }

                    createTexture(_textureFormat, resourceContent, width, elementCount / width);
                }

                _needToBind = true;
            }
            else
            {
                SafeResourceMapper safeResourceMapper{ _graphicDevice, _resource.Get(), 0 };
                if (safeResourceMapper.isValid() == true)
                {
                    safeResourceMapper.set(resourceContent, _elementStride * elementCount);
                }
            }
        }

        void DxResource::setOffset(const uint32 elementOffset)
        {
            _elementOffset = elementOffset;
        }

        const uint32 DxResource::getRegisterIndex() const noexcept
        {
            return _registerIndex;
        }

        ID3D11Buffer* const* DxResource::getBuffer() const noexcept
        {
            return reinterpret_cast<ID3D11Buffer* const*>(_resource.GetAddressOf());
        }

        ID3D11ShaderResourceView* const* DxResource::getResourceView() const noexcept
        {
            return reinterpret_cast<ID3D11ShaderResourceView* const*>(_view.GetAddressOf());
        }

        const bool DxResource::needToBind() const noexcept
        {
            return _needToBind;
        }

        void DxResource::bindAsInput() const noexcept
        {
            if (_resourceType == DxResourceType::VertexBuffer)
            {
                _graphicDevice.getStateManager().setIaVertexBuffers(0, 1, reinterpret_cast<ID3D11Buffer* const *>(_resource.GetAddressOf()), &_elementStride, &_elementOffset);
            }
            else if (_resourceType == DxResourceType::IndexBuffer)
            {
                _graphicDevice.getStateManager().setIaIndexBuffer(reinterpret_cast<ID3D11Buffer*>(_resource.Get()), kIndexBufferFormat, _elementOffset);
            }
            else
            {
                MINT_LOG_ERROR("�����", "bindToShader �� ȣ���ؾ� �մϴ�!");
            }

            _needToBind = false;
        }

        void DxResource::bindToShader(const DxShaderType shaderType, const uint32 bindingSlot) const noexcept
        {
            if (_resourceType == DxResourceType::ConstantBuffer)
            {
                if (shaderType == DxShaderType::VertexShader)
                {
                    _graphicDevice.getStateManager().setVsConstantBuffers(*this);
                }
                else if (shaderType == DxShaderType::GeometryShader)
                {
                    _graphicDevice.getStateManager().setGsConstantBuffers(*this);
                }
                else if (shaderType == DxShaderType::PixelShader)
                {
                    _graphicDevice.getStateManager().setPsConstantBuffers(*this);
                }
                else
                {
                    MINT_LOG_ERROR("�����", "������ ShaderType �Դϴ�!");
                }
            }
            else if (_resourceType == DxResourceType::StructuredBuffer || DxResourceType::Texture2D <= _resourceType)
            {
                if (shaderType == DxShaderType::VertexShader)
                {
                    _graphicDevice.getStateManager().setVsResources(*this);
                }
                else if (shaderType == DxShaderType::GeometryShader)
                {
                    _graphicDevice.getStateManager().setGsResources(*this);
                }
                else if (shaderType == DxShaderType::PixelShader)
                {
                    _graphicDevice.getStateManager().setPsResources(*this);
                }
                else
                {
                    MINT_LOG_ERROR("�����", "������ ShaderType �Դϴ�!");
                }
            }
            else
            {
                MINT_LOG_ERROR("�����", "bindAsInpt �� ȣ���ؾ� �մϴ�!");
            }

            _needToBind = false;
        }


        DxResourcePool::DxResourcePool(GraphicDevice& graphicDevice)
            : IDxObject(graphicDevice, DxObjectType::Pool)
        {
            __noop;
        }

        const DxObjectID& DxResourcePool::pushConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex)
        {
            DxResource resource{ _graphicDevice };
            resource._resourceType = DxResourceType::ConstantBuffer;
            if (resource.createBuffer(resourceContent, bufferSize, 1) == true)
            {
                resource.assignIDXXX();
                resource._registerIndex = registerIndex;
                
                _resourceArray.push_back(std::move(resource));
                return _resourceArray.back().getID();
            }
            MINT_ASSERT("�����", false, "pushConstantBuffer �� �����߽��ϴ�!");
            return DxObjectID::kInvalidObjectID;
        }

        const DxObjectID& DxResourcePool::pushVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount)
        {
            DxResource resource{ _graphicDevice };
            resource._resourceType = DxResourceType::VertexBuffer;
            if (resource.createBuffer(resourceContent, elementStride, elementCount) == true)
            {
                resource.assignIDXXX();
                _resourceArray.push_back(std::move(resource));
                return _resourceArray.back().getID();
            }

            MINT_ASSERT("�����", false, "pushVertexBuffer �� �����߽��ϴ�!");
            return DxObjectID::kInvalidObjectID;
        }

        const DxObjectID& DxResourcePool::pushIndexBuffer(const void* const resourceContent, const uint32 elementCount)
        {
            DxResource resource{ _graphicDevice };
            resource._resourceType = DxResourceType::IndexBuffer;
            if (resource.createBuffer(resourceContent, DxResource::kIndexBufferElementStride, elementCount) == true)
            {
                resource.assignIDXXX();
                _resourceArray.push_back(std::move(resource));
                return _resourceArray.back().getID();
            }

            MINT_ASSERT("�����", false, "pushIndexBuffer �� �����߽��ϴ�!");
            return DxObjectID::kInvalidObjectID;
        }

        const DxObjectID& DxResourcePool::pushStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex)
        {
            DxResource resource{ _graphicDevice };
            resource._resourceType = DxResourceType::StructuredBuffer;
            if (resource.createBuffer(resourceContent, elementStride, elementCount) == true)
            {
                resource.assignIDXXX();
                resource._registerIndex = registerIndex;

                _resourceArray.push_back(std::move(resource));
                return _resourceArray.back().getID();
            }

            MINT_ASSERT("�����", false, "pushStructuredBuffer �� �����߽��ϴ�!");
            return DxObjectID::kInvalidObjectID;
        }

        const DxObjectID& DxResourcePool::pushTexture2D(const DxTextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height)
        {
            DxResource resource{ _graphicDevice };
            resource._resourceType = DxResourceType::Texture2D;
            if (resource.createTexture(format, textureContent, width, height) == true)
            {
                resource.assignIDXXX();
                _resourceArray.push_back(std::move(resource));
                return _resourceArray.back().getID();
            }

            MINT_ASSERT("�����", false, "pushTexture2D �� �����߽��ϴ�!");
            return DxObjectID::kInvalidObjectID;
        }

        void DxResourcePool::bindAsInput(const DxObjectID& objectID) noexcept
        {
            DxResource& resource = getResource(objectID);
            if (resource.isValid() == true)
            {
                resource.bindAsInput();
            }
        }

        void DxResourcePool::bindToShader(const DxObjectID& objectID, const DxShaderType shaderType, const uint32 bindingSlot) noexcept
        {
            DxResource& resource = getResource(objectID);
            if (resource.isValid() == true)
            {
                resource.bindToShader(shaderType, bindingSlot);
            }
        }

        DxResource& DxResourcePool::getResource(const DxObjectID& objectID)
        {
            MINT_ASSERT("�����", objectID.isObjectType(DxObjectType::Resource) == true, "Invalid parameter - ObjectType !!");

            const int32 index = binarySearch(_resourceArray, objectID);
            if (index >= 0)
            {
                return _resourceArray[index];
            }

            MINT_ASSERT("�����", false, "Resource �� ã�� ���߽��ϴ�!!!");
            return DxResource::s_invalidInstance;
        }
    }
}
