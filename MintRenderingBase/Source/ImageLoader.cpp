#include <MintRenderingBase/Include/ImageLoader.h>
#include <MintPlatform/Include/BinaryFile.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>


#pragma optimize("", off)


namespace mint
{
	namespace Rendering
	{
		bool ImageLoader::loadImage(const StringReferenceA& fileName, ColorImage& outColorImage) const
		{
			BinaryFileReader binaryFileReader;
			if (binaryFileReader.open(fileName.c_str()) == false)
			{
				return false;
			}
			binaryFileReader.getBytes();
			return loadImage(binaryFileReader.getBytes(), outColorImage);
		}

		bool ImageLoader::loadImage(const Vector<byte>& bytes, ColorImage& outColorImage) const
		{
			int32 width{};
			int32 height{};
			int32 comp{};
			const int32 req_comp{ 4 };
			stbi_uc* rawImageData = stbi_load_from_memory(bytes.data(), bytes.size(), &width, &height, &comp, req_comp);
			outColorImage.setSize(Int2(width, height));
			const int32 dimension = static_cast<int32>(static_cast<int64>(width) * height * req_comp);
			for (int32 at = 0; at < dimension; ++at)
			{
				if (at % req_comp == 0)
				{
					outColorImage.setPixel(at / req_comp, Color(rawImageData[at], rawImageData[at + 1], rawImageData[at + 2], rawImageData[at + 3]));
				}
			}
			stbi_image_free(rawImageData);
			rawImageData = nullptr;
			return true;
		}

		bool ImageLoader::loadImage(const uint32 requiredComponents, const Vector<byte>& inCompressedBytes, Vector<byte>& outDecompressedBytes) const
		{
			int32 width{};
			int32 height{};
			int32 comp{};
			const int32 req_comp{ static_cast<int32>(requiredComponents) };
			stbi_uc* rawImageData = stbi_load_from_memory(inCompressedBytes.data(), inCompressedBytes.size(), &width, &height, &comp, req_comp);
			const int32 dimension = static_cast<int32>(static_cast<int64>(width) * height * comp);
			outDecompressedBytes.resize(dimension);
			for (int32 at = 0; at < dimension; ++at)
			{
				outDecompressedBytes[at] = rawImageData[at];
			}
			stbi_image_free(rawImageData);
			rawImageData = nullptr;
			return true;
		}

		bool ImageLoader::saveImagePNG(const StringReferenceA& fileName, const ColorImage& colorImage) const
		{
			Vector<byte> decompressedBytes;
			colorImage.buildPixelRgbaArray(decompressedBytes);
			return saveImagePNG(fileName, decompressedBytes, colorImage.getSize()._x, colorImage.getSize()._y, 4);
		}

		bool ImageLoader::saveImagePNG(const StringReferenceA& fileName, const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components) const
		{
			BinaryFileWriter binaryFileWriter;
			if (saveImagePNG(decompressedBytes, width, height, components, binaryFileWriter, false) == true)
			{
				return binaryFileWriter.save(fileName.c_str());
			}
			return false;
		}

		bool ImageLoader::saveImagePNG(const Vector<byte>& decompressedBytes, const uint32 width, const uint32 height, const uint32 components, BinaryFileWriter& binaryFileWriter, const bool prependLength) const
		{
			int32 length{ 0 };
			unsigned char* png = stbi_write_png_to_mem(decompressedBytes.data(), width * components, width, height, components, &length);
			if (png == nullptr)
			{
				MINT_LOG_ERROR("ImageLoader - 이미지 정보를 PNG 형식으로 추출하는 데 실패했습니다.");
				return false;
			}

			if (prependLength)
			{
				binaryFileWriter.write(length);
			}

			for (int32 at = 0; at < length; ++at)
			{
				binaryFileWriter.write(png[at]);
			}
			STBIW_FREE(png);
			return true;
		}
	}
}
