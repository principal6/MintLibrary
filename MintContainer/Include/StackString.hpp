#pragma once


#include <MintContainer/Include/StackString.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString()
        : MutableString<T>(StringType::StackString)
        , _length {}
        , _raw{}
    {
        __noop;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(const T* const rawString)
        : StackString()
    {
        _length = min(static_cast<uint32>(_getRawStringLength(rawString)), BufferSize - 1);
        _copyString(&_raw[0], rawString, _length);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(const StackString& rhs)
        : StackString()
    {
        _length = rhs._length;
        _copyString(&_raw[0], &rhs._raw[0], _length);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(StackString&& rhs) noexcept
        : StackString()
    {
        std::swap(_length, rhs._length);
        std::swap(_raw, rhs._raw);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::~StackString()
    {
        __noop;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::operator=(const StackString& rhs) noexcept
    {
        _length = rhs._length;
        _copyString(&_raw[0], &rhs._raw[0], _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::operator=(StackString&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(_length, rhs._length);
            std::swap(_raw, rhs._raw);
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::operator=(const T* const rawString) noexcept
    {
        return assign(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::operator==(const T* const rawString) const noexcept
    {
        return compare(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::operator==(const StackString& rhs) const noexcept
    {
        return compare(rhs);
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::operator!=(const T* const rawString) const noexcept
    {
        return !compare(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::operator!=(const StackString& rhs) const noexcept
    {
        return !compare(rhs);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::operator+=(const T* const rawString) noexcept
    {
        return append(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::operator+=(const StackString& rhs) noexcept
    {
        return append(rhs);
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE T& StackString<T, BufferSize>::operator[](const uint32 at) noexcept
    {
        return _raw[at];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const T& StackString<T, BufferSize>::operator[](const uint32 at) const noexcept
    {
        return _raw[at];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE uint32 StackString<T, BufferSize>::capacity() const
    {
        return BufferSize;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE uint32 StackString<T, BufferSize>::length() const noexcept
    {
        return _length;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const T* StackString<T, BufferSize>::c_str() const
    {
        return &_raw[0];
    }

    template<typename T, uint32 BufferSize>
    MINT_INLINE uint32 StackString<T, BufferSize>::_getRawStringLength(const T* const rawString) noexcept
    {
        return StringUtil::length(rawString);
    }

    template<typename T, uint32 BufferSize>
    MINT_INLINE void StackString<T, BufferSize>::_copyString(T* const destination, const T* const source, const uint64 length) noexcept
    {
        ::memcpy(destination, source, sizeof(T) * length);
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE T* StackString<T, BufferSize>::data() noexcept
    {
        return &_raw[0];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE bool StackString<T, BufferSize>::canInsert(const uint32 insertLength) const noexcept
    {
        if (BufferSize <= _length + insertLength)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과해서 insert 할 수 없습니다.");
            return false;
        }
        return true;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE void StackString<T, BufferSize>::clear()
    {
        _length = 0;
        _raw[0] = 0; // NULL
    }

    template <typename T, uint32 BufferSize>
    MutableString<T>& StackString<T, BufferSize>::append(const StringBase<T>& rhs)
    {
        const uint32 rhsLength = static_cast<uint32>(_getRawStringLength(rhs.c_str()));
        if (canInsert(rhsLength))
        {
            _copyString(&_raw[_length], rhs.c_str(), rhsLength);
            _length += rhsLength;
            _raw[_length] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::append(const T* const rawString) noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(_getRawStringLength(rawString));
        if (canInsert(rawStringLength))
        {
            _copyString(&_raw[_length], rawString, rawStringLength);
            _length += rawStringLength;
            _raw[_length] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::append(const StackString& rhs) noexcept
    {
        if (canInsert(rhs._length))
        {
            _copyString(&_raw[_length], &rhs._raw[0], rhs._length);
            _length += rhs._length;
            _raw[_length] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline MutableString<T>& StackString<T, BufferSize>::assign(const StringBase<T>& rhs)
    {
        uint32 rhsLength = StringUtil::length(rhs.c_str());
        if (BufferSize <= rhsLength)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과하여 문자열이 잘립니다.");
            rhsLength = BufferSize - 1;
        }
        _length = rhsLength;
        _copyString(&_raw[0], rhs.c_str(), _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::assign(const T* const rawString) noexcept
    {
        uint32 rawStringLength = static_cast<uint32>(_getRawStringLength(rawString));
        if (BufferSize <= rawStringLength)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과하여 문자열이 잘립니다.");
            rawStringLength = BufferSize - 1;
        }
        _length = rawStringLength;
        _copyString(&_raw[0], rawString, _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>& StackString<T, BufferSize>::assign(const StackString& rhs) noexcept
    {
        if (this != &rhs)
        {
            return assign(rhs.c_str());
        }
        return *this;
    }

    template<typename T, uint32 BufferSize>
    inline void StackString<T, BufferSize>::resize(const uint32 newSize) noexcept
    {
        const uint32 oldSize = length();
        if (oldSize < newSize)
        {
            for (uint32 iter = oldSize; iter < newSize; ++iter)
            {
                _raw[iter] = 0;
            }
        }
        else
        {
            _raw[newSize] = 0;
        }

        _length = newSize;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize> StackString<T, BufferSize>::substr(const uint32 offset, const uint32 count) const noexcept
    {
        StackString<T, BufferSize> result; // { &_raw[offset] };
        result._length = min(count, _length - offset - 1);
        _copyString(&result._raw[0], &_raw[offset], result._length);
        return result;
    }

    template <typename T, uint32 BufferSize>
    inline uint32 StackString<T, BufferSize>::find(const T* const rawString, const uint32 offset) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(_getRawStringLength(rawString));
        if (_length < rawStringLength)
        {
            return kStringNPos;
        }
        
        for (uint32 myAt = offset; myAt < _length; ++myAt)
        {
            uint32 rawStringAt = 0;
            while (rawStringAt < rawStringLength)
            {
                if (_raw[myAt + rawStringAt] != rawString[rawStringAt])
                {
                    break;
                }
                ++rawStringAt;
            }

            if (rawStringAt == rawStringLength)
            {
                return myAt;
            }
        }
        return kStringNPos;
    }

    template <typename T, uint32 BufferSize>
    inline uint32 StackString<T, BufferSize>::rfind(const T* const rawString, const uint32 offset) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(_getRawStringLength(rawString));
        if (_length < rawStringLength)
        {
            return kStringNPos;
        }

        const uint32 myAtStart = _length - rawStringLength;
        if (myAtStart < offset)
        {
            return kStringNPos;
        }
        for (uint32 myAt = myAtStart - offset; myAt != kUint32Max; --myAt)
        {
            uint32 rawStringAt = 0;
            while (rawStringAt < rawStringLength)
            {
                if (_raw[myAt + rawStringAt] != rawString[rawStringAt])
                {
                    break;
                }
                ++rawStringAt;
            }

            if (rawStringAt == rawStringLength)
            {
                return myAt;
            }
        }
        return kStringNPos;
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::compare(const T* const rawString) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(_getRawStringLength(rawString));
        if (_length != rawStringLength)
        {
            return false;
        }

        for (uint32 at = 0; at < _length; ++at)
        {
            if (_raw[at] != rawString[at])
            {
                return false;
            }
        }
        return true;
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::compare(const StackString& rhs) const noexcept
    {
        if (_length != rhs._length)
        {
            return false;
        }

        for (uint32 at = 0; at < _length; ++at)
        {
            if (_raw[at] != rhs._raw[at])
            {
                return false;
            }
        }
        return true;
    }

    template<typename T, uint32 BufferSize>
    inline uint64 StackString<T, BufferSize>::computeHash() const noexcept
    {
        return mint::computeHash(c_str());
    }
}
