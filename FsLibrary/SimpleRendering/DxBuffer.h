#pragma once


#ifndef FS_DX_BUFFER_H
#define FS_DX_BUFFER_H


#include <CommonDefinitions.h>

#include <FsLibrary/SimpleRendering/IDxObject.h>

namespace fs
{
	namespace SimpleRendering
	{
		class DxBufferPool;


		using Microsoft::WRL::ComPtr;


		enum class DxBufferType
		{
			INVALID,

			ConstantBuffer,
			VertexBuffer,
			IndexBuffer,
			StructuredBuffer,
		};

		using IndexElementType = uint16;
		class DxBuffer final : public IDxObject
		{
			friend DxBufferPool;

			static constexpr uint32			kIndexBufferElementStride = sizeof(IndexElementType);
			static constexpr DXGI_FORMAT	kIndexBufferFormat = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

		private:
											DxBuffer(GraphicDevice* const graphicDevice);

		public:
			virtual							~DxBuffer() = default;

		private:
			const bool						create(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount);

		public:
			const bool						isValid() const noexcept;
		
		public:
			void							updateContent(const byte* const bufferContent);
			void							updateContent(const byte* const bufferContent, const uint32 elementCount);
			void							updateContent(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount);
			void							setOffset(const uint32 elementOffset);
		
		public:
			void							bind() const noexcept;
			void							bindToShader(const DxShaderType shaderType, const uint32 bindingSlot) const noexcept;

		private:
			ComPtr<ID3D11Buffer>			_internalBuffer;
			ComPtr<ID3D11View>				_view;

		private:
			DxBufferType					_bufferType;
			uint32							_bufferSize;
			uint32							_elementStride;
			uint32							_elementCount;
			uint32							_elementOffset;

		private:
			static DxBuffer					s_invalidInstance;
		};


		class DxBufferPool final : public IDxObject
		{
		public:
											DxBufferPool(GraphicDevice* const graphicDevice);
			virtual							~DxBufferPool() = default;

		public:
			const DxObjectId&				pushConstantBuffer(const byte* const bufferContent, const uint32 bufferSize);
			const DxObjectId&				pushVertexBuffer(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount);
			const DxObjectId&				pushIndexBuffer(const byte* const bufferContent, const uint32 elementCount);
			const DxObjectId&				pushStructuredBuffer(const byte* const bufferContent, const uint32 elementStride, const uint32 elementCount);

		public:
			void							bind(const DxObjectId& objectId) noexcept;
			void							bindToShader(const DxObjectId& objectId, const DxShaderType shaderType, const uint32 bindingSlot) noexcept;

		public:
			DxBuffer&						getBuffer(const DxObjectId& objectId);

		private:
			std::vector<DxBuffer>			_bufferArray{};
		};
	}
}


#endif // !FS_DX_BUFFER_H
