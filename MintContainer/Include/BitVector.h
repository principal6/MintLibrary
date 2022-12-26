#pragma once


#ifndef _MINT_CONTAINER_BIT_VECTOR_H_
#define _MINT_CONTAINER_BIT_VECTOR_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	class BitVector final
	{
		static constexpr uint32	kMinByteCapacity = 1;

	public:
					BitVector();
					BitVector(const uint32 byteCapacity);
					~BitVector();

	public:
		void		push_back(const bool value);
		bool		pop_back();
		void		resizeBitCount(const uint32 newBitCount);
		void		reserveByteCapacity(const uint32 newByteCapacity);

	public:
		bool		isEmpty() const noexcept;
		bool		isFull() const noexcept;
		bool		isInSizeBoundary(const uint32 bitAt) const noexcept;
		bool		get(const uint32 bitAt) const noexcept;
		uint8		getByte(const uint32 byteAt) const noexcept;
		bool		first() const noexcept;
		bool		last() const noexcept;
		void		set(const uint32 bitAt, const bool value) noexcept;
		void		set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		void		setByte(const uint32 byteAt, const uint8 byte) noexcept;
		void		fill(const bool value) noexcept;
		void		swap(const uint32 aBitAt, const uint32 bBitAt) noexcept;

	public:
		uint32		bitCount() const noexcept;
		uint32		byteCapacity() const noexcept;

	public:
		// 여러 비트를 set, get 할 때는 아래 함수들을 이용하는 게 성능에 훨씬 좋다!
		static void		setBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		static bool		getBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept;

	public:
		static uint32	computeByteCount(const uint32 bitCount) noexcept;
		static uint32	computeByteAt(const uint32 bitAt) noexcept;
		static uint32	computeBitOffset(const uint32 bitAt) noexcept;
		static uint8	makeByte(const bool(&valueArray)[8]) noexcept;
		static uint8	makeBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept;

	private:
		uint8*		_byteArray;
		uint32		_byteCapacity;
		uint32		_bitCount;
	};
}


#endif // !_MINT_CONTAINER_BIT_VECTOR_H_
