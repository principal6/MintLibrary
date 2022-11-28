#pragma once


#include <MintContainer/Include/StackString.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString()
        : MutableString<T>()
        , _byteCount{}
        , _raw{}
    {
        __noop;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(const T* const rawString)
        : StackString()
    {
        _byteCount = min(static_cast<uint32>(StringUtil::countBytes(rawString)), BufferSize - 1);
        StringUtil::copy(&_raw[0], rawString, _byteCount);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(const StackString& rhs)
        : StackString()
    {
        _byteCount = rhs._byteCount;
        StringUtil::copy(&_raw[0], &rhs._raw[0], _byteCount);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::StackString(StackString&& rhs) noexcept
        : StackString()
    {
        std::swap(_byteCount, rhs._byteCount);
        std::swap(_raw, rhs._raw);
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize>::~StackString()
    {
        __noop;
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
    MINT_INLINE uint32 StackString<T, BufferSize>::capacity() const
    {
        return BufferSize;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE uint32 StackString<T, BufferSize>::countBytes() const
    {
        return _byteCount;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const T* StackString<T, BufferSize>::c_str() const
    {
        return &_raw[0];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE T* StackString<T, BufferSize>::data()
    {
        return &_raw[0];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE bool StackString<T, BufferSize>::canInsert(const uint32 byteCount) const noexcept
    {
        if (BufferSize <= _byteCount + byteCount)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과해서 insert 할 수 없습니다.");
            return false;
        }
        return true;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE void StackString<T, BufferSize>::clear()
    {
        _byteCount = 0;
        _raw[0] = 0; // NULL
    }

    template <typename T, uint32 BufferSize>
    MutableString<T>& StackString<T, BufferSize>::append(const StringReference<T>& rhs)
    {
        const uint32 rhsByteCount = static_cast<uint32>(StringUtil::countBytes(rhs.c_str()));
        if (canInsert(rhsByteCount))
        {
            StringUtil::copy(&_raw[_byteCount], rhs.c_str(), rhsByteCount);
            _byteCount += rhsByteCount;
            _raw[_byteCount] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline MutableString<T>& StackString<T, BufferSize>::assign(const StringReference<T>& rhs)
    {
        uint32 rhsByteCount = StringUtil::countChars(rhs.c_str());
        if (BufferSize <= rhsByteCount)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과하여 문자열이 잘립니다.");
            rhsByteCount = BufferSize - 1;
        }
        _byteCount = rhsByteCount;
        StringUtil::copy(&_raw[0], rhs.c_str(), _byteCount);
        _raw[_byteCount] = 0; // NULL
        return *this;
    }

    template<typename T, uint32 BufferSize>
    inline void StackString<T, BufferSize>::resize(const uint32 newByteCount) noexcept
    {
        const uint32 oldByteCount = countBytes();
        if (oldByteCount < newByteCount)
        {
            for (uint32 iter = oldByteCount; iter < newByteCount; ++iter)
            {
                _raw[iter] = 0;
            }
        }
        else
        {
            _raw[newByteCount] = 0;
        }

        _byteCount = newByteCount;
    }

    template <typename T, uint32 BufferSize>
    inline StackString<T, BufferSize> StackString<T, BufferSize>::substr(const uint32 offset, const uint32 count) const noexcept
    {
        StackString<T, BufferSize> result; // { &_raw[offset] };
        result._byteCount = min(count, _byteCount - offset - 1);
        StringUtil::copy(&result._raw[0], &_raw[offset], result._byteCount);
        return result;
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::compare(const T* const rhs) const noexcept
    {
        const uint32 rhsByteCount = static_cast<uint32>(StringUtil::countBytes(rhs));
        if (_byteCount != rhsByteCount)
        {
            return false;
        }

        for (uint32 at = 0; at < _byteCount; ++at)
        {
            if (_raw[at] != rhs[at])
            {
                return false;
            }
        }
        return true;
    }

    template <typename T, uint32 BufferSize>
    inline bool StackString<T, BufferSize>::compare(const StackString& rhs) const noexcept
    {
        if (_byteCount != rhs._byteCount)
        {
            return false;
        }

        for (uint32 at = 0; at < _byteCount; ++at)
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
