#include "Quaternion.h"
#pragma once


namespace mint
{
    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::conjugate(const Quaternion&q) noexcept
    {
        return Quaternion(q._w, -q._x, -q._y, -q._z);
    }

    template<typename T>
    MINT_INLINE const T Quaternion<T>::norm(const Quaternion& q) noexcept
    {
        return Math::Vector4<T>({ q._x, q._y, q._z, q._w }).norm();
        //return Float4(q._x, q._y, q._z, q._w).length();
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::reciprocal(const Quaternion& q) noexcept
    {
        const Quaternion conjugate = Quaternion::conjugate(q);
        const T norm = Quaternion::norm(q);
        return Quaternion(conjugate / (norm * norm));
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::makeRotationQuaternion(const Float3& axis, const T angle) noexcept
    {
        const Float3 r = Float3::normalize(axis);
        const T half_angle = static_cast<T>(angle * 0.5);
        const T cos_half = ::cos(half_angle);
        const T sin_half = ::sin(half_angle);
        return Quaternion(cos_half, sin_half * r._x, sin_half * r._y, sin_half * r._z);
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::makeRotationQuaternion(const Math::Vector3<T>& axis, const T angle) noexcept
    {
        const Math::Vector3<T>& r = axis.normalize();
        const T half_angle = static_cast<T>(angle * 0.5);
        const T cos_half = ::cos(half_angle);
        const T sin_half = ::sin(half_angle);
        return Quaternion(cos_half, sin_half * r.getComponent(0), sin_half * r.getComponent(1), sin_half * r.getComponent(2));
    }


    template<typename T>
    inline Quaternion<T>::Quaternion()
        : Quaternion(1, 0, 0, 0)
    {
        __noop;
    }

    template<typename T>
    inline Quaternion<T>::Quaternion(const T a, const T b, const T c, const T d)
        : _a{ a }
        , _b{ b }
        , _c{ c }
        , _d{ d }
    {
        __noop;
    }

    template<typename T>
    MINT_INLINE Quaternion<T>& Quaternion<T>::operator*=(const Quaternion& q) noexcept
    {
        const T a = _a;
        const T b = _b;
        const T c = _c;
        const T d = _d;
        _a = +a * q._a - b * q._b - c * q._c - d * q._d;
        _b = +a * q._b + b * q._a + c * q._d - d * q._c;
        _c = +a * q._c - b * q._d + c * q._a + d * q._b;
        _d = +a * q._d + b * q._c - c * q._b + d * q._a;
        return *this;
    }

    template<typename T>
    MINT_INLINE Quaternion<T>& Quaternion<T>::operator*=(const T s) noexcept
    {
        _a *= s;
        _b *= s;
        _c *= s;
        _d *= s;
        return *this;
    }

    template<typename T>
    MINT_INLINE Quaternion<T>& Quaternion<T>::operator/=(const T s) noexcept
    {
        _a /= s;
        _b /= s;
        _c /= s;
        _d /= s;
        return *this;
    }

    template<typename T>
    MINT_INLINE Quaternion<T>& Quaternion<T>::operator*=(const Float4& v) noexcept
    {
        const T a = _a;
        const T b = _b;
        const T c = _c;
        const T d = _d;
        _a = -b * v._x - c * v._y - d * v._z;
        _b = +a * v._x + c * v._z - d * v._y;
        _c = +a * v._y - b * v._z + d * v._x;
        _d = +a * v._z + b * v._y - c * v._x;
        return *this;
    }

    template<typename T>
    MINT_INLINE Quaternion<T>& Quaternion<T>::operator*=(const Math::Vector4<T>& v) noexcept
    {
        const T vx = v.getComponent(0);
        const T vy = v.getComponent(1);
        const T vz = v.getComponent(2);
        const T vw = v.getComponent(3);
        const T a = _a;
        const T b = _b;
        const T c = _c;
        const T d = _d;
        _a = -b * vx - c * vy - d * vz;
        _b = +a * vx + c * vz - d * vy;
        _c = +a * vy - b * vz + d * vx;
        _d = +a * vz + b * vy - c * vx;
        return *this;
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::operator*(const Quaternion& q) const noexcept
    {
        return Quaternion
        (
            +_a * q._a - _b * q._b - _c * q._c - _d * q._d,
            +_a * q._b + _b * q._a + _c * q._d - _d * q._c,
            +_a * q._c - _b * q._d + _c * q._a + _d * q._b,
            +_a * q._d + _b * q._c - _c * q._b + _d * q._a
        );
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::operator*(const T s) const noexcept
    {
        return Quaternion(_a * s, _b * s, _c * s, _d * s);
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::operator/(const T s) const noexcept
    {
        return Quaternion(_a / s, _b / s, _c / s, _d / s);
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::conjugate() const noexcept
    {
        return Quaternion::conjugate(*this);
    }

    template<typename T>
    MINT_INLINE Quaternion<T> Quaternion<T>::reciprocal() const noexcept
    {
        return Quaternion::reciprocal(*this);
    }

    template<typename T>
    MINT_INLINE Float4 Quaternion<T>::rotateVector(const Float4& inputVector) const noexcept
    {
        Quaternion result = *this;
        result *= inputVector;
        result *= conjugate();
        return Float4(result._x, result._y, result._z, inputVector._w);
    }

    template<typename T>
    inline Math::Vector4<T> Quaternion<T>::rotateVector(const Math::Vector4<T>& inputVector) const noexcept
    {
        Quaternion result = *this;
        result *= inputVector;
        result *= conjugate();
        return Math::Vector4<T>(result._x, result._y, result._z, inputVector.getComponent(3));
    }

    template<typename T>
    MINT_INLINE void Quaternion<T>::setAxisAngle(const Float3& axis, const T angle) noexcept
    {
        const Float3 normalizedAxis = Float3::normalize(axis);
        const T halfAngle = static_cast<T>(angle * 0.5);
        const T cosHalfAngle = ::cos(halfAngle);
        const T sinHalfAngle = static_cast<T>(1.0 - cosHalfAngle * cosHalfAngle);
        _x = sinHalfAngle * normalizedAxis._x;
        _y = sinHalfAngle * normalizedAxis._y;
        _z = sinHalfAngle * normalizedAxis._z;
        _w = cosHalfAngle;
    }

    template<typename T>
    MINT_INLINE void Quaternion<T>::setAxisAngle(const Math::Vector3<T>& axis, const T angle) noexcept
    {
        const Math::Vector3<T>& normalizedAxis = axis.normalize();
        const T halfAngle = static_cast<T>(angle * 0.5);
        const T cosHalfAngle = ::cos(halfAngle);
        const T sinHalfAngle = static_cast<T>(1.0 - cosHalfAngle * cosHalfAngle);
        _x = sinHalfAngle * normalizedAxis.getComponent(0);
        _y = sinHalfAngle * normalizedAxis.getComponent(1);
        _z = sinHalfAngle * normalizedAxis.getComponent(2);
        _w = cosHalfAngle;
    }

    template<typename T>
    MINT_INLINE void Quaternion<T>::getAxisAngle(Float3& axis, T& angle) const noexcept
    {
        angle = static_cast<T>(::acos(_w) * 2.0);

        const T sinHalfAngle = static_cast<T>(1.0 - _w * _w);
        if (sinHalfAngle == static_cast<T>(0.0))
        {
            axis._x = static_cast<T>(1);
            axis._y = static_cast<T>(0);
            axis._z = static_cast<T>(0);
        }
        else
        {
            axis._x = _x / sinHalfAngle;
            axis._y = _y / sinHalfAngle;
            axis._z = _z / sinHalfAngle;
            axis.normalize();
        }
    }

    template<typename T>
    MINT_INLINE void Quaternion<T>::getAxisAngle(Math::Vector3<T>& axis, T& angle) const noexcept
    {
        angle = static_cast<T>(::acos(_w) * 2.0);

        const T sinHalfAngle = static_cast<T>(1.0 - _w * _w);
        if (sinHalfAngle == static_cast<T>(0.0))
        {
            axis.setComponnet(0, static_cast<T>(1));
            axis.setComponnet(1, static_cast<T>(0));
            axis.setComponnet(2, static_cast<T>(0));
        }
        else
        {
            axis.setComponnet(0, static_cast<T>(_x / sinHalfAngle));
            axis.setComponnet(1, static_cast<T>(_y / sinHalfAngle));
            axis.setComponnet(2, static_cast<T>(_z / sinHalfAngle));
            axis.normalize();
        }
    }
}
