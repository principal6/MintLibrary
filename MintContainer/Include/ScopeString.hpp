#pragma once


#include <MintContainer/Include/ScopeString.h>

#include <MintContainer/Include/StringUtil.h>
#include <MintContainer/Include/Hash.hpp>


namespace mint
{
    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>::ScopeString()
        : _length{}
        , _raw{}
    {
        __noop;
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>::ScopeString(const T* const rawString)
        : _length{}
        , _raw{}
    {
        _length = min(static_cast<uint32>(_getRawStringLength(rawString)), BufferSize - 1);
        _copyString(&_raw[0], rawString, _length);
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>::ScopeString(const ScopeString& rhs)
    {
        _length = rhs._length;
        _copyString(&_raw[0], &rhs._raw[0], _length);
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>::ScopeString(ScopeString&& rhs) noexcept
    {
        std::swap(_length, rhs._length);
        std::swap(_raw, rhs._raw);
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>::~ScopeString()
    {
        __noop;
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::operator=(const ScopeString& rhs) noexcept
    {
        _length = rhs._length;
        _copyString(&_raw[0], &rhs._raw[0], _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::operator=(ScopeString&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(_length, rhs._length);
            std::swap(_raw, rhs._raw);
        }
        return *this;
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::operator=(const T* const rawString) noexcept
    {
        return assign(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline const bool ScopeString<T, BufferSize>::operator==(const T* const rawString) const noexcept
    {
        return compare(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline const bool ScopeString<T, BufferSize>::operator==(const ScopeString& rhs) const noexcept
    {
        return compare(rhs);
    }

    template <typename T, uint32 BufferSize>
    inline const bool ScopeString<T, BufferSize>::operator!=(const T* const rawString) const noexcept
    {
        return !compare(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline const bool ScopeString<T, BufferSize>::operator!=(const ScopeString& rhs) const noexcept
    {
        return !compare(rhs);
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::operator+=(const T* const rawString) noexcept
    {
        return append(rawString);
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::operator+=(const ScopeString& rhs) noexcept
    {
        return append(rhs);
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE T& ScopeString<T, BufferSize>::operator[](const uint32 at) noexcept
    {
        return _raw[at];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const T& ScopeString<T, BufferSize>::operator[](const uint32 at) const noexcept
    {
        return _raw[at];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE uint32 ScopeString<T, BufferSize>::capacity() const noexcept
    {
        return BufferSize;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE uint32 ScopeString<T, BufferSize>::length() const noexcept
    {
        return _length;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const T* ScopeString<T, BufferSize>::c_str() const noexcept
    {
        return &_raw[0];
    }

    template<typename T, uint32 BufferSize>
    MINT_INLINE const uint32 ScopeString<T, BufferSize>::_getRawStringLength(const T* const rawString) noexcept
    {
        return StringUtil::length(rawString);
    }

    template<typename T, uint32 BufferSize>
    MINT_INLINE void ScopeString<T, BufferSize>::_copyString(T* const destination, const T* const source, const uint64 length) noexcept
    {
        ::memcpy(destination, source, sizeof(T) * length);
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE T* ScopeString<T, BufferSize>::data() noexcept
    {
        return &_raw[0];
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE const bool ScopeString<T, BufferSize>::canInsert(const uint32 insertLength) const noexcept
    {
        if (BufferSize <= _length + insertLength)
        {
            MINT_ASSERT(false, "버퍼 크기를 초과해서 insert 할 수 없습니다.");
            return false;
        }
        return true;
    }

    template <typename T, uint32 BufferSize>
    MINT_INLINE void ScopeString<T, BufferSize>::clear() noexcept
    {
        _length = 0;
        _raw[0] = 0; // NULL
    }

    template <typename T, uint32 BufferSize>
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::append(const T* const rawString) noexcept
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
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::append(const ScopeString& rhs) noexcept
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
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::assign(const T* const rawString) noexcept
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
    inline ScopeString<T, BufferSize>& ScopeString<T, BufferSize>::assign(const ScopeString& rhs) noexcept
    {
        if (this != &rhs)
        {
            return assign(rhs.c_str());
        }
        return *this;
    }

    template<typename T, uint32 BufferSize>
    inline void ScopeString<T, BufferSize>::resize(const uint32 newSize) noexcept
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
    inline ScopeString<T, BufferSize> ScopeString<T, BufferSize>::substr(const uint32 offset, const uint32 count) const noexcept
    {
        ScopeString<T, BufferSize> result; // { &_raw[offset] };
        result._length = min(count, _length - offset - 1);
        _copyString(&result._raw[0], &_raw[offset], result._length);
        return result;
    }

    template <typename T, uint32 BufferSize>
    inline const uint32 ScopeString<T, BufferSize>::find(const T* const rawString, const uint32 offset) const noexcept
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
    inline const uint32 ScopeString<T, BufferSize>::rfind(const T* const rawString, const uint32 offset) const noexcept
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
    inline const bool ScopeString<T, BufferSize>::compare(const T* const rawString) const noexcept
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
    inline const bool ScopeString<T, BufferSize>::compare(const ScopeString& rhs) const noexcept
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
    inline const uint64 ScopeString<T, BufferSize>::computeHash() const noexcept
    {
        return mint::computeHash(c_str());
    }
}
