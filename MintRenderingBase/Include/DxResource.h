#pragma once


#ifndef _MINT_RENDERING_BASE_DX_RESOURCE_H_
#define _MINT_RENDERING_BASE_DX_RESOURCE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicObject.h>

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


		class DxResource final : public GraphicObject
		{
			friend DxResourcePool;

			static constexpr uint32 kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
			static DXGI_FORMAT GetDXGIFormat(const DxTextureFormat format);
			static uint32 GetColorCount(const DxTextureFormat format);

		private:
			DxResource(GraphicDevice& graphicDevice);

		public:
			DxResource(DxResource&& rhs) noexcept = default;
			virtual ~DxResource() = default;

		public:
			DxResource& operator=(DxResource&& rhs) noexcept = default;

		private:
			bool CreateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			bool CreateTexture(const DxTextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height);

		public:
			bool IsValid() const noexcept;

		public:
			void UpdateBuffer(const void* const resourceContent, const uint32 elementCount);
			void UpdateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);

		public:
			void UpdateTexture(const void* const resourceContent);
			void UpdateTexture(const void* const resourceContent, const uint32 width, const uint32 height);

		private:
			void UpdateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width);

		public:
			void SetOffset(const uint32 elementOffset);

		public:
			uint32 GetRegisterIndex() const noexcept;
			ID3D11Buffer* const* GetBuffer() const noexcept;
			ID3D11ShaderResourceView* const* GetResourceView() const noexcept;

		public:
			bool NeedsToBind() const noexcept;
			void BindAsInput() const noexcept;
			void BindToShader(const GraphicShaderType shaderType, const uint32 bindingSlot) const noexcept;
			void UnbindFromShader() const noexcept;

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
			mutable uint32 _boundSlots[static_cast<uint32>(GraphicShaderType::COUNT)];

		private:
			static DxResource s_invalidInstance;
		};


		class DxResourcePool final : public GraphicObject
		{
		public:
			DxResourcePool(GraphicDevice& graphicDevice);
			DxResourcePool(const DxResourcePool& rhs) = delete;
			virtual ~DxResourcePool() = default;

		public:
			GraphicObjectID AddConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex);
			GraphicObjectID AddVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			GraphicObjectID AddIndexBuffer(const void* const resourceContent, const uint32 elementCount);
			GraphicObjectID AddStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex);

		public:
			GraphicObjectID AddTexture2D(const DxTextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height);

		public:
			void BindAsInput(const GraphicObjectID& objectID) noexcept;
			void BindToShader(const GraphicObjectID& objectID, const GraphicShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			DxResource& GetResource(const GraphicObjectID& objectID);

		private:
			Vector<DxResource> _resourceArray;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_DX_RESOURCE_H_
