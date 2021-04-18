#pragma once


#ifndef FS_STATIC_BIT_ARRAY_HPP
#define FS_STATIC_BIT_ARRAY_HPP


#include <FsContainer/Include/StaticBitArray.h>

#include <FsContainer/Include/BitVector.hpp>


namespace fs
{
    template<uint32 BitCount>
    inline constexpr StaticBitArray<BitCount>::StaticBitArray()
        : _byteArray{}
    {
        static_assert(0 < BitCount, "BitCount of StaticBitArray must be greater than zero!!!");
    }

    template<uint32 BitCount>
    inline StaticBitArray<BitCount>::~StaticBitArray()
    {
        __noop;
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setAll(const bool value) noexcept
    {
        ::memset(_byteArray, 0xFF * value, kByteCount);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setByte(const uint32 byteAt, const byte value) noexcept
    {
        _byteArray[fs::min(byteAt, kByteCount - 1)] = value;
    }
    
    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setByteUnsafe(const uint32 byteAt, const byte value) noexcept
    {
        _byteArray[byteAt] = value;
    }

    template<uint32 BitCount>
    inline const byte StaticBitArray<BitCount>::getByte(const uint32 byteAt) const noexcept
    {
        return _byteArray[fs::min(byteAt, kByteCount - 1)];
    }

    template<uint32 BitCount>
    inline const byte StaticBitArray<BitCount>::getByteUnsafe(const uint32 byteAt) const noexcept
    {
        return _byteArray[byteAt];
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::set(const uint32 bitAt, const bool value) noexcept
    {
        if (BitCount <= bitAt)
        {
            return;
        }

        const uint32 byteAt = fs::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = fs::BitVector::getBitOffsetByBitAt(bitAt);
        fs::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setUnsafe(const uint32 bitAt, const bool value) noexcept
    {
        const uint32 byteAt = fs::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = fs::BitVector::getBitOffsetByBitAt(bitAt);
        fs::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
    {
        fs::BitVector::setBit(_byteArray[fs::min(byteAt, kByteCount - 1)], fs::min(bitOffset, kBitsPerByte - 1), value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
    {
        fs::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::get(const uint32 bitAt) const noexcept
    {
        const uint32 clampedBitAt = fs::min(bitAt, BitCount - 1);
        const uint32 byteAt = fs::BitVector::getByteAtByBitAt(clampedBitAt);
        const uint32 bitOffset = fs::BitVector::getBitOffsetByBitAt(clampedBitAt);
        return fs::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::getUnsafe(const uint32 bitAt) const noexcept
    {
        const uint32 byteAt = fs::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = fs::BitVector::getBitOffsetByBitAt(bitAt);
        return fs::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::get(const uint32 byteAt, const uint8 bitOffset) const noexcept
    {
        const uint32 clampedByteAt = fs::min(byteAt, kByteCount - 1);
        return fs::BitVector::getBit(_byteArray[clampedByteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::getUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept
    {
        return fs::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline constexpr const uint32 StaticBitArray<BitCount>::getBitCount() const noexcept
    {
        return BitCount;
    }

    template<uint32 BitCount>
    inline constexpr const uint32 StaticBitArray<BitCount>::getByteCount() const noexcept
    {
        return kByteCount;
    }
}


#endif // !FS_STATIC_BIT_ARRAY_HPP
