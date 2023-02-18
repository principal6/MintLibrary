#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/Vector.hpp>

#include <fstream>


namespace mint
{
#pragma region BinaryPointerReader
	BinaryPointerReader::BinaryPointerReader()
		: BinaryPointerReader(nullptr, 0)
	{
		__noop;
	}

	BinaryPointerReader::BinaryPointerReader(const byte* const bytes, const uint32 byteCount)
		: _bytes{ bytes }
		, _byteCount{ byteCount }
	{
		__noop;
	}

	BinaryPointerReader::BinaryPointerReader(const BinaryFileReader& binaryFileReader)
		: BinaryPointerReader(binaryFileReader.GetBytes().Data(), binaryFileReader.GetFileSize())
	{
		__noop;
	}
#pragma endregion


#pragma region Binary File Reader
	bool BinaryFileReader::Open(const char* const fileName)
	{
		_bytes.Clear();
		_binaryPointerReader.Reset(nullptr, 0);

		std::ifstream ifs{ fileName, std::ifstream::binary };
		if (ifs.is_open() == false)
		{
			return false;
		}

		ifs.seekg(0, ifs.end);
		const uint64 length = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		_bytes.Resize(static_cast<uint32>(length));
		ifs.read(reinterpret_cast<char*>(&_bytes[0]), length);
		_binaryPointerReader.Reset(_bytes.Data(), static_cast<uint32>(length));
		return true;
	}

	bool BinaryFileReader::IsOpen() const noexcept
	{
		return !_bytes.IsEmpty();
	}

	uint32 BinaryFileReader::GetFileSize() const noexcept
	{
		return static_cast<uint32>(_bytes.Size());
	}
#pragma endregion


#pragma region Binary File Writer
	bool BinaryFileWriter::Save(const char* const fileName)
	{
		std::ofstream ofs{ fileName, std::ofstream::binary };
		if (ofs.is_open() == false)
		{
			return false;
		}
		if (_bytes.IsEmpty() == true)
		{
			return false;
		}

		ofs.write((const char*)&_bytes[0], _bytes.Size());
		return false;
	}
#pragma endregion
}
