#include "StaticArray.h"
#pragma once


namespace mint
{
    template <typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(const std::initializer_list<T>& il)
    {
        const size_t    count = il.size();
        const T* const    first = il.begin();
        for (size_t index = 0; index < count; ++index)
        {
            _data[index] = *(first + index);
        }
    }
    
    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(T value)
    {
        _data[0] = value;
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(const T& s)
    {
        for (uint32 index = 0; index < maxSize; ++index)
        {
            _data[index] = s;
        }
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(const T* const arr, const uint32 arrSize)
    {
        for (uint32 index = 0; index < arrSize; ++index)
        {
            _data[index] = arr[index];
        }
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(const StaticArray& rhs)
    {
        for (uint32 index = 0; index < maxSize; ++index)
        {
            _data[index] = rhs._data[index];
        }
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>::StaticArray(StaticArray&& rhs) noexcept
    {
        std::swap(_data, rhs._data);
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>& StaticArray<T, maxSize>::operator=(const StaticArray& rhs)
    {
        for (uint32 index = 0; index < maxSize; ++index)
        {
            _data[index] = rhs._data[index];
        }
        return *this;
    }

    template<typename T, uint32 maxSize>
    inline constexpr StaticArray<T, maxSize>& StaticArray<T, maxSize>::operator=(StaticArray&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(_data, rhs._data);
        }
        return *this;
    }

    template<typename T, uint32 maxSize>
    inline constexpr T& StaticArray<T, maxSize>::operator[](const uint32 index) noexcept
    {
        MINT_ASSERT("김장원", index < maxSize, "범위를 벗어난 접근입니다.");
        return _data[index];
    }

    template<typename T, uint32 maxSize>
    inline constexpr const T& StaticArray<T, maxSize>::operator[](const uint32 index) const noexcept
    {
        MINT_ASSERT("김장원", index < maxSize, "범위를 벗어난 접근입니다.");
        return _data[index];
    }

    template<typename T, uint32 maxSize>
    inline constexpr uint32 StaticArray<T, maxSize>::size() const noexcept
    {
        return maxSize;
    }
}
