#pragma once


#ifndef MINT_SCOPE_STRING_H
#define MINT_SCOPE_STRING_H


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintContainer/Include/Array.h>
#include <MintContainer/Include/StringBase.h>


namespace mint
{
    template <typename T, uint32 BufferSize>
    class StackString : public MutableString<T>
    {
    public:
                                StackString();
                                StackString(const T* const rawString);
                                StackString(const StackString& rhs);
                                StackString(StackString&& rhs) noexcept;
                                ~StackString();

    public:
        StackString&            operator=(const StackString& rhs) noexcept;
        StackString&            operator=(StackString&& rhs) noexcept;
        StackString&            operator=(const T* const rawString) noexcept;

    public:
        bool                    operator==(const T* const rawString) const noexcept;
        bool                    operator==(const StackString& rhs) const noexcept;
        bool                    operator!=(const T* const rawString) const noexcept;
        bool                    operator!=(const StackString& rhs) const noexcept;

    public:
        StackString&            operator+=(const T* const rawString) noexcept;
        StackString&            operator+=(const StackString& rhs) noexcept;

    public:
        T&                      operator[](const uint32 at) noexcept;
        const T&                operator[](const uint32 at) const noexcept;

    public:
        virtual uint32          capacity() const override;
        uint32                  countBytes() const noexcept;
        uint32                  countChars() const noexcept;
        virtual const T*        c_str() const override;

    public:
        T*                      data() noexcept;

    private:
        bool                    canInsert(const uint32 byteCount) const noexcept;

    public:
        virtual void            clear();
        virtual MutableString<T>&   append(const StringBase<T>& rhs) override;
        StackString&            append(const T* const rhs) noexcept;
        StackString&            append(const StackString& rhs) noexcept;
        virtual MutableString<T>&   assign(const StringBase<T>& rhs) override;
        StackString&            assign(const T* const rhs) noexcept;
        StackString&            assign(const StackString& rhs) noexcept;
        void                    resize(const uint32 newByteCount) noexcept;

    public:
        StackString             substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
        uint32                  rfind(const T* const token, const uint32 offset = 0) const noexcept;
        bool                    compare(const T* const rhs) const noexcept;
        bool                    compare(const StackString& rhs) const noexcept;
        uint64                  computeHash() const noexcept;

    private:
        uint32                  _byteCount;
        Array<T, BufferSize>    _raw;
    };


    template <uint32 BufferSize>
    using StackStringA = StackString<char, BufferSize>;
    
    template <uint32 BufferSize>
    using StackStringW = StackString<wchar_t, BufferSize>;

    template <uint32 BufferSize>
    using StackStringU8 = StackString<char8_t, BufferSize>;
}


#endif // !MINT_SCOPE_STRING_H
