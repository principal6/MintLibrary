#pragma once


#ifndef MINT_TRANSFORM_H
#define MINT_TRANSFORM_H


#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


namespace mint
{
    struct Transform
    {
    public:
                        Transform();
                        Transform(const Float3& translation);

    public:
        Float4x4        toMatrix() const noexcept { return Float4x4::srtMatrix(_scale, _rotation, _translation); }

    public:
        Float3          _scale = Float3::kUnitScale;
        QuaternionF     _rotation;
        Float3          _translation;
    };
}


#include <MintMath/Include/Transform.inl>


#endif // !MINT_TRANSFORM_H
