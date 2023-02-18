#pragma once


#ifndef _MINT_CONTAINER_BIT_ARRAY_H_
#define _MINT_CONTAINER_BIT_ARRAY_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template <uint32 BitCount>
	class BitArray
	{
	public:
		BitArray();
		BitArray(const bool initialValue);
		~BitArray();

	public:
		void SetAll(const bool value) noexcept;
			 
	public:	 
		void SetByte(const uint32 byteAt, const byte value) noexcept;
		void SetByteUnsafe(const uint32 byteAt, const byte value) noexcept;
			 
	public:	 
		byte GetByte(const uint32 byteAt) const noexcept;
		byte GetByteUnsafe(const uint32 byteAt) const noexcept;

	public:
		void Set(const uint32 bitAt, const bool value) noexcept;
		void SetUnsafe(const uint32 bitAt, const bool value) noexcept;
		void Set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;
		void SetUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;
			 
	public:	 
		bool Get(const uint32 bitAt) const noexcept;
		bool GetUnsafe(const uint32 bitAt) const noexcept;
		bool Get(const uint32 byteAt, const uint8 bitOffset) const noexcept;
		bool GetUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept;

	public:
		constexpr uint32 GetBitCount() const noexcept;
		constexpr uint32 GetByteCount() const noexcept;

	private:
		static constexpr uint32 kByteCount{ 1 + ((BitCount - 1) / kBitsPerByte) };

	private:
		byte _byteArray[kByteCount];
	};
}


#endif // !_MINT_CONTAINER_BIT_ARRAY_H_
