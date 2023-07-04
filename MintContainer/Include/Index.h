#pragma once


#ifndef _MINT_CONTAINER_INDEX_H_
#define _MINT_CONTAINER_INDEX_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T, T InvalidValue>
	class Index
	{
		T _raw;
	public:
		using RawType = T;
		Index() : _raw{ InvalidValue } { __noop; }
		Index(T raw) { _raw = raw; }
		Index(const Index& rhs) = default;
		Index(Index&& rhs) noexcept = default;
		Index& operator=(const Index& rhs) = default;
		Index& operator=(Index&& rhs) noexcept = default;
		Index& operator++() { ++_raw; return *this; }
		Index& operator--() { --_raw; return *this; }
		void operator=(T raw) { _raw = raw; }
		bool operator==(const Index& rhs) const { return _raw == rhs._raw; }
		bool operator==(Index&& rhs) const { return _raw == rhs._raw; }
		bool operator!=(const Index& rhs) const { return _raw != rhs._raw; }
		bool operator!=(Index&& rhs) const { return _raw != rhs._raw; }
		bool operator<(const Index& rhs) const { return _raw < rhs._raw; }
		bool operator<=(const Index& rhs) const { return _raw <= rhs._raw; }
		bool operator>(const Index& rhs) const { return _raw > rhs._raw; }
		bool operator>=(const Index& rhs) const { return _raw >= rhs._raw; }
		// implicit type conversion could be dangerous...
		explicit operator T() const { return _raw; }
	public:
		bool IsValid() const { return _raw != InvalidValue; }
		void Invalidate() { _raw = InvalidValue; }
		T Value() const { return _raw; }
	};

	using Index8 = Index<uint8, kUint8Max>;
	using Index16 = Index<uint16, kUint16Max>;
	using Index32 = Index<uint32, kUint32Max>;
	using Index64 = Index<uint64, kUint64Max>;
}


#endif // !_MINT_CONTAINER_INDEX_H_
