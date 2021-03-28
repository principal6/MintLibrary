#pragma once


namespace fs
{
    inline constexpr Float3::Float3()
        : Float3(0.0f)
    {
        __noop;
    }

    inline constexpr Float3::Float3(const float s)
        : Float3(s, s, s)
    { 
        __noop; 
    }

    inline constexpr Float3::Float3(const float x, const float y, const float z)
        : _x{ x }
        , _y{ y }
        , _z{ z }
    {
        __noop;
    }

    inline constexpr Float3::Float3(const Float2& rhs)
        : Float3(rhs._x, rhs._y, 0.0f)
    {
        __noop;
    }
}
