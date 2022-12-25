#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/Vector.hpp>

#include <fstream>


namespace mint
{
#pragma region Binary File Reader
	bool BinaryFileReader::open(const char* const fileName)
	{
		_at = 0;
		_bytes.clear();

		std::ifstream ifs{ fileName, std::ifstream::binary };
		if (ifs.is_open() == false)
		{
			return false;
		}

		ifs.seekg(0, ifs.end);
		const uint64 legth = ifs.tellg();
		ifs.seekg(0, ifs.beg);
		_bytes.reserve(static_cast<uint32>(legth));
		while (true)
		{
			const byte readByte{ static_cast<byte>(ifs.get()) };
			if (ifs.eof() == true)
			{
				break;
			}

			_bytes.push_back(readByte);
		}
		return true;
	}

	bool BinaryFileReader::isOpen() const noexcept
	{
		return !_bytes.empty();
	}

	uint32 BinaryFileReader::getFileSize() const noexcept
	{
		return static_cast<uint32>(_bytes.size());
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
		if (_bytes.empty() == true)
		{
			return false;
		}

		ofs.write((const char*)&_bytes[0], _bytes.size());
		return false;
	}
#pragma endregion
}
