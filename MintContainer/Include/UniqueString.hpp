#pragma once

#include <MintContainer/Include/UniqueString.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
    inline UniqueStringAId::UniqueStringAId()
        : _rawID{ UniqueStringAId::kInvalidRawID }
    {
        __noop;
    }

    inline UniqueStringAId::UniqueStringAId(const uint32 newRawID)
        : _rawID{ newRawID }
    {
        __noop;
    }

    MINT_INLINE const bool UniqueStringAId::operator==(const UniqueStringAId& rhs) const noexcept
    {
        return _rawID == rhs._rawID;
    }

    MINT_INLINE const bool UniqueStringAId::operator!=(const UniqueStringAId& rhs) const noexcept
    {
        return _rawID != rhs._rawID;
    }


    MINT_INLINE const bool UniqueStringA::operator==(const UniqueStringA& rhs) const noexcept
    {
        return _id == rhs._id;
    }

    MINT_INLINE const bool UniqueStringA::operator!=(const UniqueStringA& rhs) const noexcept
    {
        return _id != rhs._id;
    }

    MINT_INLINE const char* UniqueStringA::c_str() const noexcept
    {
        return _pool.getRawString(_id);
    }

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    MINT_INLINE const UniqueStringAId UniqueStringA::getId() const noexcept
    {
        return _id;
    }
#endif


    MINT_INLINE const bool UniqueStringPoolA::isValid(const UniqueStringAId id) const noexcept
    {
        return (id._rawID < _uniqueStringCount);
    }

    MINT_INLINE const char* UniqueStringPoolA::getRawString(const UniqueStringAId id) const noexcept
    {
        if (isValid(id) == false)
        {
            return nullptr;
        }
        return &_rawMemory[_offsetArray[id._rawID]];;
    }
}
