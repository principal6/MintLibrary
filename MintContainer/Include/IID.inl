#pragma once


namespace mint
{
	MINT_INLINE bool IID::operator==(const IID& rhs) const noexcept
	{
		return _rawID == rhs._rawID;
	}

	MINT_INLINE bool IID::operator!=(const IID& rhs) const noexcept
	{
		return _rawID != rhs._rawID;
	}

	MINT_INLINE bool IID::operator<(const IID& rhs) const noexcept
	{
		return _rawID < rhs._rawID;
	}

	MINT_INLINE bool IID::operator>(const IID& rhs) const noexcept
	{
		return _rawID > rhs._rawID;
	}

	MINT_INLINE bool IID::IsValid() const noexcept
	{
		return _rawID != kInvalidRawID;
	}
}
