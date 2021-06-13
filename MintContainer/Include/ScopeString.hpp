#pragma once


#include <MintContainer/Include/ScopeString.h>


namespace mint
{
    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>::ScopeStringA()
        : _length{}
        , _raw{}
    {
        __noop;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>::ScopeStringA(const char* const rawString)
        : _length{}
        , _raw{}
    {
        _length = min(static_cast<uint32>(strlen(rawString)), BufferSize - 1);
        memcpy(&_raw[0], rawString, _length);
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>::ScopeStringA(const ScopeStringA& rhs)
    {
        _length = rhs._length;
        memcpy(&_raw[0], &rhs._raw[0], _length);
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>::ScopeStringA(ScopeStringA&& rhs) noexcept
    {
        std::swap(_length, rhs._length);
        std::swap(_raw, rhs._raw);
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>::~ScopeStringA()
    {
        __noop;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::operator=(const ScopeStringA& rhs) noexcept
    {
        _length = rhs._length;
        memcpy(&_raw[0], &rhs._raw[0], _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::operator=(ScopeStringA&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(_length, rhs._length);
            std::swap(_raw, rhs._raw);
        }
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::operator=(const char* const rawString) noexcept
    {
        return assign(rawString);
    }

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::operator==(const char* const rawString) const noexcept
    {
        return compare(rawString);
    }

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::operator==(const ScopeStringA& rhs) const noexcept
    {
        return compare(rhs);
    }

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::operator!=(const char* const rawString) const noexcept
    {
        return !compare(rawString);
    }

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::operator!=(const ScopeStringA& rhs) const noexcept
    {
        return !compare(rhs);
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::operator+=(const char* const rawString) noexcept
    {
        return append(this);
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::operator+=(const ScopeStringA& rhs) noexcept
    {
        return append(rhs);
    }

    template<uint32 BufferSize>
    MINT_INLINE char& ScopeStringA<BufferSize>::operator[](const uint32 at) noexcept
    {
        return _raw[at];
    }

    template<uint32 BufferSize>
    MINT_INLINE const char& ScopeStringA<BufferSize>::operator[](const uint32 at) const noexcept
    {
        return _raw[at];
    }

    template<uint32 BufferSize>
    MINT_INLINE uint32 ScopeStringA<BufferSize>::capacity() const noexcept
    {
        return BufferSize;
    }

    template<uint32 BufferSize>
    MINT_INLINE uint32 ScopeStringA<BufferSize>::length() const noexcept
    {
        return _length;
    }

    template<uint32 BufferSize>
    MINT_INLINE const char* ScopeStringA<BufferSize>::c_str() const noexcept
    {
        return &_raw[0];
    }

    template<uint32 BufferSize>
    MINT_INLINE char* ScopeStringA<BufferSize>::data() noexcept
    {
        return &_raw[0];
    }

    template<uint32 BufferSize>
    MINT_INLINE const bool ScopeStringA<BufferSize>::canInsert(const uint32 insertLength) const noexcept
    {
        if (BufferSize <= _length + insertLength)
        {
            MINT_ASSERT("김장원", false, "버퍼 크기를 초과해서 insert 할 수 없습니다.");
            return false;
        }
        return true;
    }

    template<uint32 BufferSize>
    MINT_INLINE void ScopeStringA<BufferSize>::clear() noexcept
    {
        _length = 0;
        _raw[0] = 0; // NULL
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::append(const char* const rawString) noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
        if (true == canInsert(rawStringLength))
        {
            memcpy(&_raw[_length], rawString, rawStringLength);
            _length += rawStringLength;
            _raw[_length] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::append(const ScopeStringA& rhs) noexcept
    {
        if (true == canInsert(rhs._length))
        {
            memcpy(&_raw[_length], &rhs._raw[0], rhs._length);
            _length += rhs._length;
            _raw[_length] = 0; // NULL
            return *this;
        }
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::assign(const char* const rawString) noexcept
    {
        uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
        if (BufferSize <= rawStringLength)
        {
            MINT_ASSERT("김장원", false, "버퍼 크기를 초과하여 문자열이 잘립니다.");
            rawStringLength = BufferSize - 1;
        }
        _length = rawStringLength;
        memcpy(&_raw[0], rawString, _length);
        _raw[_length] = 0; // NULL
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize>& ScopeStringA<BufferSize>::assign(const ScopeStringA& rhs) noexcept
    {
        if (this != &rhs)
        {
            return assign(rhs.c_str());
        }
        return *this;
    }

    template<uint32 BufferSize>
    inline ScopeStringA<BufferSize> ScopeStringA<BufferSize>::substr(const uint32 offset, const uint32 count) const noexcept
    {
        ScopeStringA<BufferSize> result; // { &_raw[offset] };
        result._length = min(count, _length - offset - 1);
        memcpy(&result._raw[0], &_raw[offset], result._length);
        return result;
    }

    template<uint32 BufferSize>
    inline const uint32 ScopeStringA<BufferSize>::find(const char* const rawString, const uint32 offset) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
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

    template<uint32 BufferSize>
    inline const uint32 ScopeStringA<BufferSize>::rfind(const char* const rawString, const uint32 offset) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
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

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::compare(const char* const rawString) const noexcept
    {
        const uint32 rawStringLength = static_cast<uint32>(strlen(rawString));
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

    template<uint32 BufferSize>
    inline const bool ScopeStringA<BufferSize>::compare(const ScopeStringA& rhs) const noexcept
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
}
