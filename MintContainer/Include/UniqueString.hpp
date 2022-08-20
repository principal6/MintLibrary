#pragma once

#include <MintContainer/Include/UniqueString.h>

#include <MintContainer/Include/StringUtil.h>

#include <MintContainer/Include/Vector.hpp>


namespace mint
{
    inline UniqueStringAID::UniqueStringAID()
        : _rawID{ UniqueStringAID::kInvalidRawID }
    {
        __noop;
    }

    inline UniqueStringAID::UniqueStringAID(const uint32 newRawID)
        : _rawID{ newRawID }
    {
        __noop;
    }

    MINT_INLINE bool UniqueStringAID::operator==(const UniqueStringAID& rhs) const noexcept
    {
        return _rawID == rhs._rawID;
    }

    MINT_INLINE bool UniqueStringAID::operator!=(const UniqueStringAID& rhs) const noexcept
    {
        return _rawID != rhs._rawID;
    }


    MINT_INLINE bool UniqueStringA::operator==(const UniqueStringA& rhs) const noexcept
    {
        return _id == rhs._id;
    }

    MINT_INLINE bool UniqueStringA::operator!=(const UniqueStringA& rhs) const noexcept
    {
        return _id != rhs._id;
    }

    MINT_INLINE const char* UniqueStringA::c_str() const noexcept
    {
        return _pool.getRawString(_id);
    }

#if defined MINT_UNIQUE_STRING_EXPOSE_ID
    MINT_INLINE UniqueStringAID UniqueStringA::getID() const noexcept
    {
        return _id;
    }
#endif


    MINT_INLINE bool UniqueStringPoolA::isValid(const UniqueStringAID id) const noexcept
    {
        return (id._rawID < _uniqueStringCount);
    }

    MINT_INLINE const char* UniqueStringPoolA::getRawString(const UniqueStringAID id) const noexcept
    {
        if (isValid(id) == false)
        {
            return nullptr;
        }
        return &_rawMemory[_offsetArray[id._rawID]];;
    }
}
