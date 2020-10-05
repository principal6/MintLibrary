#pragma once


#include <stdafx.h>
#include <Container/BitVector.h>
#include <Math/MathCommon.h>


namespace fs
{
	inline BitVector::BitVector()
		: _byteCapacity{ kMinByteCapacity }
		, _bitSize{ 0 }
	{
		_byteArray = FS_NEW_ARRAY(uint8, kMinByteCapacity);
	}

	inline BitVector::BitVector(const uint32 byteCapacity)
		: _byteCapacity{ 0 }
		, _bitSize{ 0 }
	{
		reserve(max(byteCapacity, kMinByteCapacity));
	}

	inline BitVector::~BitVector()
	{
		FS_DELETE_ARRAY(_byteArray);
	}

	inline void BitVector::push_back(const bool value)
	{
		if (true == isFull())
		{
			reserve(_byteCapacity * 2);
		}
		
		++_bitSize;
		set(_bitSize - 1, value);
	}

	inline const bool BitVector::pop_back()
	{
		if (0 < _bitSize)
		{
			const bool result = last();
			if (true == result)
			{
				set(_bitSize - 1, false);
			}
			--_bitSize;
			return result;
		}
		return false;
	}

	inline void BitVector::resize(const uint32 newSize)
	{
		if (_bitSize == newSize)
		{
			return;
		}

		const uint32 newByteCapacity = getByteAtByBitAt(newSize);
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

			_bitSize = newSize;
			_byteCapacity = newByteCapacity;
		}
	}

	inline void BitVector::reserve(const uint32 newByteCapacity)
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

	inline const bool BitVector::isEmpty() const noexcept
	{
		return (0 == _bitSize);
	}

	inline const bool BitVector::isFull() const noexcept
	{
		return (_byteCapacity < getByteAtByBitAt(_bitSize) + 1);
	}

	inline const bool BitVector::isInSizeBoundary(const uint32 bitAt) const noexcept
	{
		return (bitAt < _bitSize);
	}

	inline const bool BitVector::get(const uint32 bitAt) const noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = getByteAtByBitAt(bitAt);
		const uint32 byteBitOffsetFromLeft = getByteBitOffsetByBitAt(bitAt);

		const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
		return (_byteArray[byteAt] & bitMaskOneAt);
	}

	inline const bool BitVector::first() const noexcept
	{
		return get(0);
	}

	inline const bool BitVector::last() const noexcept
	{
		return get(_bitSize - 1);
	}

	inline void BitVector::set(const uint32 bitAt, const bool value) noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = getByteAtByBitAt(bitAt);
		const uint32 byteBitOffsetFromLeft = getByteBitOffsetByBitAt(bitAt);
		
		const uint8 bitMaskOneAt = getBitMaskOneAt(byteBitOffsetFromLeft);
		const uint8 bitMaskCull = ~bitMaskOneAt;
		const uint8 bitValueAt = static_cast<uint8>((true == value) ? bitMaskOneAt : 0);

		_byteArray[byteAt] = (_byteArray[byteAt] & bitMaskCull) | bitValueAt;
	}

	inline void BitVector::swap(const uint32 aBitAt, const uint32 bBitAt) noexcept
	{
		FS_ASSERT("김장원", true == isInSizeBoundary(aBitAt) && true == isInSizeBoundary(bBitAt), "범위를 벗어난 접근입니다.");

		const bool a = get(aBitAt);
		const bool b = get(bBitAt);
		set(aBitAt, b);
		set(bBitAt, a);
	}

	inline const uint32 BitVector::bitCount() const noexcept
	{
		return _bitSize;
	}

	inline const uint32 BitVector::byteCapacity() const noexcept
	{
		return _byteCapacity;
	}

	inline const uint32 BitVector::getByteAtByBitAt(const uint32 bitAt) const noexcept
	{
		return bitAt / kBitsPerByte;
	}

	inline const uint32 BitVector::getByteBitOffsetByBitAt(const uint32 bitAt) const noexcept
	{
		return bitAt % kBitsPerByte;
	}

	inline const uint8 BitVector::getBitMaskOneAt(const uint32 bitOffsetFromLeft) const noexcept
	{
		return (1 << (kBitsPerByte - bitOffsetFromLeft - 1));
	}
}
