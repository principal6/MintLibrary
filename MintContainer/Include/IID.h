#pragma once


#ifndef _MINT_CONTAINER_IID_H_
#define _MINT_CONTAINER_IID_H_


namespace mint
{
	class IID abstract
	{
		static constexpr uint32 kInvalidRawID{ kUint32Max };

	public:
		IID() = default;
		IID(const IID& rhs) = default;
		IID(IID&& rhs) = default;
		virtual ~IID() = default;

	public:
		IID& operator=(const IID& rhs) noexcept = default;
		IID& operator=(IID&& rhs) noexcept = default;

	public:
		bool operator==(const IID& rhs) const noexcept;
		bool operator!=(const IID& rhs) const noexcept;
		bool operator<(const IID& rhs) const noexcept;
		bool operator>(const IID& rhs) const noexcept;

	public:
		bool IsValid() const noexcept;

	protected:
		virtual void AssignRawID(const uint32 rawID) noexcept abstract;

	protected:
		uint32 _rawID{ kInvalidRawID };
	};
}


#include <MintContainer/Include/IID.inl>


#endif // !_MINT_CONTAINER_IID_H_
