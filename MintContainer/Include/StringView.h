#pragma once


#ifndef MINT_CONTAINER_STRING_VIEW_H
#define MINT_CONTAINER_STRING_VIEW_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    template<typename T>
    class String;

    template<typename T, uint32 BufferSize>
    class StackString;

    template<typename T>
    class UniqueString;


    template<typename T>
    class StringView
    {
    public:
                    StringView() = default;
                    StringView(const String<T>& string);
                    template <uint32 BufferSize>
                    StringView(const StackString<T, BufferSize>& string);
                    StringView(const UniqueString<T>& string);
                    ~StringView() = default;

    public:
        bool        operator==(const StringView& rhs) const noexcept;
        bool        operator!=(const StringView& rhs) const noexcept;
        const T&    operator[](const uint32 at) const noexcept;
        const T*    c_str() const noexcept;

    private:
        const T*    _rawString = nullptr;
        uint32      _length = 0;
    };


    using StringViewA = StringView<char>;
    using StringViewW = StringView<wchar_t>;
}


#endif // !MINT_CONTAINER_STRING_VIEW_H
