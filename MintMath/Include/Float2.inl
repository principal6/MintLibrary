#pragma once


namespace mint
{
    inline constexpr Float2::Float2()
        : Float2(0.0f)
    {
        __noop;
    }

    inline constexpr Float2::Float2(const float scalar)
        : Float2(scalar, scalar)
    {
        __noop;
    }

    inline constexpr Float2::Float2(const float x, const float y)
        : _x{ x }
        , _y{ y }
    {
        __noop;
    }
}
