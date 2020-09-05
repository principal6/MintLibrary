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
		
		// bit count
		void						resize(const uint32 newSize);

		// byte capacity
		void						reserve(const uint32 newByteCapacity);

	public:
		const bool					isEmpty() const noexcept;
		const bool					isFull() const noexcept;
		const bool					isInSizeBoundary(const uint32 bitAt) const noexcept;
		const bool					get(const uint32 bitAt) const noexcept;
		const bool					first() const noexcept;
		const bool					last() const noexcept;
		void						set(const uint32 bitAt, const bool value) noexcept;
		void						swap(const uint32 aBitAt, const uint32 bBitAt) noexcept;

		// bit count
		const uint32				size() const noexcept;

		// byte capacity
		const uint32				capacity() const noexcept;

	private:
		const uint32				getByteAtByBitAt(const uint32 bitAt) const noexcept;
		const uint32				getByteBitOffsetByBitAt(const uint32 bitAt) const noexcept;
		const uint8					getBitMaskOneAt(const uint32 bitOffsetFromLeft) const noexcept;

	private:
		uint8*						_byteArray;
		uint32						_byteCapacity;
		uint32						_bitSize;
	};
}


#endif // !FS_BIT_VECTOR_H
