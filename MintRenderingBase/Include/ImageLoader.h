#pragma once


#ifndef _MINT_RENDERING_BASE_IMAGE_LOADER_H_
#define _MINT_RENDERING_BASE_IMAGE_LOADER_H_


#include <MintContainer/Include/Vector.h>
#include <MintContainer/Include/StringReference.h>
#include <MintRenderingBase/Include/RenderingBaseCommon.h>


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
			bool loadImage(const StringReferenceA& fileName, ColorImage& outColorImage) const;
			bool loadImage(const Vector<byte>& bytes, ColorImage& outColorImage) const;
			bool loadImage(const uint32 requiredComponents, const Vector<byte>& inCompressedBytes, Vector<byte>& outDecompressedBytes) const;

			bool saveImagePNG(const StringReferenceA& fileName, const ColorImage& colorImage) const;
			bool saveImagePNG(const StringReferenceA& fileName, const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components) const;
			bool saveImagePNG(const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components, BinaryFileWriter& binaryFileWriter, const bool prependLength) const;
		};
	}
}


#endif // !_MINT_RENDERING_BASE_IMAGE_LOADER_H_
