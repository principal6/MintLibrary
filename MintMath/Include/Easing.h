#pragma once


#ifndef MINT_MATH_EASING_H


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
            static const float  ease(const Method method, float x) noexcept;

        private:
                                Easing() = default;
                                ~Easing() = default;
        };
    }
}


#include <MintMath/Include/Easing.inl>


#endif // !MINT_MATH_EASING_H
