#pragma once


#include <stdafx.h>
#include <FsLibrary/File/BinaryFile.h>

#include <FsContainer/Include/StringUtil.h>


namespace fs
{
#pragma region Binary File Reader
	template <typename T>
	FS_INLINE const T* const BinaryFileReader::read() const noexcept
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T));
		if (canRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_byteArray[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	template <typename T>
	FS_INLINE const T* const BinaryFileReader::read(const uint32 count) const noexcept
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T) * count);
		if (canRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_byteArray[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	/*
	FS_INLINE const byte* const BinaryFileReader::read(const uint32 byteCount) const noexcept
	{
		if (canRead(byteCount) == true)
		{
			const byte* const ptr = &_byteArray[_at];
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}
	*/

	FS_INLINE void BinaryFileReader::skip(const uint32 byteCount) const noexcept
	{
		_at += byteCount;
	}

	FS_INLINE const bool BinaryFileReader::canRead(const uint32 byteCount) const noexcept
	{
		if (static_cast<uint64>(_at) + byteCount <= _byteArray.size())
		{
			return true;
		}
		return false;
	}
#pragma endregion


#pragma region Binary File Writer
	FS_INLINE void BinaryFileWriter::clear()
	{
		_byteArray.clear();
	}

	template <typename T>
	FS_INLINE void BinaryFileWriter::write(const T& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		_byteArray.resize(static_cast<uint64>(currentSize) + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	template <typename T>
	FS_INLINE void BinaryFileWriter::write(T&& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		_byteArray.resize(static_cast<uint64>(currentSize) + deltaSize);
		memcpy(&_byteArray[currentSize], &in, deltaSize);
	}

	FS_INLINE void BinaryFileWriter::write(const char* const in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
		const uint32 deltaSize{ fs::StringUtil::strlen(in) + 1 };
		_byteArray.resize(static_cast<uint64>(currentSize) + deltaSize);
		memcpy(&_byteArray[currentSize], in, deltaSize);
	}
#pragma endregion
}
