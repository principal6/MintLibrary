#include <stdafx.h>
#include <MintMath/Include/Vector4.h>


namespace mint
{
    const Vector4<float> Vector4<float>::kZero;
    const Vector4<float> Vector4<float>::kNan   = Vector4(Math::nan());
    const Vector4<float> Vector4<float>::k1000  = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
    const Vector4<float> Vector4<float>::k0100  = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
    const Vector4<float> Vector4<float>::k0010  = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
    const Vector4<float> Vector4<float>::k0001  = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
}
