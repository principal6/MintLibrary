#pragma once


namespace mint
{

#pragma region Free functions
    MINT_INLINE const float dot(const Vector4<float>& lhs, const Vector4<float>& rhs) noexcept
    {
        const __m128 result = _mm_mul_ps(lhs.getRaw(), rhs.getRaw());
        return result.m128_f32[0] + result.m128_f32[1] + result.m128_f32[2] + result.m128_f32[3];
    }

    MINT_INLINE void normalize(Vector4<float>& inOut) noexcept
    {
        const float norm = inOut.norm();
        inOut /= norm;
    }

    MINT_INLINE Vector4<float> cross(const Vector4<float>& lhs, const Vector4<float>& rhs) noexcept
    {
        // Pair
        //    |  x |  y |  z |
        // *  |  y |  z |  x |
        //
        // =  | xy | yz | zx |
        float lhsVec[4];
        lhs.store(lhsVec);
        float rhsVec[4];
        rhs.store(rhsVec);
        const __m128 lhsPair = _mm_mul_ps(lhs.getRaw(), _mm_set_ps(lhsVec[1], lhsVec[2], lhsVec[0], 0.0f));
        const __m128 rhsPair = _mm_mul_ps(rhs.getRaw(), _mm_set_ps(rhsVec[1], rhsVec[2], rhsVec[0], 0.0f));

        // x == l.yz - r.zy
        // y == l.zx - r.xz
        // z == l.xy - r.yx
        return Vector4<float>(_mm_sub_ps(lhsPair, rhsPair));
    }
#pragma endregion


    inline Vector4<float>::Vector4()
        : _raw{ _mm_setzero_ps() }
    {
        __noop;
    }

    inline Vector4<float>::Vector4(const float scalar)
        : _raw{ _mm_set_ps(scalar, scalar, scalar, scalar)}
    {
        __noop;
    }

    inline Vector4<float>::Vector4(const float x, const float y, const float z, const float w)
        : _raw{ _mm_set_ps(x, y, z, w) }
    {
        __noop;
    }

    inline Vector4<float>::Vector4(const __m128 raw)
        : _raw{ raw }
    {
        __noop;
    }

    inline Vector4<float>::Vector4(const Vector4& rhs)
        : _raw{ rhs._raw }
    {
        __noop;
    }

    inline Vector4<float>::Vector4(Vector4&& rhs) noexcept
        : _raw{ rhs._raw }
    {
        rhs._raw = _mm_setzero_ps();
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator=(const Vector4& rhs)
    {
        if (this != &rhs)
        {
            _raw = rhs._raw;
        }
        return *this;
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator=(Vector4&& rhs) noexcept
    {
        if (this != &rhs)
        {
            _raw = rhs._raw;
            rhs._raw = _mm_setzero_ps();
        }
        return *this;
    }

    MINT_INLINE const bool Vector4<float>::operator==(const Vector4& rhs) const noexcept
    {
        const __m128 cmpResult = _mm_cmpeq_ps(_raw, rhs._raw);
        return (cmpResult.m128_i32[0] < 0) && (cmpResult.m128_i32[1] < 0) && (cmpResult.m128_i32[2] < 0) && (cmpResult.m128_i32[3] < 0);
    }

    MINT_INLINE const bool Vector4<float>::operator!=(const Vector4& rhs) const noexcept
    {
        const __m128 cmpResult = _mm_cmpeq_ps(_raw, rhs._raw);
        return (cmpResult.m128_i32[0] == 0) || (cmpResult.m128_i32[1] == 0) || (cmpResult.m128_i32[2] == 0) || (cmpResult.m128_i32[3] == 0);
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator+() noexcept
    {
        return *this;
    }

    MINT_INLINE Vector4<float> Vector4<float>::operator-() const noexcept
    {
        return Vector4(_mm_mul_ps(_raw, _mm_set_ps(-1.0f, -1.0f, -1.0f, -1.0f)));
    }

    MINT_INLINE Vector4<float> Vector4<float>::operator+(const Vector4& rhs) const noexcept
    {
        return Vector4(_mm_add_ps(_raw, rhs._raw));
    }

    MINT_INLINE Vector4<float> Vector4<float>::operator-(const Vector4& rhs) const noexcept
    {
        return Vector4(_mm_sub_ps(_raw, rhs._raw));
    }

    MINT_INLINE Vector4<float> Vector4<float>::operator*(const float scalar) const noexcept
    {
        return Vector4(_mm_mul_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar)));
    }

    MINT_INLINE Vector4<float> Vector4<float>::operator/(const float scalar) const noexcept
    {
        return Vector4(_mm_div_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar)));
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator+=(const Vector4& rhs) noexcept
    {
        _raw = _mm_add_ps(_raw, rhs._raw);
        return *this;
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator-=(const Vector4& rhs) noexcept
    {
        _raw = _mm_sub_ps(_raw, rhs._raw);
        return *this;
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator*=(const float scalar) noexcept
    {
        _raw = _mm_mul_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar));
        return *this;
    }

    MINT_INLINE Vector4<float>& Vector4<float>::operator/=(const float scalar) noexcept
    {
        _raw = _mm_div_ps(_raw, _mm_set_ps(scalar, scalar, scalar, scalar));
        return *this;
    }

    MINT_INLINE void Vector4<float>::load(const float x, const float y, const float z, const float w) noexcept
    {
        _raw = _mm_set_ps(x, y, z, w);
    }

    MINT_INLINE void Vector4<float>::store(float(&vec)[4]) const noexcept
    {
        _mm_store_ps(vec, _raw);
    }

    MINT_INLINE const __m128& Vector4<float>::getRaw() const noexcept
    {
        return _raw;
    }

    MINT_INLINE const float Vector4<float>::dot(const Vector4& rhs) const noexcept
    {
        return mint::dot(*this, rhs);
    }

    MINT_INLINE const float Vector4<float>::normSq() const noexcept
    {
        return dot(*this);
    }

    MINT_INLINE const float Vector4<float>::norm() const noexcept
    {
        return ::sqrt(normSq());
    }

    MINT_INLINE void Vector4<float>::normalize() noexcept
    {
        mint::normalize(*this);
    }

    MINT_INLINE Vector4<float> Vector4<float>::cross(const Vector4<float>& rhs) const noexcept
    {
        return mint::cross(*this, rhs);
    }
}
