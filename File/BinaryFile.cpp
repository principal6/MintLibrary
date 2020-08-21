#include "BinaryFile.h"

#include <fstream>


namespace fs
{
#pragma region Binary File Reader
	BinaryFileReader::BinaryFileReader()
	{
		_at = 0;
	}

	BinaryFileReader::BinaryFileReader(const char* const fileName)
	{
		_at = 0;

		load(fileName);
	}

	BinaryFileReader::~BinaryFileReader()
	{
	}

	bool BinaryFileReader::load(const char* const fileName)
	{
		std::ifstream ifs{ fileName, std::ifstream::binary };
		if (ifs.is_open() == false) return false;

		_byteArray.clear();
		while (ifs.eof() == false)
		{
			unsigned char byte{ static_cast<unsigned char>(ifs.get()) };
			_byteArray.emplace_back(byte);
		}
		return true;
	}

	void BinaryFileReader::skip(const uint32 byteCount) const noexcept
	{
		_at += byteCount;
	}

	bool BinaryFileReader::readBool(bool& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(bool)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readInt8(int8& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(int8)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readInt16(int16& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(int16)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readInt32(int32& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(int32)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readUint8(uint8& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(uint8)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readUint16(uint16& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(uint16)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readUint32(uint32& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(uint32)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readFloat(float& out) const noexcept
	{
		const uint32 size{ static_cast<uint32>(sizeof(float)) };
		if (canRead(size) == true)
		{
			memcpy(&out, &_byteArray[_at], size);
			_at += size;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::readString(const uint32 byteCount, std::string& out) const noexcept
	{
		if (canRead(byteCount) == true)
		{
			memcpy(&out[0], &_byteArray[_at], byteCount);
			_at += byteCount;
			return true;
		}
		return false;
	}

	bool BinaryFileReader::canRead(const uint32 byteCount) const noexcept
	{
		if (_at + byteCount <= _byteArray.size())
		{
			return true;
		}
		return false;
	}
#pragma endregion


#pragma region Binary File Writer
	BinaryFileWriter::BinaryFileWriter()
	{
	}

	BinaryFileWriter::~BinaryFileWriter()
	{
	}

	bool BinaryFileWriter::save(const char* const fileName)
	{
		std::ofstream ofs{ fileName, std::ofstream::binary };
		if (ofs.is_open() == false) return false;
		if (_byteArray.empty() == true) return false;
		auto state = ofs.rdstate();

		ofs.write((const char*)&_byteArray[0], _byteArray.size());
		return false;
	}

	void BinaryFileWriter::clear()
	{
		_byteArray.clear();
	}

	void BinaryFileWriter::writeBool(bool in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(bool)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeInt8(int8 in) noexcept
	{
		uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		uint32 deltaSize{ static_cast<uint32>(sizeof(int8)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeInt16(int16 in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(int16)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeInt32(int32 in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(int32)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeUint8(uint8 in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(uint8)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeUint16(uint16 in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(uint16)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeUint32(uint32 in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(uint32)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeFloat(float in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(float)) };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	void BinaryFileWriter::writeString(const std::string& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ in.size() };
		_byteArray.resize(currentSize + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}
#pragma endregion
}
