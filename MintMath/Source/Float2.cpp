#include <MintMath/Include/Float2.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Int2.h>


namespace mint
{
	const Float2 Float2::kZero = Float2(0.0f);
	const Float2 Float2::kOne = Float2(+1.0f);
	const Float2 Float2::kNegativeOne = Float2(-1.0f);
	const Float2 Float2::kMax = Float2(Math::kFloatMax);
	const Float2 Float2::kNan = Float2(Math::getNAN());

	Float2::Float2(const Int2& rhs)
		: Float2(static_cast<float>(rhs._x), static_cast<float>(rhs._y))
	{
		__noop;
	}

	Float2& Float2::operator+=(const Float2& rhs)
	{
		Math::SetAddVec(_c, rhs._c);
		return *this;
	}

	Float2& Float2::operator-=(const Float2& rhs)
	{
		Math::SetSubVec(_c, rhs._c);
		return *this;
	}

	Float2& Float2::operator*=(const float scalar)
	{
		Math::SetMulVec(_c, scalar);
		return *this;
	}

	Float2& Float2::operator/=(const float scalar)
	{
		Math::SetDivVec(_c, scalar);
		return *this;
	}

	const Float2& Float2::operator+() const noexcept
	{
		return *this;
	}

	Float2 Float2::operator-() const noexcept
	{
		return Float2(-_x, -_y);
	}

	Float2 Float2::operator+(const Float2& rhs) const noexcept
	{
		Float2 result = *this;
		Math::SetAddVec(result._c, rhs._c);
		return result;
	}

	Float2 Float2::operator-(const Float2& rhs) const noexcept
	{
		Float2 result = *this;
		Math::SetSubVec(result._c, rhs._c);
		return result;
	}

	Float2 Float2::operator*(const Float2& rhs) const noexcept
	{
		Float2 result = *this;
		Math::SetMulVec(result._c, rhs._c);
		return result;
	}

	Float2 Float2::operator*(const float scalar) const noexcept
	{
		Float2 result = *this;
		Math::SetMulVec(result._c, scalar);
		return result;
	}

	Float2 Float2::operator/(const float scalar) const noexcept
	{
		Float2 result = *this;
		Math::SetDivVec(result._c, scalar);
		return result;
	}

	float& Float2::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < 2, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	const float& Float2::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < 2, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	bool Float2::operator==(const Float2& rhs) const noexcept
	{
		return Math::Equals(_c, rhs._c);
	}

	bool Float2::operator!=(const Float2& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	float Float2::Dot(const Float2& lhs, const Float2& rhs) noexcept
	{
		return Math::Dot(lhs._c, rhs._c);
	}

	Float2 Float2::Normalize(const Float2& in) noexcept
	{
		Float2 result = in;
		Math::Normalize(result._c);
		return result;
	}

	Float2 Float2::Abs(const Float2& rhs) noexcept
	{
		return Float2(::abs(rhs._x), ::abs(rhs._y));
	}

	float Float2::Dot(const Float2& rhs) const noexcept
	{
		return Math::Dot(_c, rhs._c);
	}

	void Float2::Normalize() noexcept
	{
		Math::Normalize(_c);
	}

	float Float2::LengthSqaure() const noexcept
	{
		return Math::NormSq(_c);
	}

	float Float2::Length() const noexcept
	{
		return Math::Norm(_c);
	}

	float Float2::GetMaxElement() const noexcept
	{
		return mint::Max(_x, _y);
	}

	float Float2::GetMinElement() const noexcept
	{
		return mint::Min(_x, _y);
	}

	void Float2::Set(const float x, const float y) noexcept
	{
		_x = x;
		_y = y;
	}

	void Float2::SetZero() noexcept
	{
		Math::SetZeroVec(_c);
	}

	void Float2::SetNAN() noexcept
	{
		Math::SetNAN(_c);
	}

	bool Float2::IsNAN() const noexcept
	{
		return Math::IsNAN(_c);
	}

	bool Float2::HasNegativeElement() const noexcept
	{
		return (_x < 0.0f) || (_y < 0.0f);
	}

	Float2 operator*(const float scalar, const Float2& vector) noexcept
	{
		return vector * scalar;
	}
}
