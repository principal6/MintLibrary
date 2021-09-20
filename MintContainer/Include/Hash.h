#pragma once


#ifndef MINT_HASH_H
#define MINT_HASH_H


#include <type_traits>
#include <string>

#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/String.h>


namespace mint
{
    const uint64    computeHash(const char* const rawString, const uint32 length) noexcept;
    const uint64    computeHash(const char* const rawString) noexcept;
    const uint64    computeHash(const wchar_t* const rawString) noexcept;

    template <typename T>
    std::enable_if_t<std::is_arithmetic<T>::value, const uint64> computeHash(const T value) noexcept;

    template <typename T>
    std::enable_if_t<std::is_pointer<T>::value, const uint64> computeHash(const T value) noexcept;


    template <typename T>
    struct Hasher final
    {
        const uint64 operator()(const T& value) const noexcept;
    };

    template <>
    struct Hasher<std::string> final
    {
        const uint64 operator()(const std::string& value) const noexcept;
    };

    template <typename T>
    struct Hasher<String<T>> final
    {
        const uint64 operator()(const String<T>& value) const noexcept;
    };
}


#endif // !MINT_HASH_H
