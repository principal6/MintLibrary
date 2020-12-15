#pragma once


#ifndef FS_BIT_VECTOR_H
#define FS_BIT_VECTOR_H


#include <CommonDefinitions.h>


namespace fs
{
	class BitVector final
	{
		static constexpr uint32		kMinByteCapacity	= 1;

	public:
									BitVector();
									BitVector(const uint32 byteCapacity);
									~BitVector();

	public:
		void						push_back(const bool value);
		const bool					pop_back();
		void						resizeBitCount(const uint32 newBitCount);
		void						reserveByteCapacity(const uint32 newByteCapacity);

	public:
		const bool					isEmpty() const noexcept;
		const bool					isFull() const noexcept;
		const bool					isInSizeBoundary(const uint32 bitAt) const noexcept;
		const bool					get(const uint32 bitAt) const noexcept;
		const uint8					getByte(const uint32 byteAt) const noexcept;
		const bool					first() const noexcept;
		const bool					last() const noexcept;
		void						set(const uint32 bitAt, const bool value) noexcept;
		void						set(const uint32 byteAt, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		void						setByte(const uint32 byteAt, const uint8 byte) noexcept;
		void						swap(const uint32 aBitAt, const uint32 bBitAt) noexcept;

	public:
		const uint32				bitCount() const noexcept;
		const uint32				byteCapacity() const noexcept;

	public:
		static void					setBit(uint8& byte, const uint32 bitOffsetFromLeft, const bool value) noexcept;
		static const uint32			getByteAtByBitAt(const uint32 bitAt) noexcept;
		static const uint32			getBitOffsetByBitAt(const uint32 bitAt) noexcept;
		static const uint8			getBitMaskOneAt(const uint32 bitOffsetFromLeft) noexcept;

	private:
		uint8*						_byteArray;
		uint32						_byteCapacity;
		uint32						_bitCount;
	};
}


#endif // !FS_BIT_VECTOR_H
