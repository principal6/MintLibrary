#include <MintMath/Include/AffineVec.h>


namespace mint
{
    const AffineVec<float> AffineVec<float>::kZero;
    const AffineVec<float> AffineVec<float>::kNan   = AffineVec(Math::nan());
    const AffineVec<float> AffineVec<float>::k1000  = AffineVec(1.0f, 0.0f, 0.0f, 0.0f);
    const AffineVec<float> AffineVec<float>::k0100  = AffineVec(0.0f, 1.0f, 0.0f, 0.0f);
    const AffineVec<float> AffineVec<float>::k0010  = AffineVec(0.0f, 0.0f, 1.0f, 0.0f);
    const AffineVec<float> AffineVec<float>::k0001  = AffineVec(0.0f, 0.0f, 0.0f, 1.0f);
}
