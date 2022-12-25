#pragma once


#ifndef MINT_RENDERING_BASE_IMAGE_LOADER_H
#define MINT_RENDERING_BASE_IMAGE_LOADER_H


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
			bool saveImagePNG(const StringReferenceA& fileName, const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components) const;
			bool saveImagePNG(const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components, BinaryFileWriter& binaryFileWriter) const;
		};
	}
}


#endif // !MINT_RENDERING_BASE_IMAGE_LOADER_H
