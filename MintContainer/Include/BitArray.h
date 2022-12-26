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
		void	setAll(const bool value) noexcept;

	public:
		void	setByte(const uint32 byteAt, const byte value) noexcept;
		void	setByteUnsafe(const uint32 byteAt, const byte value) noexcept;

	public:
		byte	getByte(const uint32 byteAt) const noexcept;
		byte	getByteUnsafe(const uint32 byteAt) const noexcept;

	public:
		void	set(const uint32 bitAt, const bool value) noexcept;
		void	setUnsafe(const uint32 bitAt, const bool value) noexcept;
		void	set(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;
		void	setUnsafe(const uint32 byteAt, const uint8 bitOffset, const bool value) noexcept;

	public:
		bool	get(const uint32 bitAt) const noexcept;
		bool	getUnsafe(const uint32 bitAt) const noexcept;
		bool	get(const uint32 byteAt, const uint8 bitOffset) const noexcept;
		bool	getUnsafe(const uint32 byteAt, const uint8 bitOffset) const noexcept;

	public:
		constexpr uint32	getBitCount() const noexcept;
		constexpr uint32	getByteCount() const noexcept;

	private:
		static constexpr uint32		kByteCount{ 1 + ((BitCount - 1) / kBitsPerByte) };

	private:
		byte	_byteArray[kByteCount];
	};
}


#endif // !_MINT_CONTAINER_BIT_ARRAY_H_
