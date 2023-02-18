#pragma once


#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
#pragma region BinaryPointerReader
	MINT_INLINE void BinaryPointerReader::GoTo(const uint32 at) const
	{
		_at = mint::Min(at, _byteCount);
	}

	MINT_INLINE bool BinaryPointerReader::CanRead(const uint32 count) const
	{
		return (static_cast<uint64>(_at) + count <= _byteCount);
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::Peek() const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T));
		if (CanRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			return ptr;
		}
		return nullptr;
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::Read() const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T));
		if (CanRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::Read(const uint32 count) const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T) * count);
		if (CanRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	MINT_INLINE void BinaryPointerReader::Skip(const uint32 count) const
	{
		_at += count;
	}
#pragma endregion


#pragma region Binary File Reader
	MINT_INLINE void BinaryFileReader::GoTo(const uint32 at)
	{
		_binaryPointerReader.GoTo(at);
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::Peek() const noexcept
	{
		return _binaryPointerReader.Peek();
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::Read() noexcept
	{
		return _binaryPointerReader.Read<T>();
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::Read(const uint32 count) noexcept
	{
		return _binaryPointerReader.Read<T>(count);
	}

	MINT_INLINE void BinaryFileReader::Skip(const uint32 count) noexcept
	{
		return _binaryPointerReader.Skip(count);
	}

	MINT_INLINE bool BinaryFileReader::CanRead(const uint32 count) const noexcept
	{
		return _binaryPointerReader.CanRead(count);
	}
#pragma endregion


#pragma region Binary File Writer
	MINT_INLINE void BinaryFileWriter::Clear()
	{
		_bytes.Clear();
	}

	template <typename T>
	MINT_INLINE void BinaryFileWriter::Write(const T& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		WriteInternal(&in, currentSize, deltaSize);
	}

	template <typename T>
	MINT_INLINE void BinaryFileWriter::Write(T&& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		WriteInternal(&in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::Write(const char* const in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ StringUtil::Length(in) + 1 };
		WriteInternal(in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::Write(const void* const in, const uint32 byteCount) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ byteCount };
		WriteInternal(in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::WriteInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept
	{
		_bytes.Resize(static_cast<uint64>(currentSize) + deltaSize);
		::memcpy(&_bytes[currentSize], in, deltaSize);
	}
#pragma endregion
}
