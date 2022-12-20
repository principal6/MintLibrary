#pragma once


#include <MintContainer/Include/Hash.h>

#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StringUtil.h>


namespace mint
{
    MINT_INLINE uint64 computeHash(const char* const rhs, const uint32 length) noexcept
    {
        // Hashing algorithm: FNV1a

        if (StringUtil::isNullOrEmpty(rhs) == true)
        {
            return kUint64Max;
        }

        static constexpr uint64 kOffset = 0xcbf29ce484222325;
        static constexpr uint64 kPrime = 0x00000100000001B3;

        uint64 hash = kOffset;
        for (uint32 rawStringAt = 0; rawStringAt < length; ++rawStringAt)
        {
            hash ^= static_cast<uint8>(rhs[rawStringAt]);
            hash *= kPrime;
        }
        return hash;
    }

    MINT_INLINE uint64 computeHash(const char* const rhs) noexcept
    {
        const uint32 rhsLength = StringUtil::length(rhs);
        return computeHash(rhs, rhsLength);
    }

    MINT_INLINE uint64 computeHash(const wchar_t* const rhs) noexcept
    {
        const uint32 rhsLength = StringUtil::length(rhs);
        const char* const rhsA = reinterpret_cast<const char*>(rhs);
        return computeHash(rhsA, rhsLength * 2);
    }

    template <typename T>
    uint64 computeHash(const T& value) noexcept
    {
        if constexpr (std::is_arithmetic<T>::value == true)
        {
            const char* const str = reinterpret_cast<const char*>(&value);
            const uint32 length = sizeof(value);
            return computeHash(str, length);
        }
        else if constexpr (std::is_pointer<T>::value == true)
        {
            const char* const str = reinterpret_cast<const char*>(&value);
            const uint32 length = sizeof(value);
            return computeHash(str, length);
        }

        MINT_ASSERT(false, "Hash computation not implemented for this type!");
        return 0;
    }


    template<typename T>
    inline uint64 Hasher<T>::operator()(const T& value) const noexcept
    {
        return computeHash<T>(value);
        //return uint64(value); // ### FOR TEST ###
    }

    inline uint64 Hasher<std::string>::operator()(const std::string& value) const noexcept
    {
        return computeHash(value.c_str(), static_cast<uint32>(value.length()));
    }

    template<typename T>
    inline uint64 Hasher<String<T>>::operator()(const String<T>& value) const noexcept
    {
        return value.computeHash();
    }
}
