#pragma once


#include <FsMath/Include/Quaternion.h>

#include <FsMath/Include/Float4.h>


namespace fs
{
	Quaternion Quaternion::conjugate(const Quaternion& q) noexcept
	{
		return Quaternion(q.a(), -q.b(), -q.c(), -q.d());
	}

	const float Quaternion::norm(const Quaternion& q) noexcept
	{
		return q._data.length();
	}

	Quaternion Quaternion::reciprocal(const Quaternion& q) noexcept
	{
		const Quaternion	conjugate	= Quaternion::conjugate(q);
		const float			norm		= Quaternion::norm(q);
		return Quaternion(conjugate / (norm * norm));
	}

	Quaternion Quaternion::rotationQuaternion(const Float4& axis, float angle) noexcept
	{
		const Float4	r			= Float4::normalize(axis);
		const float		half_angle	= angle * 0.5f;
		const float		cos_half	= cosf(half_angle);
		const float		sin_half	= sinf(half_angle);
		return Quaternion(sin_half * r._x, sin_half * r._y, sin_half * r._z, cos_half);
	}


	Quaternion::Quaternion()
		: Quaternion(1.0f, 0.0f, 0.0f, 0.0f)
	{
		__noop;
	}

	Quaternion::Quaternion(const float a, const float b, const float c, const float d)
		: _data{ Float4(b, c, d, a) }
	{
		__noop;
	}

	Quaternion::Quaternion(const Float4& v) : _data{ v._x, v._y, v._z, 0 }
	{
		__noop;
	}

	Quaternion Quaternion::operator*(const Quaternion& q) const noexcept
	{
		const float a1 = a();
		const float b1 = b();
		const float c1 = c();
		const float d1 = d();

		const float a2 = q.a();
		const float b2 = q.b();
		const float c2 = q.c();
		const float d2 = q.d();

		return Quaternion
		(
			+a1 * a2 - b1 * b2 - c1 * c2 - d1 * d2,
			+a1 * b2 + b1 * a2 + c1 * d2 - d1 * c2, // i
			+a1 * c2 - b1 * d2 + c1 * a2 + d1 * b2, // j
			+a1 * d2 + b1 * c2 - c1 * b2 + d1 * a2  // k
		);
	}

	Quaternion Quaternion::operator/(const float s) const noexcept
	{
		return Quaternion(a() / s, b() / s, c() / s, d() / s);
	}

	Quaternion Quaternion::reciprocal() const noexcept
	{
		return Quaternion::reciprocal(*this);
	}

	const float Quaternion::a() const noexcept
	{
		return _data._w;
	}

	const float Quaternion::b() const noexcept
	{
		return _data._x;
	}

	const float Quaternion::c() const noexcept
	{
		return _data._y;
	}

	const float Quaternion::d() const noexcept
	{
		return _data._z;
	}
}

