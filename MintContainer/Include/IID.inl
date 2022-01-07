#pragma once


namespace mint
{
    MINT_INLINE const bool IID::operator==(const IID& rhs) const noexcept
    {
        return _rawID == rhs._rawID;
    }

    MINT_INLINE const bool IID::operator!=(const IID& rhs) const noexcept
    {
        return _rawID != rhs._rawID;
    }

    MINT_INLINE const bool IID::operator<(const IID& rhs) const noexcept
    {
        return _rawID < rhs._rawID;
    }

    MINT_INLINE const bool IID::operator>(const IID& rhs) const noexcept
    {
        return _rawID > rhs._rawID;
    }

    MINT_INLINE const bool IID::isValid() const noexcept
    {
        return _rawID != kInvalidRawID;
    }
}
