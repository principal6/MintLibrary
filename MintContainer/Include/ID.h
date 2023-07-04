#pragma once


#ifndef _MINT_CONTAINER_ID_H_
#define _MINT_CONTAINER_ID_H_


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
	template<typename T, T InvalidValue>
	class ID abstract
	{
	public:
		using RawType = T;
		ID() : _raw{ InvalidValue } { __noop; }
		ID(const ID& rhs) = default;
		ID(ID&& rhs) = default;
		virtual ~ID() = default;
		ID& operator=(const ID& rhs) noexcept = default;
		ID& operator=(ID&& rhs) noexcept = default;
		bool operator==(const ID& rhs) const noexcept { return _raw == rhs._raw; }
		bool operator!=(const ID& rhs) const noexcept { return _raw != rhs._raw; }
		// needed for sort
		bool operator<(const ID& rhs) const noexcept { return _raw < rhs._raw; }
		// needed for sort
		bool operator>(const ID& rhs) const noexcept { return _raw > rhs._raw; }
	public:
		bool IsValid() const noexcept { return _raw != InvalidValue; }
		T Value() const noexcept { return _raw; }
		void Invalidate() noexcept { _raw = InvalidValue; }
	protected:
		void Assign(T raw) { _raw = raw; }
	private:
		T _raw;
	};

	using ID8 = ID<uint8, kUint8Max>;
	using ID16 = ID<uint16, kUint16Max>;
	using ID32 = ID<uint32, kUint32Max>;
	using ID64 = ID<uint64, kUint64Max>;
}


#endif // !_MINT_CONTAINER_ID_H_
