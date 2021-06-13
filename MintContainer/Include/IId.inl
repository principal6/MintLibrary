#pragma once


namespace mint
{
    MINT_INLINE const bool IId::operator==(const IId& rhs) const noexcept
    {
        return _rawId == rhs._rawId;
    }

    MINT_INLINE const bool IId::operator!=(const IId& rhs) const noexcept
    {
        return _rawId != rhs._rawId;
    }

    MINT_INLINE const bool IId::operator<(const IId& rhs) const noexcept
    {
        return _rawId < rhs._rawId;
    }

    MINT_INLINE const bool IId::operator>(const IId& rhs) const noexcept
    {
        return _rawId > rhs._rawId;
    }

    MINT_INLINE const bool IId::isValid() const noexcept
    {
        return _rawId != kInvalidRawId;
    }
}
