#pragma once


#ifndef _MINT_RENDERING_BASE_DX_RESOURCE_H_
#define _MINT_RENDERING_BASE_DX_RESOURCE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/IGraphicObject.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	namespace Rendering
	{
		class DxResourcePool;


		using Microsoft::WRL::ComPtr;


		enum class DxResourceType
		{
			INVALID,

			// Buffer
			ConstantBuffer,
			VertexBuffer,
			IndexBuffer,
			StructuredBuffer,

			// Texture
			Texture2D,
		};

		enum class DxTextureFormat : uint16
		{
			INVALID,

			R8_UNORM,
			R8G8B8A8_UNORM,
		};


		class DxResource final : public IGraphicObject
		{
			friend DxResourcePool;

			static constexpr uint32 kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
			static DXGI_FORMAT getDxgiFormat(const DxTextureFormat format);
			static uint32 getColorCount(const DxTextureFormat format);

		private:
			DxResource(GraphicDevice& graphicDevice);

		public:
			virtual ~DxResource() = default;

		private:
			bool createBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			bool createTexture(const DxTextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height);

		public:
			bool isValid() const noexcept;

		public:
			void updateBuffer(const void* const resourceContent, const uint32 elementCount);
			void updateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);

		public:
			void updateTexture(const void* const resourceContent);
			void updateTexture(const void* const resourceContent, const uint32 width, const uint32 height);

		private:
			void updateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width);

		public:
			void setOffset(const uint32 elementOffset);

		public:
			uint32 getRegisterIndex() const noexcept;
			ID3D11Buffer* const* getBuffer() const noexcept;
			ID3D11ShaderResourceView* const* getResourceView() const noexcept;

		public:
			bool needToBind() const noexcept;
			void bindAsInput() const noexcept;
			void bindToShader(const GraphicShaderType shaderType, const uint32 bindingSlot) const noexcept;

		private:
			ComPtr<ID3D11Resource> _resource;
			ComPtr<ID3D11View> _view;

		private:
			DxResourceType _resourceType;
			uint32 _resourceCapacity;

			uint32 _elementStride;
			uint32 _elementMaxCount;
			uint32 _elementOffset;

			DxTextureFormat _textureFormat;
			uint16 _textureWidth;
			uint16 _textureHeight;

			uint16 _registerIndex;

		private:
			mutable bool _needToBind;

		private:
			static DxResource s_invalidInstance;
		};


		class DxResourcePool final : public IGraphicObject
		{
		public:
			DxResourcePool(GraphicDevice& graphicDevice);
			DxResourcePool(const DxResourcePool& rhs) = delete;
			virtual ~DxResourcePool() = default;

		public:
			const GraphicObjectID& pushConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex);
			const GraphicObjectID& pushVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			const GraphicObjectID& pushIndexBuffer(const void* const resourceContent, const uint32 elementCount);
			const GraphicObjectID& pushStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex);

		public:
			const GraphicObjectID& pushTexture2D(const DxTextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height);

		public:
			void bindAsInput(const GraphicObjectID& objectID) noexcept;
			void bindToShader(const GraphicObjectID& objectID, const GraphicShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			DxResource& getResource(const GraphicObjectID& objectID);

		private:
			Vector<DxResource> _resourceArray{};
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_RESOURCE_H_
