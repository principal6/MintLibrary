#pragma once


namespace fs
{
    inline StringRange::StringRange()
        : _offset{ 0 }
        , _length{ kUint32Max }
    {
        __noop;
    }

    inline StringRange::StringRange(const uint32 offset)
        : _offset{ offset }
        , _length{ kUint32Max }
    {
        __noop;
    }

    inline StringRange::StringRange(const uint32 offset, const uint32 length)
        : _offset{ offset }
        , _length{ length }
    {
        __noop;
    }

    FS_INLINE const bool StringRange::isLengthSet() const noexcept
    {
        return _length;
    }
}
