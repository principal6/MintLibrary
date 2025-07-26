#pragma once


namespace mint
{
	template<typename T>
	MINT_INLINE Quaternion<T> Quaternion<T>::Conjugate(const Quaternion& q) noexcept
	{
		return Quaternion(q._w, -q._x, -q._y, -q._z);
	}

	template<typename T>
	MINT_INLINE T Quaternion<T>::Norm(const Quaternion& q) noexcept
	{
		return Vec4<T>({ q._x, q._y, q._z, q._w }).Norm();
		//return Float4(q._x, q._y, q._z, q._w).length();
	}

	template<typename T>
	MINT_INLINE Quaternion<T> Quaternion<T>::Reciprocal(const Quaternion& q) noexcept
	{
		const Quaternion conjugate = Quaternion::conjugate(q);
		const T Norm = Quaternion::Norm(q);
		return Quaternion(conjugate / (Norm * Norm));
	}

	template<typename T>
	MINT_INLINE Quaternion<T> Quaternion<T>::MakeRotationQuaternion(const Float3& axis, const T angle) noexcept
	{
		const Float3 r = Float3::Normalize(axis);
		const T half_angle = static_cast<T>(angle * 0.5);
		const T cos_half = ::cos(half_angle);
		const T sin_half = ::sin(half_angle);
		return Quaternion(cos_half, sin_half * r._x, sin_half * r._y, sin_half * r._z);
	}

	template<typename T>
	MINT_INLINE Quaternion<T> Quaternion<T>::MakeRotationQuaternion(const Vec3<T>& axis, const T angle) noexcept
	{
		const Vec3<T>& r = axis.Normalize();
		const T half_angle = static_cast<T>(angle * 0.5);
		const T cos_half = ::cos(half_angle);
		const T sin_half = ::sin(half_angle);
		return Quaternion(cos_half, sin_half * r.GetComponent(0), sin_half * r.GetComponent(1), sin_half * r.GetComponent(2));
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
	MINT_INLINE Quaternion<T>& Quaternion<T>::operator*=(const Vec4<T>& v) noexcept
	{
		const T vx = v.GetComponent(0);
		const T vy = v.GetComponent(1);
		const T vz = v.GetComponent(2);
		const T vw = v.GetComponent(3);
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
	MINT_INLINE Quaternion<T> Quaternion<T>::Conjugate() const noexcept
	{
		return Quaternion::conjugate(*this);
	}

	template<typename T>
	MINT_INLINE Quaternion<T> Quaternion<T>::Reciprocal() const noexcept
	{
		return Quaternion::reciprocal(*this);
	}

	template<typename T>
	MINT_INLINE Float4 Quaternion<T>::RotateVector(const Float4& inputVector) const noexcept
	{
		Quaternion result = *this;
		result *= inputVector;
		result *= Conjugate();
		return Float4(result._x, result._y, result._z, inputVector._w);
	}

	template<typename T>
	inline Vec4<T> Quaternion<T>::RotateVector(const Vec4<T>& inputVector) const noexcept
	{
		Quaternion result = *this;
		result *= inputVector;
		result *= Conjugate();
		return Vec4<T>(result._x, result._y, result._z, inputVector.GetComponent(3));
	}

	template<typename T>
	MINT_INLINE void Quaternion<T>::SetAxisAngle(const Float3& axis, const T angle) noexcept
	{
		const Float3 normalizedAxis = Float3::Normalize(axis);
		const T halfAngle = static_cast<T>(angle * 0.5);
		const T cosHalfAngle = ::cos(halfAngle);
		const T sinHalfAngle = static_cast<T>(1.0 - cosHalfAngle * cosHalfAngle);
		_x = sinHalfAngle * normalizedAxis._x;
		_y = sinHalfAngle * normalizedAxis._y;
		_z = sinHalfAngle * normalizedAxis._z;
		_w = cosHalfAngle;
	}

	template<typename T>
	MINT_INLINE void Quaternion<T>::SetAxisAngle(const Vec3<T>& axis, const T angle) noexcept
	{
		const Vec3<T>& normalizedAxis = axis.Normalize();
		const T halfAngle = static_cast<T>(angle * 0.5);
		const T cosHalfAngle = ::cos(halfAngle);
		const T sinHalfAngle = static_cast<T>(1.0 - cosHalfAngle * cosHalfAngle);
		_x = sinHalfAngle * normalizedAxis.GetComponent(0);
		_y = sinHalfAngle * normalizedAxis.GetComponent(1);
		_z = sinHalfAngle * normalizedAxis.GetComponent(2);
		_w = cosHalfAngle;
	}

	template<typename T>
	MINT_INLINE void Quaternion<T>::GetAxisAngle(T(&axis)[3], T& angle) const noexcept
	{
		angle = static_cast<T>(::acos(_w) * 2.0);

		const T sinHalfAngle = static_cast<T>(1.0 - _w * _w);
		if (sinHalfAngle == static_cast<T>(0.0))
		{
			axis[0] = static_cast<T>(1);
			axis[1] = static_cast<T>(0);
			axis[2] = static_cast<T>(0);
		}
		else
		{
			axis[0] = _x / sinHalfAngle;
			axis[1] = _y / sinHalfAngle;
			axis[2] = _z / sinHalfAngle;

			const T NormSq = axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2];
			const T Norm = ::sqrt(NormSq);
			axis[0] /= Norm;
			axis[1] /= Norm;
			axis[2] /= Norm;
		}
	}

	template<typename T>
	MINT_INLINE void Quaternion<T>::GetAxisAngle(Float3& axis, T& angle) const noexcept
	{
		GetAxisAngle(axis._c, angle);
	}

	template<typename T>
	MINT_INLINE void Quaternion<T>::GetAxisAngle(Vec3<T>& axis, T& angle) const noexcept
	{
		GetAxisAngle(axis._c, angle);
	}
}
