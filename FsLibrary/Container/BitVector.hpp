#pragma once


#include <stdafx.h>
#include <FsLibrary/Container/BitVector.h>

#include <FsMath/Include/MathCommon.h>


namespace fs
{
	FS_INLINE BitVector::BitVector()
		: _byteCapacity{ kMinByteCapacity }
		, _bitCount{ 0 }
	{
		_byteArray = FS_NEW_ARRAY(uint8, kMinByteCapacity);
	}

	FS_INLINE BitVector::BitVector(const uint32 byteCapacity)
		: _byteCapacity{ 0 }
		, _bitCount{ 0 }
	{
		reserveByteCapacity(max(byteCapacity, kMinByteCapacity));
	}

	FS_INLINE BitVector::~BitVector()
	{
		FS_DELETE_ARRAY(_byteArray);
	}

	FS_INLINE void BitVector::push_back(const bool value)
	{
		if (isFull() == true)
		{
			reserveByteCapacity(_byteCapacity * 2);
		}
		
		++_bitCount;
		set(_bitCount - 1, value);
	}

	FS_INLINE const bool BitVector::pop_back()
	{
		if (0 < _bitCount)
		{
			const bool result = last();
			if (true == result)
			{
				set(_bitCount - 1, false);
			}
			--_bitCount;
			return result;
		}
		return false;
	}

	FS_INLINE void BitVector::resizeBitCount(const uint32 newBitCount)
	{
		if (_bitCount == newBitCount)
		{
			return;
		}

		const uint32 newByteCapacity = getByteAtByBitAt(newBitCount);
		if (_byteCapacity != newByteCapacity)
		{
			uint8* temp = nullptr;

			if (nullptr != _byteArray)
			{
				temp = FS_NEW_ARRAY(uint8, _byteCapacity);
				memcpy(temp, _byteArray, _byteCapacity);

				FS_DELETE_ARRAY(_byteArray);
			}
			
			_byteArray = FS_NEW_ARRAY(uint8, newByteCapacity);

			if (nullptr != temp)
			{
				memcpy(_byteArray, temp, min(_byteCapacity, newByteCapacity));

				FS_DELETE_ARRAY(temp);
			}

			_bitCount = newBitCount;
			_byteCapacity = newByteCapacity;
		}
	}

	FS_INLINE void BitVector::reserveByteCapacity(const uint32 newByteCapacity)
	{
		if (newByteCapacity <= _byteCapacity)
		{
			return;
		}

		uint8* temp = nullptr;

		if (nullptr != _byteArray)
		{
			temp = FS_NEW_ARRAY(uint8, _byteCapacity);
			memcpy(temp, _byteArray, _byteCapacity);

			FS_DELETE_ARRAY(_byteArray);
		}

		_byteArray = FS_NEW_ARRAY(uint8, newByteCapacity);

		if (nullptr != temp)
		{
			memcpy(_byteArray, temp, min(_byteCapacity, newByteCapacity));

			FS_DELETE_ARRAY(temp);
		}

		_byteCapacity = newByteCapacity;
	}

	FS_INLINE const bool BitVector::isEmpty() const noexcept
	{
		return (0 == _bitCount);
	}

	FS_INLINE const bool BitVector::isFull() const noexcept
	{
		return (_byteCapacity < getByteAtByBitAt(_bitCount) + 1);
	}

	FS_INLINE const bool BitVector::isInSizeBoundary(const uint32 bitAt) const noexcept
	{
		return (bitAt < _bitCount);
	}

	FS_INLINE const bool BitVector::get(const uint32 bitAt) const noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = getByteAtByBitAt(bitAt);
		const uint32 byteBitOffsetFromLeft = getBitOffsetByBitAt(bitAt);

		const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
		return (_byteArray[byteAt] & bitMaskOneAt);
	}

	FS_INLINE const uint8 BitVector::getByte(const uint32 byteAt) const noexcept
	{
		FS_ASSERT("김장원", byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");

		return _byteArray[byteAt];
	}

	FS_INLINE const bool BitVector::first() const noexcept
	{
		return get(0);
	}

	FS_INLINE const bool BitVector::last() const noexcept
	{
		return get(_bitCount - 1);
	}

	FS_INLINE void BitVector::set(const uint32 bitAt, const bool value) noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = getByteAtByBitAt(bitAt);
		const uint32 bitOffsetFromLeft = getBitOffsetByBitAt(bitAt);
		setBit(_byteArray[byteAt], bitOffsetFromLeft, value);
	}

	FS_INLINE void BitVector::set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(byteAt * kBitsPerByte + bitOffsetFromLeft), "범위를 벗어난 접근입니다.");
		
		setBit(_byteArray[byteAt], bitOffsetFromLeft, value);
	}

	FS_INLINE void BitVector::setByte(const uint32 byteAt, const uint8 byte) noexcept
	{
		FS_ASSERT("김장원", byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");
		
		_byteArray[byteAt] = byte;
	}

	FS_INLINE void BitVector::swap(const uint32 aBitAt, const uint32 bBitAt) noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(aBitAt) && true == isInSizeBoundary(bBitAt), "범위를 벗어난 접근입니다.");

		const bool a = get(aBitAt);
		const bool b = get(bBitAt);
		set(aBitAt, b);
		set(bBitAt, a);
	}

	FS_INLINE const uint32 BitVector::bitCount() const noexcept
	{
		return _bitCount;
	}

	FS_INLINE const uint32 BitVector::byteCapacity() const noexcept
	{
		return _byteCapacity;
	}

	FS_INLINE void BitVector::setBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept
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

	FS_INLINE const uint8 BitVector::getByteFromArray(const bool(&valueArray)[8]) noexcept
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

	FS_INLINE const bool BitVector::getBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept
	{
		return (byte & getBitMaskOneAt(bitOffsetFromLeft));
	}

	FS_INLINE const uint32 BitVector::getByteCountFromBitCount(const uint32 bitCount) noexcept
	{
		return ((bitCount - 1) / kBitsPerByte + 1);
	}

	FS_INLINE const uint32 BitVector::getByteAtByBitAt(const uint32 bitAt) noexcept
	{
		return bitAt / kBitsPerByte;
	}

	FS_INLINE const uint32 BitVector::getBitOffsetByBitAt(const uint32 bitAt) noexcept
	{
		return bitAt % kBitsPerByte;
	}

	FS_INLINE const uint8 BitVector::getBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept
	{
		return (1 << (kBitsPerByte - bitOffsetFromLeft - 1));
	}
}
