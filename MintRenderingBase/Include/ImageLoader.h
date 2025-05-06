#pragma once


#ifndef _MINT_RENDERING_BASE_IMAGE_LOADER_H_
#define _MINT_RENDERING_BASE_IMAGE_LOADER_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>


namespace mint
{
	class BinaryFileWriter;
}

namespace mint
{
	namespace Rendering
	{
		class ImageLoader
		{
		public:
			ImageLoader() = default;
			~ImageLoader() = default;

		public:
			bool LoadImage_(const StringReferenceA& fileName, ByteColorImage& outByteColorImage) const;
			bool LoadImage_(const Vector<byte>& bytes, ByteColorImage& outByteColorImage) const;
			bool LoadImage_(const uint32 requiredComponents, const Vector<byte>& inCompressedBytes, Vector<byte>& outDecompressedBytes) const;

			bool SaveImagePNG(const StringReferenceA& fileName, const ByteColorImage& byteColorImage) const;
			bool SaveImagePNG(const StringReferenceA& fileName, const byte* decompressedBytes, const uint32 width, const uint32 height, const uint32 components) const;
			bool SaveImagePNG(const byte* decompressedBytes, const uint32 width, const uint32 height, const uint32 components, BinaryFileWriter& binaryFileWriter, const bool prependLength) const;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_IMAGE_LOADER_H_
