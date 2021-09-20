#pragma once


#include <MintPlatform/Include/BinaryFile.h>

#include <MintContainer/Include/StringUtil.hpp>


namespace mint
{
#pragma region Binary File Reader
    template <typename T>
    MINT_INLINE const T* const BinaryFileReader::read() const noexcept
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
    MINT_INLINE const T* const BinaryFileReader::read(const uint32 count) const noexcept
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

    MINT_INLINE void BinaryFileReader::skip(const uint32 byteCount) const noexcept
    {
        _at += byteCount;
    }

    MINT_INLINE const bool BinaryFileReader::canRead(const uint32 byteCount) const noexcept
    {
        if (static_cast<uint64>(_at) + byteCount <= _byteArray.size())
        {
            return true;
        }
        return false;
    }
#pragma endregion


#pragma region Binary File Writer
    MINT_INLINE void BinaryFileWriter::clear()
    {
        _byteArray.clear();
    }

    template <typename T>
    MINT_INLINE void BinaryFileWriter::write(const T& in) noexcept
    {
        const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
        const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
        _writeInternal(&in, currentSize, deltaSize);
    }

    template <typename T>
    MINT_INLINE void BinaryFileWriter::write(T&& in) noexcept
    {
        const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
        const uint32 deltaSize{ static_cast<uint32>(sizeof(in)) };
        _writeInternal(&in, currentSize, deltaSize);
    }

    MINT_INLINE void BinaryFileWriter::write(const char* const in) noexcept
    {
        const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
        const uint32 deltaSize{ mint::StringUtil::strlen(in) + 1 };
        _writeInternal(in, currentSize, deltaSize);
    }

    MINT_INLINE void BinaryFileWriter::write(const void* const in, const uint32 byteCount) noexcept
    {
        const uint32 currentSize{ static_cast<uint32>(_byteArray.size()) };
        const uint32 deltaSize{ byteCount };
        _writeInternal(in, currentSize, deltaSize);
    }

    MINT_INLINE void BinaryFileWriter::_writeInternal(const void* const in, const uint32 currentSize, const uint32 deltaSize) noexcept
    {
        _byteArray.resize(static_cast<uint64>(currentSize) + deltaSize);
        ::memcpy(&_byteArray[currentSize], in, deltaSize);
    }
#pragma endregion
}
