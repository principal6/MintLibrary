#pragma once


#ifndef _MINT_CONTAINER_SERIAL_AND_INDEX_H_
#define _MINT_CONTAINER_SERIAL_AND_INDEX_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T, uint8 IndexBits>
	class SerialAndIndex
	{
	public:
		SerialAndIndex() : _raw{ 0 } { SetInvalidIndex(); }
		SerialAndIndex(const SerialAndIndex& rhs) = default;
		SerialAndIndex(SerialAndIndex&& rhs) noexcept = default;
		virtual ~SerialAndIndex() = default;

	public:
		SerialAndIndex& operator=(const SerialAndIndex& rhs) = default;
		SerialAndIndex& operator=(SerialAndIndex&& rhs) noexcept = default;
		MINT_INLINE bool operator==(const SerialAndIndex& rhs) const { return _raw == rhs._raw; }
		MINT_INLINE bool operator==(SerialAndIndex&& rhs) const { return _raw == rhs._raw; }
		MINT_INLINE bool operator!=(const SerialAndIndex& rhs) const { return _raw != rhs._raw; }
		MINT_INLINE bool operator!=(SerialAndIndex&& rhs) const { return _raw != rhs._raw; }

	public:
		MINT_INLINE void SetInvalidIndex() { SetSerialAndIndex(GetSerial(), GetInvalidIndex()); }
		MINT_INLINE void SetSerialAndIndex(const T serial, const T index) { _raw = (serial << IndexBits) | index; }

	public:
		MINT_INLINE constexpr T GetInvalidIndex() const { return (static_cast<T>(1) << IndexBits) - 1; }
		MINT_INLINE bool IsValid() const { return HasValidSerial() == true && HasValidIndex() == true; }
		MINT_INLINE bool HasValidIndex() const { return GetIndex() != GetInvalidIndex(); }
		MINT_INLINE bool HasValidSerial() const { return GetSerial() != 0; }
		MINT_INLINE T GetIndex() const { return _raw & IndexBits; }
		MINT_INLINE T GetSerial() const { return _raw >> IndexBits; }
		MINT_INLINE T GetRawValue() const { return _raw; }

	protected:
		static_assert(IndexBits < sizeof(T) * 8, "SerialAndIndex: No room for serial bits!");
		T _raw;
	};
}


#endif // !_MINT_CONTAINER_SERIAL_AND_INDEX_H_
