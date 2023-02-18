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

	MINT_INLINE BitVector::BitVector(const uint32 ByteCapacity)
		: _byteCapacity{ 0 }
		, _bitCount{ 0 }
	{
		ReserveByteCapacity(Max(ByteCapacity, kMinByteCapacity));
	}

	MINT_INLINE BitVector::~BitVector()
	{
		MINT_DELETE_ARRAY(_byteArray);
	}

	MINT_INLINE void BitVector::PushBack(const bool value)
	{
		if (IsFull() == true)
		{
			ReserveByteCapacity(_byteCapacity * 2);
		}

		++_bitCount;
		Set(_bitCount - 1, value);
	}

	MINT_INLINE bool BitVector::PopBack()
	{
		if (_bitCount > 0)
		{
			const bool value = Last();
			if (value == true)
			{
				Set(_bitCount - 1, false);
			}
			--_bitCount;
			return value;
		}
		return false;
	}

	MINT_INLINE void BitVector::ResizeBitCount(const uint32 newBitCount)
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
				memcpy(_byteArray, temp, Min(_byteCapacity, newByteCapacity));

				MINT_DELETE_ARRAY(temp);
			}

			_byteCapacity = newByteCapacity;
		}

		_bitCount = newBitCount;
	}

	MINT_INLINE void BitVector::ReserveByteCapacity(const uint32 newByteCapacity)
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
			memcpy(_byteArray, temp, Min(_byteCapacity, newByteCapacity));

			MINT_DELETE_ARRAY(temp);
		}

		_byteCapacity = newByteCapacity;
	}

	MINT_INLINE bool BitVector::IsEmpty() const noexcept
	{
		return (_bitCount == 0);
	}

	MINT_INLINE bool BitVector::IsFull() const noexcept
	{
		return (_byteCapacity < ComputeByteAt(_bitCount) + 1);
	}

	MINT_INLINE bool BitVector::IsInSizeBoundary(const uint32 bitAt) const noexcept
	{
		return (bitAt < _bitCount);
	}

	MINT_INLINE bool BitVector::Get(const uint32 bitAt) const noexcept
	{
		MINT_ASSERT(IsInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = ComputeByteAt(bitAt);
		const uint32 byteBitOffsetFromLeft = ComputeBitOffset(bitAt);

		const uint8 bitMaskOneAt = MakeBitMaskOneAt(byteBitOffsetFromLeft);
		return (_byteArray[byteAt] & bitMaskOneAt);
	}

	MINT_INLINE uint8 BitVector::GetByte(const uint32 byteAt) const noexcept
	{
		MINT_ASSERT(byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");

		return _byteArray[byteAt];
	}

	MINT_INLINE bool BitVector::First() const noexcept
	{
		return Get(0);
	}

	MINT_INLINE bool BitVector::Last() const noexcept
	{
		return Get(_bitCount - 1);
	}

	MINT_INLINE void BitVector::Set(const uint32 bitAt, const bool value) noexcept
	{
		MINT_ASSERT(IsInSizeBoundary(bitAt), "범위를 벗어난 접근입니다.");

		const uint32 byteAt = ComputeByteAt(bitAt);
		const uint32 bitOffsetFromLeft = ComputeBitOffset(bitAt);
		SetBit(_byteArray[byteAt], bitOffsetFromLeft, value);
	}

	MINT_INLINE void BitVector::Set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept
	{
		MINT_ASSERT(IsInSizeBoundary(byteAt * kBitsPerByte + bitOffsetFromLeft), "범위를 벗어난 접근입니다.");

		SetBit(_byteArray[byteAt], bitOffsetFromLeft, value);
	}

	MINT_INLINE void BitVector::SetByte(const uint32 byteAt, const uint8 byte) noexcept
	{
		MINT_ASSERT(byteAt < _byteCapacity, "범위를 벗어난 접근입니다.");

		_byteArray[byteAt] = byte;
	}

	MINT_INLINE void BitVector::Fill(const bool value) noexcept
	{
		const uint8 byteValue = (value == true) ? 255 : 0;
		for (uint32 byteAt = 0; byteAt < _byteCapacity; ++byteAt)
		{
			_byteArray[byteAt] = byteValue;
		}
	}

	MINT_INLINE void BitVector::Swap(const uint32 aBitAt, const uint32 bBitAt) noexcept
	{
		MINT_ASSERT(IsInSizeBoundary(aBitAt) == true && IsInSizeBoundary(bBitAt) == true, "범위를 벗어난 접근입니다.");

		const bool a = Get(aBitAt);
		const bool b = Get(bBitAt);
		Set(aBitAt, b);
		Set(bBitAt, a);
	}

	MINT_INLINE uint32 BitVector::BitCount() const noexcept
	{
		return _bitCount;
	}

	MINT_INLINE uint32 BitVector::ByteCapacity() const noexcept
	{
		return _byteCapacity;
	}

	MINT_INLINE void BitVector::SetBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept
	{
		// EUREKA
		// 이 경우 만큼은... 분기를 타는 게 더 빠르구나!!!

		if (value == true)
		{
			inOutByte |= MakeBitMaskOneAt(bitOffsetFromLeft);
		}
		else
		{
			inOutByte &= ~MakeBitMaskOneAt(bitOffsetFromLeft);
		}
	}

	MINT_INLINE bool BitVector::GetBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept
	{
		return (byte & MakeBitMaskOneAt(bitOffsetFromLeft));
	}

	MINT_INLINE uint32 BitVector::ComputeByteCount(const uint32 BitCount) noexcept
	{
		return ((BitCount - 1) / kBitsPerByte + 1);
	}

	MINT_INLINE uint32 BitVector::ComputeByteAt(const uint32 bitAt) noexcept
	{
		return bitAt / kBitsPerByte;
	}

	MINT_INLINE uint32 BitVector::ComputeBitOffset(const uint32 bitAt) noexcept
	{
		return bitAt % kBitsPerByte;
	}

	MINT_INLINE uint8 BitVector::MakeByte(const bool(&valueArray)[8]) noexcept
	{
		return static_cast<uint8>(
			(static_cast<int32>(valueArray[0]) << 7) &
			(static_cast<int32>(valueArray[1]) << 6) &
			(static_cast<int32>(valueArray[2]) << 5) &
			(static_cast<int32>(valueArray[3]) << 4) &
			(static_cast<int32>(valueArray[4]) << 3) &
			(static_cast<int32>(valueArray[5]) << 2) &
			(static_cast<int32>(valueArray[6]) << 1) &
			static_cast<int32>(valueArray[7])
			);
	}

	MINT_INLINE uint8 BitVector::MakeBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept
	{
		return (1 << (kBitsPerByte - bitOffsetFromLeft - 1));
	}
}
