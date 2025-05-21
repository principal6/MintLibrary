#pragma once


#ifndef _MINT_CONTAINER_SERIAL_AND_INDEX_H_
#define _MINT_CONTAINER_SERIAL_AND_INDEX_H_


#include <MintCommon/Include/CommonDefinitions.h>
#include <type_traits>


namespace mint
{
	template<typename UintT, uint8 IndexBits>
	class SerialAndIndex
	{
	public:
		using UintType = UintT;
		static constexpr uint8 kIndexBits = IndexBits;

	public:
		SerialAndIndex() : _raw{ 0 } { SetInvalidIndex(); }
		SerialAndIndex(const SerialAndIndex& rhs) = default;
		SerialAndIndex(SerialAndIndex&& rhs) noexcept = default;
		~SerialAndIndex() = default;

	public:
		SerialAndIndex& operator=(const SerialAndIndex& rhs) = default;
		SerialAndIndex& operator=(SerialAndIndex&& rhs) noexcept = default;
		MINT_INLINE bool operator==(const SerialAndIndex& rhs) const { return _raw == rhs._raw; }
		MINT_INLINE bool operator==(SerialAndIndex&& rhs) const { return _raw == rhs._raw; }
		MINT_INLINE bool operator!=(const SerialAndIndex& rhs) const { return _raw != rhs._raw; }
		MINT_INLINE bool operator!=(SerialAndIndex&& rhs) const { return _raw != rhs._raw; }

	public:
		MINT_INLINE void SetInvalidIndex() { SetSerialAndIndex(GetSerial(), GetInvalidIndex()); }
		MINT_INLINE void SetSerialAndIndex(const UintT serial, const UintT index) { _raw = (serial << IndexBits) | index; }

	public:
		MINT_INLINE constexpr UintT GetInvalidIndex() const { return (static_cast<UintT>(1) << IndexBits) - 1; }
		MINT_INLINE bool IsValid() const { return HasValidSerial() == true && HasValidIndex() == true; }
		MINT_INLINE bool HasValidIndex() const { return GetIndex() != GetInvalidIndex(); }
		MINT_INLINE bool HasValidSerial() const { return GetSerial() != 0; }
		MINT_INLINE UintT GetIndex() const { return _raw & IndexBits; }
		MINT_INLINE UintT GetSerial() const { return _raw >> IndexBits; }
		MINT_INLINE UintT GetRawValue() const { return _raw; }

	protected:
		static_assert(IndexBits < sizeof(UintT) * 8, "SerialAndIndex: No room for serial bits!");
		std::enable_if_t<std::is_unsigned_v<UintT>, UintT> _raw;
	};
}


#endif // !_MINT_CONTAINER_SERIAL_AND_INDEX_H_
