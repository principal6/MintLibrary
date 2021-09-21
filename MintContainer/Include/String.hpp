#pragma once


#ifndef MINT_CONTAINER_STRING_HPP
#define MINT_CONTAINER_STRING_HPP


#include <MintContainer/Include/String.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/MemoryRaw.hpp>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
    template <typename T>
    inline String<T>::String()
    {
        __noop;
    }

    template<typename T>
    inline String<T>::String(const T* const rhs)
    {
        assignInternalXXX(rhs);
    }

    template<typename T>
    inline String<T>::String(const String& rhs)
    {
        assignInternalXXX(rhs.c_str());
    }

    template<typename T>
    inline String<T>::String(String&& rhs) noexcept
    {
        _long = rhs._long;
        
        rhs._long._capacity = 0;
        rhs._long._size = 0;
        rhs._long._rawPointer = nullptr;
    }

    template <typename T>
    inline String<T>::~String()
    {
        release();
    }

    template <typename T>
    MINT_INLINE String<T>& String<T>::operator=(const T* const rhs) noexcept
    {
        return assign(rhs);
    }

    template<typename T>
    MINT_INLINE String<T>& String<T>::operator=(const String& rhs) noexcept
    {
        if (this != &rhs)
        {
            assignInternalXXX(rhs.c_str());
        }
        return *this;
    }

    template<typename T>
    MINT_INLINE String<T>& String<T>::operator=(String&& rhs) noexcept
    {
        if (this != &rhs)
        {
            _long = rhs._long;

            rhs._long._capacity = 0;
            rhs._long._size = 0;
            rhs._long._rawPointer = nullptr;
        }
        return *this;
    }

    template<typename T>
    MINT_INLINE String<T>& String<T>::operator+=(const T* const rhs) noexcept
    {
        return append(rhs);
    }

    template<typename T>
    MINT_INLINE String<T>& String<T>::operator+=(const String& rhs) noexcept
    {
        return append(rhs.c_str());
    }

    template<typename T>
    MINT_INLINE const bool String<T>::operator==(const T* const rhs) const noexcept
    {
        return compare(rhs);
    }

    template<typename T>
    MINT_INLINE const bool String<T>::operator==(const String& rhs) const noexcept
    {
        return compare(rhs);
    }

    template<typename T>
    inline const bool String<T>::operator!=(const T* const rhs) const noexcept
    {
        return !compare(rhs);
    }

    template<typename T>
    inline const bool String<T>::operator!=(const String& rhs) const noexcept
    {
        return !compare(rhs);
    }

    template<typename T>
    MINT_INLINE T& String<T>::operator[](const uint32 at) noexcept
    {
        return data()[at];
    }

    template<typename T>
    MINT_INLINE const T& String<T>::operator[](const uint32 at) const noexcept
    {
        return c_str()[at];
    }

    template<typename T>
    inline String<T>& String<T>::assign(const T* const rawString) noexcept
    {
        release();

        return assignInternalXXX(rawString);
    }

    template<typename T>
    inline String<T>& String<T>::assignInternalXXX(const T* const rawString) noexcept
    {
        const uint32 length = __getStringLength(rawString);
        if (length < Short::kSmallStringCapacity)
        {
            _short._size = length;
            __copyString(_short._smallString, rawString, length);
            return *this;
        }
        return assignInternalLongXXX(rawString);
    }

    template<typename T>
    inline String<T>& String<T>::assignInternalLongXXX(const T* const rawString) noexcept
    {
        const uint32 length = __getStringLength(rawString);
        _long._size = length;
        _long._capacity = _long._size + 1;

        _long._rawPointer = MemoryRaw::allocateMemory<T>(capacity());
        __copyString(_long._rawPointer, rawString, _long._size);
        return *this;
    }

    template<typename T>
    inline String<T>& String<T>::append(const T* const rawString) noexcept
    {
        if (true == isNotAllocated())
        {
            return assign(rawString);
        }

        if (true == isSmallString())
        {
            return appendInternalSmallXXX(rawString);
        }
        return appendInternalLongXXX(rawString);
    }

    template<typename T>
    inline String<T>& String<T>::appendInternalSmallXXX(const T* const rawString) noexcept
    {
        const uint32 length = __getStringLength(rawString);
        const uint64 newLength = _short._size + length;
        if (newLength < Short::kSmallStringCapacity)
        {
            __copyString(&_short._smallString[_short._size], rawString, length);
            _short._size = static_cast<T>(newLength);
            return *this;
        }

        reserve(max(capacity() * 2, static_cast<uint32>(newLength + 1)));
        return appendInternalLongXXX(rawString);
    }

    template<typename T>
    inline String<T>& String<T>::appendInternalLongXXX(const T* const rawString) noexcept
    {
        const uint32 length = __getStringLength(rawString);
        const uint64 newLength = _long._size + length;
        if (_long._capacity <= newLength)
        {
            reserve(max(static_cast<uint32>(_long._capacity * 2), static_cast<uint32>(newLength + 1)));
        }

        __copyString(&_long._rawPointer[_long._size], rawString, length);
        _long._size = newLength;
        return *this;
    }

    template<typename T>
    inline void String<T>::reserve(const uint32 newCapacity) noexcept
    {
        if (newCapacity <= capacity() || newCapacity <= Short::kSmallStringCapacity)
        {
            return;
        }

        //if (Long::kStringMaxCapacity <= newCapacity)
        //{
        //    MINT_NEVER;
        //}

        const uint32 oldLength = length();
        T* temp = MemoryRaw::allocateMemory<T>(oldLength + 1);
        __copyString(temp, (true == isSmallString()) ? _short._smallString : _long._rawPointer, oldLength);

        release();
        
        _long._rawPointer = MemoryRaw::allocateMemory<T>(newCapacity);
        __copyString(_long._rawPointer, temp, oldLength);
        _long._capacity = newCapacity;
        _long._size = oldLength;

        MemoryRaw::deallocateMemory(temp);
    }

    template<typename T>
    inline void String<T>::resize(const uint32 newSize, const T fillCharacter) noexcept
    {
        if (capacity() <= newSize)
        {
            reserve(newSize + 1);
        }

        T* const dataPointer = data();
        const bool isGrowing = (size() < newSize);
        if (isGrowing == true)
        {
            const uint32 oldSize = size();
            for (uint32 iter = oldSize; iter < newSize; ++iter)
            {
                dataPointer[iter] = fillCharacter;
            }
        }
        dataPointer[newSize] = 0;

        if (isSmallString() == true)
        {
            _short._size = newSize;
        }
        else
        {
            _long._size = newSize;
        }
    }

    template<typename T>
    MINT_INLINE void String<T>::clear() noexcept
    {
        if (true == isSmallString())
        {
            _short._size = 0;
            _short._smallString[0] = 0;
        }
        else
        {
            ::memset(_long._rawPointer, 0, kTypeSize);
            _long._size = 0;
        }
    }

    template<typename T>
    MINT_INLINE const T* String<T>::c_str() const noexcept
    {
        return (isSmallString() == true) ? _short._smallString : _long._rawPointer;
    }

    template<typename T>
    MINT_INLINE T* String<T>::data() noexcept
    {
        return (isSmallString() == true) ? _short._smallString : _long._rawPointer;
    }

    template<typename T>
    MINT_INLINE void String<T>::__copyString(T* const destination, const T* const source, const uint64 length) noexcept
    {
        ::memcpy(destination, source, length * kTypeSize);
        destination[length] = 0;
    }

    template<typename T>
    MINT_INLINE const uint32 String<T>::__getStringLength(const T* const rawString) const noexcept
    {
        if constexpr (1 == kTypeSize)
        {
            return StringUtil::strlen(rawString);
        }
        else
        {
            return StringUtil::wcslen(rawString);
        }

        MINT_NEVER;
    }

    template<typename T>
    inline const uint32 String<T>::find(const T* const target, const uint32 offset) const noexcept
    {
        const uint32 sourceLength = length();
        const uint32 targetLength = __getStringLength(target);
        if (sourceLength < offset + targetLength)
        {
            return kStringNPos;
        }

        const T* const dataPointer = c_str();
        uint32 result = kStringNPos;
        uint32 targetIter = 0;
        for (uint32 sourceIter = offset; sourceIter < sourceLength; ++sourceIter)
        {
            if (dataPointer[sourceIter] == target[targetIter])
            {
                if (targetIter == 0)
                {
                    result = sourceIter;
                }
                
                ++targetIter;
                if (targetIter == targetLength)
                {
                    break;
                }
            }
            else
            {
                targetIter = 0;
                result = kStringNPos;
            }
        }
        return result;
    }

    template<typename T>
    inline String<T> String<T>::substr(const uint32 offset, const uint32 count) const noexcept
    {
        String result;
        const T* const dataPointer = c_str();
        const uint32 stringLength = length();
        if (offset < stringLength && 0 < count)
        {
            const uint32 substringLength = min(count, stringLength - offset);
            result.resize(substringLength);
            for (uint32 iter = 0; iter < substringLength; ++iter)
            {
                result[iter] = dataPointer[offset + iter];
            }
        }
        return result;
    }

    template<typename T>
    inline const bool String<T>::compare(const T* const rhs) const noexcept
    {
        const uint32 lhsLength = length();
        const uint32 rhsLength = __getStringLength(rhs);
        if (lhsLength != rhsLength)
        {
            return false;
        }

        const T* const lhs = c_str();
        for (uint32 iter = 0; iter < lhsLength; ++iter)
        {
            if (lhs[iter] != rhs[iter])
            {
                return false;
            }
        }
        return true;
    }

    template<typename T>
    inline const bool String<T>::compare(const String& rhs) const noexcept
    {
        return compare(rhs.c_str());
    }

    template<typename T>
    inline const uint64 String<T>::computeHash() const noexcept
    {
        return mint::computeHash(c_str());
    }

    template<typename T>
    inline void String<T>::release() noexcept
    {
        if (false == isSmallString())
        {
            MemoryRaw::deallocateMemory<T>(_long._rawPointer);
        }

        _long._size = 0;
        _long._capacity = 0;
    }

    template<typename T>
    inline void String<T>::toLongString() noexcept
    {
        if (false == isSmallString() || true == isEmpty())
        {
            return;
        }

        Short tempShort;
        ::memcpy_s(&tempShort, sizeof(Short), &_short, sizeof(Short));
        
        assignInternalLongXXX(tempShort._smallString);
    }
}


#endif // !MINT_CONTAINER_STRING_HPP
