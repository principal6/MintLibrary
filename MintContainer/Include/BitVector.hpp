#pragma once


#include <MintContainer/Include/BitVector.h>

#include <MintMath/Include/MathCommon.h>


namespace mint
{
    MINT_INLINE BitVector::BitVector()
        : _byteCapacity{ kMinByteCapacity }
        , _bitCount{ 0 }
    {
        _byteArray = MINT_NEW_ARRAY(uint8, kMinByteCapacity);
    }

    MINT_INLINE BitVector::BitVector(const uint32 byteCapacity)
        : _byteCapacity{ 0 }
        , _bitCount{ 0 }
    {
        reserveByteCapacity(max(byteCapacity, kMinByteCapacity));
    }

    MINT_INLINE BitVector::~BitVector()
    {
        MINT_DELETE_ARRAY(_byteArray);
    }

    MINT_INLINE void BitVector::push_back(const bool value)
    {
        if (isFull() == true)
        {
            reserveByteCapacity(_byteCapacity * 2);
        }
        
        ++_bitCount;
        set(_bitCount - 1, value);
    }

    MINT_INLINE const bool BitVector::pop_back()
    {
        if (_bitCount > 0)
        {
            const bool value = last();
            if (value == true)
            {
                set(_bitCount - 1, false);
            }
            --_bitCount;
            return value;
        }
        return false;
    }

    MINT_INLINE void BitVector::resizeBitCount(const uint32 newBitCount)
    {
        if (_bitCount == newBitCount)
        {
            return;
        }

        const uint32 newByteCapacity = (newBitCount + kBitsPerByte - 1) / kBitsPerByte;
        if (_byteCapacity < newByteCapacity)
        {
            uint8* temp = nullptr;

            if (_byteArray != nullptr)
            {
                temp = MINT_NEW_ARRAY(uint8, _byteCapacity);
                memcpy(temp, _byteArray, _byteCapacity);

                MINT_DELETE_ARRAY(_byteArray);
            }
            
            _byteArray = MINT_NEW_ARRAY(uint8, newByteCapacity);

            if (temp != nullptr)
            {
                memcpy(_byteArray, temp, min(_byteCapacity, newByteCapacity));

                MINT_DELETE_ARRAY(temp);
            }

            _byteCapacity = newByteCapacity;
        }

        _bitCount = newBitCount;
    }

    MINT_INLINE void BitVector::reserveByteCapacity(const uint32 newByteCapacity)
    {
        if (newByteCapacity <= _byteCapacity)
        {
            return;
        }

        uint8* temp = nullptr;

        if (_byteArray != nullptr)
        {
            temp = MINT_NEW_ARRAY(uint8, _byteCapacity);
            memcpy(temp, _byteArray, _byteCapacity);

            MINT_DELETE_ARRAY(_byteArray);
        }

        _byteArray = MINT_NEW_ARRAY(uint8, newByteCapacity);

        if (temp != nullptr)
        {
            memcpy(_byteArray, temp, min(_byteCapacity, newByteCapacity));

            MINT_DELETE_ARRAY(temp);
        }

        _byteCapacity = newByteCapacity;
    }

    MINT_INLINE const bool BitVector::isEmpty() const noexcept
    {
        return (_bitCount == 0);
    }

    MINT_INLINE const bool BitVector::isFull() const noexcept
    {
        return (_byteCapacity < getByteAtByBitAt(_bitCount) + 1);
    }

    MINT_INLINE const bool BitVector::isInSizeBoundary(const uint32 bitAt) const noexcept
    {
        return (bitAt < _bitCount);
    }

    MINT_INLINE const bool BitVector::get(const uint32 bitAt) const noexcept
    {
        MINT_ASSERT("김장원", isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

        const uint32 byteAt = getByteAtByBitAt(bitAt);
        const uint32 byteBitOffsetFromLeft = getBitOffsetByBitAt(bitAt);

        const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
        return (_byteArray[byteAt] & bitMaskOneAt);
    }

    MINT_INLINE const uint8 BitVector::getByte(const uint32 byteAt) const noexcept
    {
        MINT_ASSERT("김장원", byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");

        return _byteArray[byteAt];
    }

    MINT_INLINE const bool BitVector::first() const noexcept
    {
        return get(0);
    }

    MINT_INLINE const bool BitVector::last() const noexcept
    {
        return get(_bitCount - 1);
    }

    MINT_INLINE void BitVector::set(const uint32 bitAt, const bool value) noexcept
    {
        MINT_ASSERT("김장원", isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

        const uint32 byteAt = getByteAtByBitAt(bitAt);
        const uint32 bitOffsetFromLeft = getBitOffsetByBitAt(bitAt);
        setBit(_byteArray[byteAt], bitOffsetFromLeft, value);
    }

    MINT_INLINE void BitVector::set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept
    {
        MINT_ASSERT("김장원", isInSizeBoundary(byteAt * kBitsPerByte + bitOffsetFromLeft), "범위를 벗어난 접근입니다.");
        
        setBit(_byteArray[byteAt], bitOffsetFromLeft, value);
    }

    MINT_INLINE void BitVector::setByte(const uint32 byteAt, const uint8 byte) noexcept
    {
        MINT_ASSERT("김장원", byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");
        
        _byteArray[byteAt] = byte;
    }

    MINT_INLINE void BitVector::fill(const bool value) noexcept
    {
        const uint8 byteValue = (value == true) ? 255 : 0;
        for (uint32 byteAt = 0; byteAt < _byteCapacity; ++byteAt)
        {
            _byteArray[byteAt] = byteValue;
        }
    }

    MINT_INLINE void BitVector::swap(const uint32 aBitAt, const uint32 bBitAt) noexcept
    {
        MINT_ASSERT("김장원", isInSizeBoundary(aBitAt) == true && isInSizeBoundary(bBitAt) == true, "범위를 벗어난 접근입니다.");

        const bool a = get(aBitAt);
        const bool b = get(bBitAt);
        set(aBitAt, b);
        set(bBitAt, a);
    }

    MINT_INLINE const uint32 BitVector::bitCount() const noexcept
    {
        return _bitCount;
    }

    MINT_INLINE const uint32 BitVector::byteCapacity() const noexcept
    {
        return _byteCapacity;
    }

    MINT_INLINE void BitVector::setBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept
    {
        // EUREKA
        // 이 경우 만큼은... 분기를 타는 게 더 빠르구나!!!

        if (value == true)
        {
            inOutByte |= getBitMaskOneAt(bitOffsetFromLeft);
        }
        else
        {
            inOutByte &= ~getBitMaskOneAt(bitOffsetFromLeft);
        }
    }

    MINT_INLINE const uint8 BitVector::getByteFromArray(const bool(&valueArray)[8]) noexcept
    {
        return static_cast<uint8>((static_cast<int32>(valueArray[0]) << 7) &
            (static_cast<int32>(valueArray[1]) << 6) &
            (static_cast<int32>(valueArray[2]) << 5) &
            (static_cast<int32>(valueArray[3]) << 4) &
            (static_cast<int32>(valueArray[4]) << 3) &
            (static_cast<int32>(valueArray[5]) << 2) &
            (static_cast<int32>(valueArray[6]) << 1) &
            static_cast<int32>(valueArray[7]));
    }

    MINT_INLINE const bool BitVector::getBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept
    {
        return (byte & getBitMaskOneAt(bitOffsetFromLeft));
    }

    MINT_INLINE const uint32 BitVector::getByteCountFromBitCount(const uint32 bitCount) noexcept
    {
        return ((bitCount - 1) / kBitsPerByte + 1);
    }

    MINT_INLINE const uint32 BitVector::getByteAtByBitAt(const uint32 bitAt) noexcept
    {
        return bitAt / kBitsPerByte;
    }

    MINT_INLINE const uint32 BitVector::getBitOffsetByBitAt(const uint32 bitAt) noexcept
    {
        return bitAt % kBitsPerByte;
    }

    MINT_INLINE const uint8 BitVector::getBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept
    {
        return (1 << (kBitsPerByte - bitOffsetFromLeft - 1));
    }
}
