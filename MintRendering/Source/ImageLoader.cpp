#include <MintRendering/Include/ImageLoader.h>
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
		bool ImageLoader::LoadImage_(const StringReferenceA& fileName, ByteColorImage& outByteColorImage) const
		{
			BinaryFileReader binaryFileReader;
			if (binaryFileReader.Open(fileName.CString()) == false)
			{
				return false;
			}
			binaryFileReader.GetBytes();
			return LoadImage_(binaryFileReader.GetBytes(), outByteColorImage);
		}

		bool ImageLoader::LoadImage_(const Vector<byte>& bytes, ByteColorImage& outByteColorImage) const
		{
			int32 width{};
			int32 height{};
			int32 comp{};
			const int32 req_comp{ 4 };
			stbi_uc* rawImageData = stbi_load_from_memory(bytes.Data(), bytes.Size(), &width, &height, &comp, req_comp);
			outByteColorImage.SetSize(Int2(width, height));
			const int32 dimension = static_cast<int32>(static_cast<int64>(width) * height * req_comp);
			for (int32 at = 0; at < dimension; ++at)
			{
				if (at % req_comp == 0)
				{
					outByteColorImage.SetPixel(at / req_comp, ByteColor(rawImageData[at], rawImageData[at + 1], rawImageData[at + 2], rawImageData[at + 3]));
				}
			}
			stbi_image_free(rawImageData);
			rawImageData = nullptr;
			return true;
		}

		bool ImageLoader::LoadImage_(const uint32 requiredComponents, const Vector<byte>& inCompressedBytes, Vector<byte>& outDecompressedBytes) const
		{
			int32 width{};
			int32 height{};
			int32 comp{};
			const int32 req_comp{ static_cast<int32>(requiredComponents) };
			stbi_uc* rawImageData = stbi_load_from_memory(inCompressedBytes.Data(), inCompressedBytes.Size(), &width, &height, &comp, req_comp);
			const int32 dimension = static_cast<int32>(static_cast<int64>(width) * height * comp);
			outDecompressedBytes.Resize(dimension);
			for (int32 at = 0; at < dimension; ++at)
			{
				outDecompressedBytes[at] = rawImageData[at];
			}
			stbi_image_free(rawImageData);
			rawImageData = nullptr;
			return true;
		}

		bool ImageLoader::SaveImagePNG(const StringReferenceA& fileName, const ByteColorImage& byteColorImage) const
		{
			return SaveImagePNG(fileName, byteColorImage.GetBytes(), byteColorImage.GetSize()._x, byteColorImage.GetSize()._y, 4);
		}

		bool ImageLoader::SaveImagePNG(const StringReferenceA& fileName, const byte* decompressedBytes, const uint32 width, const uint32 height, const uint32 components) const
		{
			BinaryFileWriter binaryFileWriter;
			if (SaveImagePNG(decompressedBytes, width, height, components, binaryFileWriter, false) == true)
			{
				return binaryFileWriter.Save(fileName.CString());
			}
			return false;
		}

		bool ImageLoader::SaveImagePNG(const byte* decompressedBytes, const uint32 width, const uint32 height, const uint32 components, BinaryFileWriter& binaryFileWriter, const bool prependLength) const
		{
			int32 length{ 0 };
			unsigned char* png = stbi_write_png_to_mem(decompressedBytes, width * components, width, height, components, &length);
			if (png == nullptr)
			{
				MINT_LOG_ERROR("ImageLoader - 이미지 정보를 PNG 형식으로 추출하는 데 실패했습니다.");
				return false;
			}

			if (prependLength)
			{
				binaryFileWriter.Write(length);
			}

			for (int32 at = 0; at < length; ++at)
			{
				binaryFileWriter.Write(png[at]);
			}
			STBIW_FREE(png);
			return true;
		}
	}
}
