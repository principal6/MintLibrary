#pragma once


#ifndef _MINT_MATH_EASING_H_


#include <MintMath/Include/MathCommon.h>


namespace mint
{
    namespace Math
    {
        class Easing
        {
        public:
            enum class Method
            {
                InQuad,
                OutQuad,
                InOutQuad,
                COUNT
            };

        public:
            static float    ease(const Method method, float x) noexcept;

        private:
                            Easing() = default;
                            ~Easing() = default;
        };
    }
}


#include <MintMath/Include/Easing.inl>


#endif // !_MINT_MATH_EASING_H_
