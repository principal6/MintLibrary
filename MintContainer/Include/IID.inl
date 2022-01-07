#pragma once


namespace mint
{
    MINT_INLINE const bool IID::operator==(const IID& rhs) const noexcept
    {
        return _rawId == rhs._rawId;
    }

    MINT_INLINE const bool IID::operator!=(const IID& rhs) const noexcept
    {
        return _rawId != rhs._rawId;
    }

    MINT_INLINE const bool IID::operator<(const IID& rhs) const noexcept
    {
        return _rawId < rhs._rawId;
    }

    MINT_INLINE const bool IID::operator>(const IID& rhs) const noexcept
    {
        return _rawId > rhs._rawId;
    }

    MINT_INLINE const bool IID::isValid() const noexcept
    {
        return _rawId != kInvalidRawId;
    }
}
