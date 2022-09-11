#pragma once


#ifndef MINT_SCOPE_STRING_H
#define MINT_SCOPE_STRING_H


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/StaticArray.h>


namespace mint
{
    template <typename T, uint32 BufferSize>
    class StackString
    {
    public:
                                    StackString();
                                    StackString(const T* const rawString);
                                    StackString(const StackString& rhs);
                                    StackString(StackString&& rhs) noexcept;
                                    ~StackString();

    public:
        StackString&                operator=(const StackString& rhs) noexcept;
        StackString&                operator=(StackString&& rhs) noexcept;
        StackString&                operator=(const T* const rawString) noexcept;

    public:
        bool                        operator==(const T* const rawString) const noexcept;
        bool                        operator==(const StackString& rhs) const noexcept;
        bool                        operator!=(const T* const rawString) const noexcept;
        bool                        operator!=(const StackString& rhs) const noexcept;

    public:
        StackString&                operator+=(const T* const rawString) noexcept;
        StackString&                operator+=(const StackString& rhs) noexcept;

    public:
        T&                          operator[](const uint32 at) noexcept;
        const T&                    operator[](const uint32 at) const noexcept;

    public:
        uint32                      capacity() const noexcept;
        uint32                      length() const noexcept;
        const T*                    c_str() const noexcept;

    private:
        static uint32               _getRawStringLength(const T* const rawString) noexcept;
        static void                 _copyString(T* const destination, const T* const source, const uint64 length) noexcept;

    public:
        T*                          data() noexcept;

    private:
        bool                        canInsert(const uint32 insertLength) const noexcept;

    public:
        void                        clear() noexcept;
        StackString&                append(const T* const rawString) noexcept;
        StackString&                append(const StackString& rhs) noexcept;
        StackString&                assign(const T* const rawString) noexcept;
        StackString&                assign(const StackString& rhs) noexcept;
        void                        resize(const uint32 newSize) noexcept;

    public:
        StackString                 substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
        uint32                      find(const T* const rawString, const uint32 offset = kStringNPos) const noexcept;
        uint32                      rfind(const T* const rawString, const uint32 offset = kStringNPos) const noexcept;
        bool                        compare(const T* const rawString) const noexcept;
        bool                        compare(const StackString& rhs) const noexcept;
        uint64                      computeHash() const noexcept;

    private:
        uint32                      _length;
        StaticArray<T, BufferSize>  _raw;
    };


    template <uint32 BufferSize>
    using StackStringA = StackString<char, BufferSize>;
    
    template <uint32 BufferSize>
    using StackStringW = StackString<wchar_t, BufferSize>;
}


#endif // !MINT_SCOPE_STRING_H
