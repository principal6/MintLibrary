#pragma once


#include <FsContainer/Include/Hash.h>

#include <FsContainer/Include/StringUtil.h>


namespace fs
{
    FS_INLINE const uint64 computeHash(const char* const rawString, const uint32 length) noexcept
    {
        // Hashing algorithm: FNV1a

        if (fs::StringUtil::isNullOrEmpty(rawString) == true)
        {
            return kUint64Max;
        }

        static constexpr uint64 kOffset = 0xcbf29ce484222325;
        static constexpr uint64 kPrime = 0x00000100000001B3;

        uint64 hash = kOffset;
        for (uint32 rawStringAt = 0; rawStringAt < length; ++rawStringAt)
        {
            hash ^= static_cast<uint8>(rawString[rawStringAt]);
            hash *= kPrime;
        }
        return hash;
    }

    FS_INLINE const uint64 computeHash(const char* const rawString) noexcept
    {
        const uint32 rawStringLength = fs::StringUtil::strlen(rawString);
        return computeHash(rawString, rawStringLength);
    }

    FS_INLINE const uint64 computeHash(const wchar_t* const rawString) noexcept
    {
        const uint32 rawStringLength = fs::StringUtil::wcslen(rawString);
        const char* const rawStringA = reinterpret_cast<const char*>(rawString);
        return computeHash(rawStringA, rawStringLength * 2);
    }

    template <typename T>
    FS_INLINE typename std::enable_if<std::is_arithmetic<T>::value, const uint64>::type computeHash(const T value) noexcept
    {
        const char* const str = reinterpret_cast<const char*>(&value);
        const uint32 length = sizeof(value);
        return computeHash(str, length);
    }

    template <typename T>
    FS_INLINE typename std::enable_if<std::is_pointer<T>::value, const uint64>::type computeHash(const T value) noexcept
    {
        const char* const str = reinterpret_cast<const char*>(&value);
        const uint32 length = sizeof(value);
        return computeHash(str, length);
    }

    template<typename T>
    inline const uint64 Hasher<T>::operator()(const T& value) const noexcept
    {
        return fs::computeHash<T>(value);
        //return uint64(value); // ### FOR TEST ###
    }

    inline const uint64 Hasher<std::string>::operator()(const std::string& value) const noexcept
    {
        return computeHash(value.c_str(), static_cast<uint32>(value.length()));
    }
}
