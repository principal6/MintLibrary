#pragma once


#ifndef FS_HASH_H
#define FS_HASH_H


#include <FsCommon/Include/CommonDefinitions.h>


namespace fs
{
    const uint64    computeHash(const char* const rawString, const uint32 length) noexcept;
    const uint64    computeHash(const char* const rawString) noexcept;
    const uint64    computeHash(const wchar_t* const rawString) noexcept;

    template <typename T>
    typename std::enable_if<std::is_arithmetic<T>::value, const uint64>::type computeHash(const T value) noexcept;


    template <typename T>
    struct Hasher final
    {
        typename std::enable_if<std::is_arithmetic<T>::value, const uint64>::type operator()(const T& value) const noexcept;
    };

    template <>
    struct Hasher<std::string> final
    {
        const uint64 operator()(const std::string& value) const noexcept;
    };
}


#endif // !FS_HASH_H
