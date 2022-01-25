#pragma once


#ifndef MINT_TRANSFORM_INL
#define MINT_TRANSFORM_INL


namespace mint
{
    MINT_INLINE Transform::Transform()
        : Transform(Float3::kZero)
    {
        __noop;
    }

    MINT_INLINE Transform::Transform(const Float3& translation)
        : _scale{ Float3::kUnitScale }
        , _translation{ translation }
    {
        __noop;
    }
}


#endif // !MINT_TRANSFORM_INL
