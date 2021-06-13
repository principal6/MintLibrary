#pragma once


#ifndef MINT_STATIC_BIT_ARRAY_HPP
#define MINT_STATIC_BIT_ARRAY_HPP


#include <MintContainer/Include/StaticBitArray.h>

#include <MintContainer/Include/BitVector.hpp>


namespace mint
{
    template<uint32 BitCount>
    inline StaticBitArray<BitCount>::StaticBitArray()
        : _byteArray{}
    {
        static_assert(0 < BitCount, "BitCount of StaticBitArray must be greater than zero!!!");
    }

    template<uint32 BitCount>
    inline StaticBitArray<BitCount>::StaticBitArray(const bool initialValue)
    {
        setAll(initialValue);
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
        _byteArray[mint::min(byteAt, kByteCount - 1)] = value;
    }
    
    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setByteUnsafe(const uint32 byteAt, const byte value) noexcept
    {
        _byteArray[byteAt] = value;
    }

    template<uint32 BitCount>
    inline const byte StaticBitArray<BitCount>::getByte(const uint32 byteAt) const noexcept
    {
        return _byteArray[mint::min(byteAt, kByteCount - 1)];
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

        const uint32 byteAt = mint::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = mint::BitVector::getBitOffsetByBitAt(bitAt);
        mint::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setUnsafe(const uint32 bitAt, const bool value) noexcept
    {
        const uint32 byteAt = mint::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = mint::BitVector::getBitOffsetByBitAt(bitAt);
        mint::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
    {
        mint::BitVector::setBit(_byteArray[mint::min(byteAt, kByteCount - 1)], mint::min(bitOffset, kBitsPerByte - 1), value);
    }

    template<uint32 BitCount>
    inline void StaticBitArray<BitCount>::setUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
    {
        mint::BitVector::setBit(_byteArray[byteAt], bitOffset, value);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::get(const uint32 bitAt) const noexcept
    {
        const uint32 clampedBitAt = mint::min(bitAt, BitCount - 1);
        const uint32 byteAt = mint::BitVector::getByteAtByBitAt(clampedBitAt);
        const uint32 bitOffset = mint::BitVector::getBitOffsetByBitAt(clampedBitAt);
        return mint::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::getUnsafe(const uint32 bitAt) const noexcept
    {
        const uint32 byteAt = mint::BitVector::getByteAtByBitAt(bitAt);
        const uint32 bitOffset = mint::BitVector::getBitOffsetByBitAt(bitAt);
        return mint::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::get(const uint32 byteAt, const uint8 bitOffset) const noexcept
    {
        const uint32 clampedByteAt = mint::min(byteAt, kByteCount - 1);
        return mint::BitVector::getBit(_byteArray[clampedByteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const bool StaticBitArray<BitCount>::getUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept
    {
        return mint::BitVector::getBit(_byteArray[byteAt], bitOffset);
    }

    template<uint32 BitCount>
    inline const uint32 StaticBitArray<BitCount>::getBitCount() const noexcept
    {
        return BitCount;
    }

    template<uint32 BitCount>
    inline const uint32 StaticBitArray<BitCount>::getByteCount() const noexcept
    {
        return kByteCount;
    }
}


#endif // !MINT_STATIC_BIT_ARRAY_HPP
