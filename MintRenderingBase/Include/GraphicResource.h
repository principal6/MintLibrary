#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHIC_RESOURCE_H_
#define _MINT_RENDERING_BASE_GRAPHIC_RESOURCE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicObject.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	namespace Rendering
	{
		class GraphicResourcePool;

		using Microsoft::WRL::ComPtr;
	}
}

namespace mint
{
	namespace Rendering
	{
		enum class GraphicResourceType
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

		enum class TextureFormat : uint16
		{
			INVALID,

			R8_UNORM,
			R8G8B8A8_UNORM,
		};

		class GraphicResource final : public GraphicObject
		{
			friend GraphicResourcePool;

			static constexpr uint32 kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
			static DXGI_FORMAT GetDXGIFormat(const TextureFormat format);
			static uint32 GetColorCount(const TextureFormat format);

		private:
			GraphicResource(GraphicDevice& graphicDevice);

		public:
			GraphicResource(GraphicResource&& rhs) noexcept = default;
			virtual ~GraphicResource() = default;

		public:
			GraphicResource& operator=(GraphicResource&& rhs) noexcept = default;

		private:
			bool CreateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			bool CreateTexture(const TextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height);

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
			GraphicResourceType _resourceType;
			uint32 _resourceCapacity;

			uint32 _elementStride;
			uint32 _elementMaxCount;
			uint32 _elementOffset;

			TextureFormat _textureFormat;
			uint16 _textureWidth;
			uint16 _textureHeight;

			uint16 _registerIndex;

		private:
			mutable bool _needToBind;
			mutable uint32 _boundSlots[static_cast<uint32>(GraphicShaderType::COUNT)];

		private:
			static GraphicResource s_invalidInstance;
		};


		class GraphicResourcePool final : public GraphicObject
		{
		public:
			GraphicResourcePool(GraphicDevice& graphicDevice);
			GraphicResourcePool(const GraphicResourcePool& rhs) = delete;
			virtual ~GraphicResourcePool() = default;

		public:
			GraphicObjectID AddConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex);
			GraphicObjectID AddVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			GraphicObjectID AddIndexBuffer(const void* const resourceContent, const uint32 elementCount);
			GraphicObjectID AddStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex);

		public:
			GraphicObjectID AddTexture2D(const TextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height);

		public:
			void BindAsInput(const GraphicObjectID& objectID) noexcept;
			void BindToShader(const GraphicObjectID& objectID, const GraphicShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			GraphicResource& GetResource(const GraphicObjectID& objectID);

		private:
			Vector<GraphicResource> _resourceArray;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GRAPHIC_RESOURCE_H_
