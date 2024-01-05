#include <MintMath/Include/Float2x2.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
	Float2x2 Float2x2::RotationMatrix(const float angle) noexcept
	{
		const float cosTheta = ::cos(angle);
		const float sinTheta = ::sin(angle);
		return Float2x2
		(
			cosTheta, -sinTheta,
			sinTheta, cosTheta
		);
	}

	Float2x2::Float2x2()
		: Float2x2(
			1, 0,
			0, 1)
	{
		__noop;
	}

	Float2x2::Float2x2(const float scalar)
		: Float2x2(
			scalar, scalar,
			scalar, scalar)
	{
		__noop;
	}

	Float2x2::Float2x2(
		const float _11, const float _12,
		const float _21, const float _22)
		: _m{ _11, _12, _21, _22 }
	{
		__noop;
	}

	Float2x2 Float2x2::operator*(const float scalar) const noexcept
	{
		Float2x2 result = *this;
		Math::SetMulMat(result._m, scalar);
		return result;
	}

	Float2x2 Float2x2::operator/(const float scalar) const noexcept
	{
		Float2x2 result = *this;
		Math::SetDivMat(result._m, scalar);
		return result;
	}

	Float2x2 Float2x2::operator*(const Float2x2& rhs) const noexcept
	{
		Float2x2 result;
		Math::Mul(_m, rhs._m, result._m);
		return result;
	}

	Float2 Float2x2::operator*(const Float2& vec) const noexcept
	{
		Float2 result;
		Math::Mul(_m, vec._c, result._c);
		return result;
	}

	void Float2x2::Set(const uint32 row, const uint32 col, const float newValue) noexcept
	{
		_m[Min(row - 1, Float2x2::kMaxIndex)][Min(col - 1, Float2x2::kMaxIndex)] = newValue;
	}

	float Float2x2::Get(const uint32 row, const uint32 col) const noexcept
	{
		return _m[Min(row - 1, Float2x2::kMaxIndex)][Min(col - 1, Float2x2::kMaxIndex)];
	}

	void Float2x2::SetZero() noexcept
	{
		Math::SetZeroMat(_m);
	}

	void Float2x2::SetIdentity() noexcept
	{
		Math::SetIdentity(_m);
	}

	float Float2x2::Determinant() const noexcept
	{
		return Math::Determinant(_m);
	}

	Float2x2 Float2x2::Inverse() const noexcept
	{
		const float a = _m[0][0];
		const float b = _m[0][1];
		const float c = _m[1][0];
		const float d = _m[1][1];
		return Float2x2(
			d, -b,
			-c, a
		) / Determinant();
	}

	bool Float2x2::isInvertible() const noexcept
	{
		return (Determinant() != 0.0f);
	}

	Float2 Float2x2::Mul(const Float2& vec) const noexcept
	{
		Float2 result;
		Math::Mul(_m, vec._c, result._c);
		return result;
	}
}
