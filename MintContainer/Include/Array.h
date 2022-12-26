#pragma once


#ifndef _MINT_CONTAINER_ARRAY_H_
#define _MINT_CONTAINER_ARRAY_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <initializer_list>


namespace mint
{
    template <typename T, uint32 maxSize>
    class Array
    {
        static_assert((sizeof(T)* maxSize) < kStackSizeLimit, "Stack size is too large!!! Consider using heap allocationn.");

    public:
        constexpr           Array() = default;
        constexpr           Array(const std::initializer_list<T>& il);
        constexpr           Array(T value);
        constexpr           Array(const T& s);        
        constexpr           Array(const T* const arr, const uint32 arrSize); // Compiler checks overflow
        constexpr           Array(const Array& rhs);
        constexpr           Array(Array&& rhs) noexcept;
                            ~Array() = default;

    public:
        constexpr Array&    operator=(const Array& rhs);
        constexpr Array&    operator=(Array&& rhs) noexcept;

    public:
        constexpr T&        operator[](const uint32 index) noexcept;
        constexpr const T&  operator[](const uint32 index) const noexcept;

    public:
        constexpr uint32    size() const noexcept;

    private:
        T                   _data[maxSize]{};
    };
}


#include <MintContainer/Include/Array.inl>


#endif // !_MINT_CONTAINER_ARRAY_H_
