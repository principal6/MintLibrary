#pragma once


#ifndef MINT_CONTAINER_STRING_H
#define MINT_CONTAINER_STRING_H


#include <MintCommon/Include/CommonDefinitions.h>


namespace mint
{
    template <typename T>
    class String
    {
        static constexpr uint32     kTypeSize = sizeof(T);

    public:
                                    String();
                                    String(const T* const rhs);
                                    String(const String& rhs);
                                    String(String&& rhs) noexcept;
                                    ~String();

    public:
        String&                     operator=(const T* const rhs) noexcept;
        String&                     operator=(const String& rhs) noexcept;
        String&                     operator=(String&& rhs) noexcept;

    public:
        String&                     operator+=(const T* const rhs) noexcept;
        String&                     operator+=(const String& rhs) noexcept;

    public:
        const bool                  operator==(const T* const rhs) const noexcept;
        const bool                  operator==(const String& rhs) const noexcept;
        const bool                  operator!=(const T* const rhs) const noexcept;
        const bool                  operator!=(const String& rhs) const noexcept;

    public:
        T&                          operator[](const uint32 at) noexcept;
        const T&                    operator[](const uint32 at) const noexcept;

    public:
        String&                     assign(const T* const rawString) noexcept;

    private:
        String&                     assignInternalXXX(const T* const rawString) noexcept;
        String&                     assignInternalLongXXX(const T* const rawString) noexcept;

    public:
        String&                     append(const T* const rawString) noexcept;

    private:
        String&                     appendInternalSmallXXX(const T* const rawString) noexcept;
        String&                     appendInternalLongXXX(const T* const rawString) noexcept;

    public:
        void                        reserve(const uint32 newCapacity) noexcept;
        void                        resize(const uint32 newSize, const T fillCharacter = 0) noexcept;

    public:
        void                        clear() noexcept;

    public:
        MINT_INLINE const uint32    size() const noexcept { return static_cast<uint32>(true == isSmallString() ? _short._size : _long._size); }
        MINT_INLINE const uint32    length() const noexcept { return size(); }
        MINT_INLINE const uint32    capacity() const noexcept { return static_cast<uint32>(true == isSmallString() ? Short::kSmallStringCapacity : _long._capacity); }
        const T*                    c_str() const noexcept;

    private:
        T*                          data() noexcept;
        static void                 __copyString(T* const destination, const T* const source, const uint64 length) noexcept;
        const uint32                __getStringLength(const T* const rawString) const noexcept;

    public:
        const uint32                find(const T* const target, const uint32 offset = 0) const noexcept;
        String                      substr(const uint32 offset, const uint32 count = kStringNPos) const noexcept;
        const bool                  compare(const T* const rhs) const noexcept;
        const bool                  compare(const String& rhs) const noexcept;
        const uint64                computeHash() const noexcept;

    private:
        void                        release() noexcept;
        void                        toLongString() noexcept;
        MINT_INLINE const bool      isSmallString() const noexcept { return _short._size < Short::kSmallStringCapacity; }
        MINT_INLINE const bool      isEmpty() const noexcept { return (isSmallString() == true) ? (0 == _short._size) : (0 == _long._size); }
        MINT_INLINE const bool      isNotAllocated() const noexcept { return (isSmallString() == true) ? (0 == _short._size) : (nullptr == _long._rawPointer); }

    private:
        //
        // |                  LC                   |                  LS                   |                  RP                   |
        // | cS | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 |
        // |   wS    |   w00   |   w01   |   w02   |   w03   |   w04   |   w05   |   w06   |   w07   |   w08   |   w09   |   w10   |
        //
        struct Long
        {
            //static constexpr uint64 kStringMaxCapacity = 0x0000FFFFFFFFFFFF;

            uint64  _capacity;          // 8
            uint64  _size;              // 8
            T*      _rawPointer;        // 8
        };
        struct Short
        {
            static constexpr uint32 kSmallStringCapacity = (24 / kTypeSize) - 1;

            T       _size;                              //  1 (char) or  2 (wchar_t)
            T       _smallString[kSmallStringCapacity]; // 23 (char) or 22 (wchar_t)
        };
        union
        {
            Long    _long{};
            Short   _short;
        };
    };


    using StringA = String<char>;
    using StringW = String<wchar_t>;
}


#endif // !MINT_CONTAINER_STRING_H
