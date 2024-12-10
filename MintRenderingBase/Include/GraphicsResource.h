#pragma once


#ifndef _MINT_RENDERING_BASE_GRAPHICS_RESOURCE_H_
#define _MINT_RENDERING_BASE_GRAPHICS_RESOURCE_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintRenderingBase/Include/GraphicsObject.h>

#include <MintContainer/Include/Vector.h>


namespace mint
{
	template<typename T>
	class OwnPtr;

	namespace Rendering
	{
		class GraphicsResourcePool;

		using Microsoft::WRL::ComPtr;
	}
}

namespace mint
{
	namespace Rendering
	{
		enum class GraphicsResourceType
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

		class GraphicsResource : public GraphicsObject
		{
			friend GraphicsResourcePool;

			static constexpr uint32 kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
			static DXGI_FORMAT GetDXGIFormat(const TextureFormat format);
			static uint32 GetColorCount(const TextureFormat format);

		private:
			GraphicsResource(GraphicsDevice& graphicsDevice);

		public:
			GraphicsResource(GraphicsResource&& rhs) noexcept = default;
			virtual ~GraphicsResource() = default;

		public:
			GraphicsResource& operator=(GraphicsResource&& rhs) noexcept = default;

		public:
			bool IsValid() const noexcept;
			uint32 GetRegisterIndex() const noexcept;
			ID3D11Buffer* const* GetBuffer() const noexcept;
			ID3D11ShaderResourceView* const* GetResourceView() const noexcept;
			bool NeedsToBind() const noexcept;

		public:
			void UpdateBuffer(const void* const resourceContent, const uint32 elementCount);
			void UpdateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			void UpdateTexture(const void* const resourceContent);
			void UpdateTexture(const void* const resourceContent, const uint32 width, const uint32 height);

		public:
			void SetOffset(const uint32 elementOffset);
			void BindAsInput() const noexcept;
			void BindToShader(const GraphicsShaderType shaderType, const uint32 bindingSlot) const noexcept;
			void UnbindFromShader() const noexcept;

		private:
			bool CreateBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			bool CreateTexture(const TextureFormat format, const void* const resourceContent, const uint32 width, const uint32 height);

		private:
			void UpdateContentInternal(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width);

		private:
			ComPtr<ID3D11Resource> _resource;
			ComPtr<ID3D11View> _view;

		private:
			GraphicsResourceType _resourceType;
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
			mutable uint32 _boundSlots[static_cast<uint32>(GraphicsShaderType::COUNT)];

		private:
			static GraphicsResource s_invalidInstance;
		};


		class GraphicsResourcePool final : public GraphicsObject
		{
		public:
			GraphicsResourcePool(GraphicsDevice& graphicsDevice);
			GraphicsResourcePool(const GraphicsResourcePool& rhs) = delete;
			virtual ~GraphicsResourcePool() = default;

		public:
			GraphicsObjectID AddConstantBuffer(const void* const resourceContent, const uint32 bufferSize, const uint32 registerIndex);
			GraphicsObjectID AddVertexBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			GraphicsObjectID AddIndexBuffer(const void* const resourceContent, const uint32 elementCount);
			GraphicsObjectID AddStructuredBuffer(const void* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 registerIndex);

		public:
			GraphicsObjectID AddTexture2D(const ByteColorImage& byteColorImage);
			GraphicsObjectID AddTexture2D(const TextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height);

		public:
			void BindAsInput(const GraphicsObjectID& objectID) noexcept;
			void BindToShader(const GraphicsObjectID& objectID, const GraphicsShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			GraphicsResource& GetResource(const GraphicsObjectID& objectID);

		private:
			Vector<OwnPtr<GraphicsObject>> _resourceArray;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_GRAPHICS_RESOURCE_H_
