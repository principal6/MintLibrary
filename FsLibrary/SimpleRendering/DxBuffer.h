#pragma once


#ifndef FS_DX_BUFFER_H
#define FS_DX_BUFFER_H


#include <CommonDefinitions.h>
#include <SimpleRendering/IDxObject.h>


namespace fs
{
	namespace SimpleRendering
	{
		class DxBufferPool;


		using Microsoft::WRL::ComPtr;


		enum class DxBufferType
		{
			VertexBuffer,
			IndexBuffer,
			ConstantBuffer,
		};

		class DxBuffer final : public IDxObject
		{
			friend DxBufferPool;

		private:
									DxBuffer(GraphicDevice* const graphicDevice);

		public:
			virtual					~DxBuffer() = default;

		public:
			void					update();
			void					reset(const byte* const bufferContent, const uint32 bufferSize);
			void					bind() const noexcept;
			void					bindToShader(const DxShaderType shaderType, const uint32 bindSlot) const noexcept;

		private:
			ComPtr<ID3D11Buffer>	_buffer;
			DxBufferType			_bufferType{};
			const byte*				_bufferContent{};
			uint32					_bufferSize{};

		public:
			static const DxBuffer	kNullInstance;
		};


		class DxBufferPool final : public IDxObject
		{
		public:
									DxBufferPool(GraphicDevice* const graphicDevice);
			virtual					~DxBufferPool() = default;

		public:
			const DxObjectId&		pushBuffer(const DxBufferType bufferType, const byte* const bufferContent, const uint32 bufferSize);

		public:
			const DxBuffer&			getBuffer(const DxObjectId& objectId);

		private:
			std::vector<DxBuffer>	_bufferArray{};
		};
	}
}


#endif // !FS_DX_BUFFER_H
