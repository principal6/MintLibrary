#pragma once


#include <stdafx.h>

#include <Container/BitVector.h>
#include <Math/MathCommon.h>


namespace fs
{
	inline BitVector::BitVector()
		: _byteCapacity{ kMinByteCapacity }
		, _bitCount{ 0 }
	{
		_byteArray = FS_NEW_ARRAY(uint8, kMinByteCapacity);
	}

	inline BitVector::BitVector(const uint32 byteCapacity)
		: _byteCapacity{ 0 }
		, _bitCount{ 0 }
	{
		reserveByteCapacity(max(byteCapacity, kMinByteCapacity));
	}

	inline BitVector::~BitVector()
	{
		FS_DELETE_ARRAY(_byteArray);
	}

	inline void BitVector::push_back(const bool value)
	{
		if (isFull() == true)
		{
			reserveByteCapacity(_byteCapacity * 2);
		}
		
		++_bitCount;
		set(_bitCount - 1, value);
	}

	inline const bool BitVector::pop_back()
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

	inline void BitVector::resizeBitCount(const uint32 newBitCount)
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

	inline void BitVector::reserveByteCapacity(const uint32 newByteCapacity)
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
		const uint32 byteBitOffsetFromLeft = getByteBitOffsetByBitAt(bitAt);

		const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
		return (_byteArray[byteAt] & bitMaskOneAt);
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
		const uint32 byteBitOffsetFromLeft = getByteBitOffsetByBitAt(bitAt);
		
		const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
		const uint8 bitMaskCull = ~bitMaskOneAt;
		const uint8 bitValueAt = static_cast<uint8>((true == value) ? bitMaskOneAt : 0);

		_byteArray[byteAt] = (_byteArray[byteAt] & bitMaskCull) | bitValueAt;
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

	FS_INLINE const uint32 BitVector::getByteAtByBitAt(const uint32 bitAt) const noexcept
	{
		return bitAt / kBitsPerByte;
	}

	FS_INLINE const uint32 BitVector::getByteBitOffsetByBitAt(const uint32 bitAt) const noexcept
	{
		return bitAt % kBitsPerByte;
	}

	FS_INLINE const uint8 BitVector::getBitMaskOneAt(const uint32 bitOffsetFromLeft) const noexcept
	{
		return (1 << (kBitsPerByte - bitOffsetFromLeft - 1));
	}
}
