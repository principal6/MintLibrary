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
		BitVector(const uint32 ByteCapacity);
		~BitVector();

	public:
		void PushBack(const bool value);
		bool PopBack();
		void ResizeBitCount(const uint32 newBitCount);
		void ReserveByteCapacity(const uint32 newByteCapacity);

	public:
		bool IsEmpty() const noexcept;
		bool IsFull() const noexcept;
		bool IsInSizeBoundary(const uint32 bitAt) const noexcept;
		bool Get(const uint32 bitAt) const noexcept;
		uint8 GetByte(const uint32 byteAt) const noexcept;
		bool First() const noexcept;
		bool Last() const noexcept;
		void Set(const uint32 bitAt, const bool value) noexcept;
		void Set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		void SetByte(const uint32 byteAt, const uint8 byte) noexcept;
		void Fill(const bool value) noexcept;
		void Swap(const uint32 aBitAt, const uint32 bBitAt) noexcept;

	public:
		uint32 BitCount() const noexcept;
		uint32 ByteCapacity() const noexcept;

	public:
		// 여러 비트를 set, get 할 때는 아래 함수들을 이용하는 게 성능에 훨씬 좋다!
		static void SetBit(uint8& inOutByte, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		static bool GetBit(const uint8 byte, const uint32 bitOffsetFromLeft) noexcept;

	public:
		static uint32 ComputeByteCount(const uint32 BitCount) noexcept;
		static uint32 ComputeByteAt(const uint32 bitAt) noexcept;
		static uint32 ComputeBitOffset(const uint32 bitAt) noexcept;
		static uint8 MakeByte(const bool(&valueArray)[8]) noexcept;
		static uint8 MakeBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept;

	private:
		uint8* _byteArray;
		uint32 _byteCapacity;
		uint32 _bitCount;
	};
}


#endif // !_MINT_CONTAINER_BIT_VECTOR_H_
