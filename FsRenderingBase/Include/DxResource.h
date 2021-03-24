#pragma once


#ifndef FS_DX_RESOURCE_H
#define FS_DX_RESOURCE_H


#include <CommonDefinitions.h>

#include <FsRenderingBase/Include/IDxObject.h>


namespace fs
{
	namespace RenderingBase
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


		class DxResource final : public IDxObject
		{
			friend DxResourcePool;

			static constexpr uint32			kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT	kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
			static DXGI_FORMAT				getDxgiFormat(const DxTextureFormat format);
			static const uint32				getColorCount(const DxTextureFormat format);

		private:
											DxResource(GraphicDevice* const graphicDevice);

		public:
			virtual							~DxResource() = default;

		private:
			const bool						createBuffer(const byte* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			const bool						createTexture(const DxTextureFormat format, const byte* const resourceContent, const uint32 width, const uint32 height);

		public:
			const bool						isValid() const noexcept;
		
		public:
			void							updateBuffer(const byte* const resourceContent, const uint32 elementCount);
			void							updateBuffer(const byte* const resourceContent, const uint32 elementStride, const uint32 elementCount);
		
		public:
			void							updateTexture(const byte* const resourceContent);
			void							updateTexture(const byte* const resourceContent, const uint32 width, const uint32 height);
		
		private:
			void							updateContentInternal(const byte* const resourceContent, const uint32 elementStride, const uint32 elementCount, const uint32 width);

		public:
			void							setOffset(const uint32 elementOffset);
		
		public:
			const uint32					getRegisterIndex() const noexcept;
		
		public:
			void							bindAsInput() const noexcept;
			void							bindToShader(const DxShaderType shaderType, const uint32 bindingSlot) const noexcept;

		private:
			ComPtr<ID3D11Resource>			_resource;
			ComPtr<ID3D11View>				_view;

		private:
			DxResourceType					_resourceType;
			uint32							_resourceSize;

			uint32							_elementStride;
			uint32							_elementCount;
			uint32							_elementOffset;

			DxTextureFormat					_textureFormat;
			uint16							_textureWidth;
			uint16							_textureHeight;

			uint16							_registerIndex;

		private:
			static DxResource				s_invalidInstance;
		};


		class DxResourcePool final : public IDxObject
		{
		public:
											DxResourcePool(GraphicDevice* const graphicDevice);
											DxResourcePool(const DxResourcePool& rhs) = delete;
			virtual							~DxResourcePool() = default;

		public:
			const DxObjectId&				pushConstantBuffer(const byte* const resourceContent, const uint32 bufferSize, const uint32 registerIndex);
			const DxObjectId&				pushVertexBuffer(const byte* const resourceContent, const uint32 elementStride, const uint32 elementCount);
			const DxObjectId&				pushIndexBuffer(const byte* const resourceContent, const uint32 elementCount);
			const DxObjectId&				pushStructuredBuffer(const byte* const resourceContent, const uint32 elementStride, const uint32 elementCount);

		public:
			const DxObjectId&				pushTexture2D(const DxTextureFormat format, const byte* const textureContent, const uint32 width, const uint32 height);

		public:
			void							bindAsInput(const DxObjectId& objectId) noexcept;
			void							bindToShader(const DxObjectId& objectId, const DxShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			DxResource&						getResource(const DxObjectId& objectId);

		private:
			std::vector<DxResource>			_resourceArray{};
		};
	}
}


#endif // !FS_DX_RESOURCE_H
