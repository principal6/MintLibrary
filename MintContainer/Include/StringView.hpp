#pragma once


#ifndef MINT_CONTAINER_STRING_VIEW_HPP
#define MINT_CONTAINER_STRING_VIEW_HPP


#include <MintContainer/Include/StringView.h>
#include <MintContainer/Include/String.hpp>
#include <MintContainer/Include/StackString.hpp>
#include <MintContainer/Include/UniqueString.hpp>


namespace mint
{
    template<typename T>
    inline StringView<T>::StringView(const T* const string)
    {
        _rawString = string;
        _length = StringUtil::length(string);
    }
    
    template<typename T>
    inline StringView<T>::StringView(const String<T>& string)
    {
        _rawString = string.c_str();
        _length = string.length();
    }
    
    template<typename T>
    template<uint32 BufferSize>
    inline StringView<T>::StringView(const StackString<T, BufferSize>& string)
    {
        _rawString = string.c_str();
        _length = string.length();
    }
    
    template<typename T>
    inline StringView<T>::StringView(const UniqueString<T>& string)
    {
        _rawString = string.c_str();
        _length = string.length();
    }

    template<typename T>
    bool StringView<T>::operator==(const StringView& rhs) const noexcept
    {
        return StringUtil::compare(_rawString, rhs._rawString);
    }
    
    template<typename T>
    bool StringView<T>::operator!=(const StringView& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    template<typename T>
    const T& StringView<T>::operator[](const uint32 at) const noexcept
    {
        return _rawString[at];
    }

    template<typename T>
    const T* StringView<T>::c_str() const noexcept
    {
        return _rawString;
    }
}


#endif // !MINT_CONTAINER_STRING_VIEW_HPP
