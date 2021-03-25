﻿#pragma once


#ifndef FS_STATIC_ARRAY_H
#define FS_STATIC_ARRAY_H


#include <CommonDefinitions.h>

#include <initializer_list>
#include <utility>


namespace fs
{
    template <typename T, uint32 maxSize>
    class StaticArray
    {
        static_assert((sizeof(T)* maxSize) < kStackSizeLimit, "Stack size is too large!!! Consider using heap allocationn.");

    public:
        constexpr               StaticArray() = default;
        constexpr               StaticArray(const std::initializer_list<T>& il);
        constexpr               StaticArray(T value);
        constexpr               StaticArray(const T& s);        
        constexpr               StaticArray(const T* const arr, const uint32 arrSize); // Compiler checks overflow
        constexpr               StaticArray(const StaticArray& rhs);
        constexpr               StaticArray(StaticArray&& rhs) noexcept;
                                ~StaticArray() = default;

    public:
        constexpr StaticArray&  operator=(const StaticArray& rhs);
        constexpr StaticArray&  operator=(StaticArray&& rhs) noexcept;

    public:
        constexpr T&            operator[](const uint32 index) noexcept;
        constexpr const T&      operator[](const uint32 index) const noexcept;

    public:
        constexpr uint32        size() const noexcept;

    private:
        T                       _data[maxSize]{};
    };
}


#include <FsContainer/Include/StaticArray.inl>


#endif // !FS_STATIC_ARRAY_H
