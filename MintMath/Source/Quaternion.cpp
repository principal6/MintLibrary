#include <MintMath/Include/Quaternion.h>


namespace mint
{
    Quaternion Quaternion::conjugate(const Quaternion& q) noexcept
    {
        return Quaternion(q._w, -q._x, -q._y, -q._z);
    }

    const float Quaternion::norm(const Quaternion& q) noexcept
    {
        return q._data.length();
    }

    Quaternion Quaternion::reciprocal(const Quaternion& q) noexcept
    {
        const Quaternion conjugate  = Quaternion::conjugate(q);
        const float norm = Quaternion::norm(q);
        return Quaternion(conjugate / (norm * norm));
    }

    Quaternion Quaternion::makeRotationQuaternion(const mint::Float3& axis, float angle) noexcept
    {
        const mint::Float3 r = mint::Float3::normalize(axis);
        const float half_angle = angle * 0.5f;
        const float cos_half = cosf(half_angle);
        const float sin_half = sinf(half_angle);
        return Quaternion(cos_half, sin_half * r._x, sin_half * r._y, sin_half * r._z);
    }


    Quaternion::Quaternion()
        : Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
    {
        __noop;
    }

    Quaternion::Quaternion(const float a, const float b, const float c, const float d)
        : _data{ b, c, d, a }
    {
        __noop;
    }

    Quaternion& Quaternion::operator*=(const Quaternion& q) noexcept
    {
        const float a = _a;
        const float b = _b;
        const float c = _c;
        const float d = _d;
        _a = +a * q._a - b * q._b - c * q._c - d * q._d;
        _b = +a * q._b + b * q._a + c * q._d - d * q._c;
        _c = +a * q._c - b * q._d + c * q._a + d * q._b;
        _d = +a * q._d + b * q._c - c * q._b + d * q._a;
        return *this;
    }

    Quaternion& Quaternion::operator*=(const float s) noexcept
    {
        _data *= s;
        return *this;
    }

    Quaternion& Quaternion::operator/=(const float s) noexcept
    {
        _data /= s;
        return *this;
    }

    Quaternion& Quaternion::operator*=(const mint::Float4& v) noexcept
    {
        const float a = _a;
        const float b = _b;
        const float c = _c;
        const float d = _d;
        _a = -b * v._x - c * v._y - d * v._z;
        _b = +a * v._x + c * v._z - d * v._y;
        _c = +a * v._y - b * v._z + d * v._x;
        _d = +a * v._z + b * v._y - c * v._x;
        return *this;
    }

    Quaternion Quaternion::operator*(const Quaternion& q) const noexcept
    {
        return Quaternion
        (
            +_a * q._a - _b * q._b - _c * q._c - _d * q._d,
            +_a * q._b + _b * q._a + _c * q._d - _d * q._c,
            +_a * q._c - _b * q._d + _c * q._a + _d * q._b,
            +_a * q._d + _b * q._c - _c * q._b + _d * q._a
        );
    }

    Quaternion Quaternion::operator*(const float s) const noexcept
    {
        return Quaternion(_a * s, _b * s, _c * s, _d * s);
    }

    Quaternion Quaternion::operator/(const float s) const noexcept
    {
        return Quaternion(_a / s, _b / s, _c / s, _d / s);
    }

    Quaternion Quaternion::conjugate() const noexcept
    {
        return Quaternion::conjugate(*this);
    }

    Quaternion Quaternion::reciprocal() const noexcept
    {
        return Quaternion::reciprocal(*this);
    }

    mint::Float4 Quaternion::rotateVector(const mint::Float4& inputVector)
    {
        Quaternion result = *this;
        result *= inputVector;
        result *= conjugate();
        return mint::Float4(result._x, result._y, result._z, inputVector._w);
    }

    void Quaternion::setAxisAngle(const mint::Float3& axis, float angle) noexcept
    {
        const mint::Float3 normalizedAxis = mint::Float3::normalize(axis);
        const float halfAngle = angle * 0.5f;
        const float cosHalfAngle = cosf(halfAngle);
        const float sinHalfAngle = 1.0f - cosHalfAngle * cosHalfAngle;
        _x = sinHalfAngle * normalizedAxis._x;
        _y = sinHalfAngle * normalizedAxis._y;
        _z = sinHalfAngle * normalizedAxis._z;
        _w = cosHalfAngle;
    }

    void Quaternion::getAxisAngle(mint::Float3& axis, float& angle) const noexcept
    {
        angle = acos(_w) * 2.0f;

        const float sinHalfAngle = 1.0f - _w * _w;
        if (sinHalfAngle == 0.0f)
        {
            axis._x = 1.0f;
            axis._y = 0.0f;
            axis._z = 0.0f;
        }
        else
        {
            axis._x = _x / sinHalfAngle;
            axis._y = _y / sinHalfAngle;
            axis._z = _z / sinHalfAngle;
            axis.normalize();
        }
    }
}
