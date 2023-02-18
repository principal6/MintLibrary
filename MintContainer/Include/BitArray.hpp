#pragma once


#ifndef _MINT_CONTAINER_BIT_ARRAY_HPP_
#define _MINT_CONTAINER_BIT_ARRAY_HPP_


#include <MintContainer/Include/BitArray.h>

#include <MintContainer/Include/BitVector.hpp>


namespace mint
{
	template<uint32 BitCount>
	inline BitArray<BitCount>::BitArray()
		: _byteArray{}
	{
		static_assert(BitCount > 0, "BitCount of BitArray must be greater than zero!!!");
	}

	template<uint32 BitCount>
	inline BitArray<BitCount>::BitArray(const bool initialValue)
	{
		SetAll(initialValue);
	}

	template<uint32 BitCount>
	inline BitArray<BitCount>::~BitArray()
	{
		__noop;
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::SetAll(const bool value) noexcept
	{
		::memset(_byteArray, 0xFF * value, kByteCount);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::SetByte(const uint32 byteAt, const byte value) noexcept
	{
		_byteArray[mint::Min(byteAt, kByteCount - 1)] = value;
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::SetByteUnsafe(const uint32 byteAt, const byte value) noexcept
	{
		_byteArray[byteAt] = value;
	}

	template<uint32 BitCount>
	inline byte BitArray<BitCount>::GetByte(const uint32 byteAt) const noexcept
	{
		return _byteArray[mint::Min(byteAt, kByteCount - 1)];
	}

	template<uint32 BitCount>
	inline byte BitArray<BitCount>::GetByteUnsafe(const uint32 byteAt) const noexcept
	{
		return _byteArray[byteAt];
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::Set(const uint32 bitAt, const bool value) noexcept
	{
		if (BitCount <= bitAt)
		{
			return;
		}

		const uint32 byteAt = BitVector::ComputeByteAt(bitAt);
		const uint32 bitOffset = BitVector::ComputeBitOffset(bitAt);
		BitVector::SetBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::SetUnsafe(const uint32 bitAt, const bool value) noexcept
	{
		const uint32 byteAt = BitVector::ComputeByteAt(bitAt);
		const uint32 bitOffset = BitVector::ComputeBitOffset(bitAt);
		BitVector::SetBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::Set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
	{
		BitVector::SetBit(_byteArray[mint::Min(byteAt, kByteCount - 1)], mint::Min(bitOffset, kBitsPerByte - 1), value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::SetUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
	{
		BitVector::SetBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::Get(const uint32 bitAt) const noexcept
	{
		const uint32 clampedBitAt = mint::Min(bitAt, BitCount - 1);
		const uint32 byteAt = BitVector::ComputeByteAt(clampedBitAt);
		const uint32 bitOffset = BitVector::ComputeBitOffset(clampedBitAt);
		return BitVector::GetBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::GetUnsafe(const uint32 bitAt) const noexcept
	{
		const uint32 byteAt = BitVector::ComputeByteAt(bitAt);
		const uint32 bitOffset = BitVector::ComputeBitOffset(bitAt);
		return BitVector::GetBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::Get(const uint32 byteAt, const uint8 bitOffset) const noexcept
	{
		const uint32 clampedByteAt = mint::Min(byteAt, kByteCount - 1);
		return BitVector::GetBit(_byteArray[clampedByteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::GetUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept
	{
		return BitVector::GetBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline constexpr uint32 BitArray<BitCount>::GetBitCount() const noexcept
	{
		return BitCount;
	}

	template<uint32 BitCount>
	inline constexpr uint32 BitArray<BitCount>::GetByteCount() const noexcept
	{
		return kByteCount;
	}
}


#endif // !_MINT_CONTAINER_BIT_ARRAY_HPP_
