#pragma once


#ifndef MINT_SCOPE_STRING_H
#define MINT_SCOPE_STRING_H


#include <MintCommon/Include/CommonDefinitions.h>
#include <MintContainer/Include/StaticArray.h>


namespace mint
{
    template <typename T, uint32 BufferSize>
    class ScopeString
    {
    public:
                                            ScopeString();
                                            ScopeString(const T* const rawString);
                                            ScopeString(const ScopeString& rhs);
                                            ScopeString(ScopeString&& rhs) noexcept;
                                            ~ScopeString();

    public:
        ScopeString&                        operator=(const ScopeString& rhs) noexcept;
        ScopeString&                        operator=(ScopeString&& rhs) noexcept;
        ScopeString&                        operator=(const T* const rawString) noexcept;

    public:
        const bool                          operator==(const T* const rawString) const noexcept;
        const bool                          operator==(const ScopeString& rhs) const noexcept;
        const bool                          operator!=(const T* const rawString) const noexcept;
        const bool                          operator!=(const ScopeString& rhs) const noexcept;

    public:
        ScopeString&                        operator+=(const T* const rawString) noexcept;
        ScopeString&                        operator+=(const ScopeString& rhs) noexcept;

    public:
        T&                                  operator[](const uint32 at) noexcept;
        const T&                            operator[](const uint32 at) const noexcept;

    public:
        uint32                              capacity() const noexcept;
        uint32                              length() const noexcept;
        const T*                            c_str() const noexcept;

    private:
        static const uint32                 _getRawStringLength(const T* const rawString) noexcept;
        static void                         _copyString(T* const destination, const T* const source, const uint64 length) noexcept;

    public:
        T*                                  data() noexcept;

    private:
        const bool                          canInsert(const uint32 insertLength) const noexcept;

    public:
        void                                clear() noexcept;
        ScopeString&                        append(const T* const rawString) noexcept;
        ScopeString&                        append(const ScopeString& rhs) noexcept;
        ScopeString&                        assign(const T* const rawString) noexcept;
        ScopeString&                        assign(const ScopeString& rhs) noexcept;
        void                                resize(const uint32 newSize) noexcept;

    public:
        ScopeString                         substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
        const uint32                        find(const T* const rawString, const uint32 offset = kStringNPos) const noexcept;
        const uint32                        rfind(const T* const rawString, const uint32 offset = kStringNPos) const noexcept;
        const bool                          compare(const T* const rawString) const noexcept;
        const bool                          compare(const ScopeString& rhs) const noexcept;
        const uint64                        computeHash() const noexcept;

    private:
        uint32                              _length;
        StaticArray<T, BufferSize>          _raw;
    };


    template <uint32 BufferSize>
    using ScopeStringA = ScopeString<char, BufferSize>;
    
    template <uint32 BufferSize>
    using ScopeStringW = ScopeString<wchar_t, BufferSize>;
}


#endif // !MINT_SCOPE_STRING_H
