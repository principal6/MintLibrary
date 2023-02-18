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
		: BinaryPointerReader(binaryFileReader.getBytes().Data(), binaryFileReader.getFileSize())
	{
		__noop;
	}
#pragma endregion


#pragma region Binary File Reader
	bool BinaryFileReader::open(const char* const fileName)
	{
		_bytes.Clear();
		_binaryPointerReader.reset(nullptr, 0);

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
		_binaryPointerReader.reset(_bytes.Data(), static_cast<uint32>(length));
		return true;
	}

	bool BinaryFileReader::isOpen() const noexcept
	{
		return !_bytes.IsEmpty();
	}

	uint32 BinaryFileReader::getFileSize() const noexcept
	{
		return static_cast<uint32>(_bytes.Size());
	}
#pragma endregion


#pragma region Binary File Writer
	bool BinaryFileWriter::save(const char* const fileName)
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
