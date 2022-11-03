#pragma once


#ifndef MINT_STATIC_BIT_ARRAY_HPP
#define MINT_STATIC_BIT_ARRAY_HPP


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
		setAll(initialValue);
	}

	template<uint32 BitCount>
	inline BitArray<BitCount>::~BitArray()
	{
		__noop;
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::setAll(const bool value) noexcept
	{
		::memset(_byteArray, 0xFF * value, kByteCount);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::setByte(const uint32 byteAt, const byte value) noexcept
	{
		_byteArray[mint::min(byteAt, kByteCount - 1)] = value;
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::setByteUnsafe(const uint32 byteAt, const byte value) noexcept
	{
		_byteArray[byteAt] = value;
	}

	template<uint32 BitCount>
	inline byte BitArray<BitCount>::getByte(const uint32 byteAt) const noexcept
	{
		return _byteArray[mint::min(byteAt, kByteCount - 1)];
	}

	template<uint32 BitCount>
	inline byte BitArray<BitCount>::getByteUnsafe(const uint32 byteAt) const noexcept
	{
		return _byteArray[byteAt];
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::set(const uint32 bitAt, const bool value) noexcept
	{
		if (BitCount <= bitAt)
		{
			return;
		}

		const uint32 byteAt = BitVector::computeByteAt(bitAt);
		const uint32 bitOffset = BitVector::computeBitOffset(bitAt);
		BitVector::setBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::setUnsafe(const uint32 bitAt, const bool value) noexcept
	{
		const uint32 byteAt = BitVector::computeByteAt(bitAt);
		const uint32 bitOffset = BitVector::computeBitOffset(bitAt);
		BitVector::setBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
	{
		BitVector::setBit(_byteArray[mint::min(byteAt, kByteCount - 1)], mint::min(bitOffset, kBitsPerByte - 1), value);
	}

	template<uint32 BitCount>
	inline void BitArray<BitCount>::setUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept
	{
		BitVector::setBit(_byteArray[byteAt], bitOffset, value);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::get(const uint32 bitAt) const noexcept
	{
		const uint32 clampedBitAt = mint::min(bitAt, BitCount - 1);
		const uint32 byteAt = BitVector::computeByteAt(clampedBitAt);
		const uint32 bitOffset = BitVector::computeBitOffset(clampedBitAt);
		return BitVector::getBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::getUnsafe(const uint32 bitAt) const noexcept
	{
		const uint32 byteAt = BitVector::computeByteAt(bitAt);
		const uint32 bitOffset = BitVector::computeBitOffset(bitAt);
		return BitVector::getBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::get(const uint32 byteAt, const uint8 bitOffset) const noexcept
	{
		const uint32 clampedByteAt = mint::min(byteAt, kByteCount - 1);
		return BitVector::getBit(_byteArray[clampedByteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline bool BitArray<BitCount>::getUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept
	{
		return BitVector::getBit(_byteArray[byteAt], bitOffset);
	}

	template<uint32 BitCount>
	inline constexpr uint32 BitArray<BitCount>::getBitCount() const noexcept
	{
		return BitCount;
	}

	template<uint32 BitCount>
	inline constexpr uint32 BitArray<BitCount>::getByteCount() const noexcept
	{
		return kByteCount;
	}
}


#endif // !MINT_STATIC_BIT_ARRAY_HPP
