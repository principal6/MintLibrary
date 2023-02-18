#pragma once


#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
#pragma region BinaryPointerReader
	MINT_INLINE void BinaryPointerReader::goTo(const uint32 at) const
	{
		_at = mint::Min(at, _byteCount);
	}

	MINT_INLINE bool BinaryPointerReader::canRead(const uint32 count) const
	{
		return (static_cast<uint64>(_at) + count <= _byteCount);
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::peek() const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T));
		if (canRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			return ptr;
		}
		return nullptr;
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::read() const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T));
		if (canRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	template <typename T>
	MINT_INLINE const T* const BinaryPointerReader::read(const uint32 count) const
	{
		const uint32 byteCount = static_cast<uint32>(sizeof(T) * count);
		if (canRead(byteCount) == true)
		{
			const T* const ptr = reinterpret_cast<const T*>(&_bytes[_at]);
			_at += byteCount;
			return ptr;
		}
		return nullptr;
	}

	MINT_INLINE void BinaryPointerReader::skip(const uint32 count) const
	{
		_at += count;
	}
#pragma endregion


#pragma region Binary File Reader
	MINT_INLINE void BinaryFileReader::goTo(const uint32 at)
	{
		_binaryPointerReader.goTo(at);
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::peek() const noexcept
	{
		return _binaryPointerReader.peek();
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::read() noexcept
	{
		return _binaryPointerReader.read<T>();
	}

	template <typename T>
	MINT_INLINE const T* const BinaryFileReader::read(const uint32 count) noexcept
	{
		return _binaryPointerReader.read<T>(count);
	}

	MINT_INLINE void BinaryFileReader::skip(const uint32 count) noexcept
	{
		return _binaryPointerReader.skip(count);
	}

	MINT_INLINE bool BinaryFileReader::canRead(const uint32 count) const noexcept
	{
		return _binaryPointerReader.canRead(count);
	}
#pragma endregion


#pragma region Binary File Writer
	MINT_INLINE void BinaryFileWriter::clear()
	{
		_bytes.Clear();
	}

	template <typename T>
	MINT_INLINE void BinaryFileWriter::write(const T& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		_writeInternal(&in, currentSize, deltaSize);
	}

	template <typename T>
	MINT_INLINE void BinaryFileWriter::write(T&& in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
		_writeInternal(&in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::write(const char* const in) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ StringUtil::Length(in) + 1 };
		_writeInternal(in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::write(const void* const in, const uint32 byteCount) noexcept
	{
		const uint32 currentSize{ static_cast<uint32>(_bytes.Size()) };
		const uint32 deltaSize{ byteCount };
		_writeInternal(in, currentSize, deltaSize);
	}

	MINT_INLINE void BinaryFileWriter::_writeInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept
	{
		_bytes.Resize(static_cast<uint64>(currentSize) + deltaSize);
		::memcpy(&_bytes[currentSize], in, deltaSize);
	}
#pragma endregion
}
