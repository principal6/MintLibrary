#include <MintMath/Include/Float3.h>

#include <MintMath/Include/VectorR.hpp>


namespace mint
{
	const Float3 Float3::kZero = Float3(0.0f, 0.0f, 0.0f);
	const Float3 Float3::kUnitScale = Float3(1.0f, 1.0f, 1.0f);
	const Float3 Float3::kAxisX = Float3(1.0f, 0.0f, 0.0f);
	const Float3 Float3::kAxisY = Float3(0.0f, 1.0f, 0.0f);
	const Float3 Float3::kAxisZ = Float3(0.0f, 0.0f, 1.0f);


	Float3& Float3::operator+=(const Float3& rhs)
	{
		Math::SetAddVec(_c, rhs._c);
		return *this;
	}

	Float3& Float3::operator-=(const Float3& rhs)
	{
		Math::SetSubVec(_c, rhs._c);
		return *this;
	}

	Float3& Float3::operator*=(const float scalar)
	{
		Math::SetMulVec(_c, scalar);
		return *this;
	}

	Float3& Float3::operator/=(const float scalar)
	{
		Math::SetDivVec(_c, scalar);
		return *this;
	}

	const Float3& Float3::operator+() const noexcept
	{
		return *this;
	}

	Float3 Float3::operator-() const noexcept
	{
		return Float3(-_x, -_y, -_z);
	}

	Float3 Float3::operator+(const Float3& rhs) const noexcept
	{
		return Float3(_x + rhs._x, _y + rhs._y, _z + rhs._z);
	}

	Float3 Float3::operator-(const Float3& rhs) const noexcept
	{
		return Float3(_x - rhs._x, _y - rhs._y, _z - rhs._z);
	}

	Float3 Float3::operator*(const float scalar) const noexcept
	{
		return Float3(_x * scalar, _y * scalar, _z * scalar);
	}

	Float3 Float3::operator/(const float scalar) const noexcept
	{
		return Float3(_x / scalar, _y / scalar, _z / scalar);
	}

	float& Float3::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < 3, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	const float& Float3::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < 3, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	bool Float3::operator==(const Float3& rhs) const noexcept
	{
		return Math::Equals(_c, rhs._c);
	}

	bool Float3::operator!=(const Float3& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	float Float3::Dot(const Float3& lhs, const Float3& rhs) noexcept
	{
		return Math::Dot(lhs._c, rhs._c);
	}

	Float3 Float3::Cross(const Float3& lhs, const Float3& rhs) noexcept
	{
		Float3 result;
		Math::Cross(lhs._c, rhs._c, result._c);
		return result;
	}

	Float3 Float3::CrossAndNormalize(const Float3& lhs, const Float3& rhs) noexcept
	{
		return Normalize(Cross(lhs, rhs));
	}

	Float3 Float3::Normalize(const Float3& in) noexcept
	{
		Float3 result = in;
		result.Normalize();
		return result;
	}

	float Float3::Dot(const Float3& rhs) const noexcept
	{
		return Float3::Dot(*this, rhs);
	}

	Float3 Float3::Cross(const Float3& rhs) const noexcept
	{
		return Float3::Cross(*this, rhs);
	}

	float Float3::LengthSqaure() const noexcept
	{
		return Math::NormSq(_c);
	}

	float Float3::Length() const noexcept
	{
		return Math::Norm(_c);
	}

	void Float3::Normalize() noexcept
	{
		Math::Normalize(_c);
	}

	void Float3::Set(const float x, const float y, const float z) noexcept
	{
		_x = x;
		_y = y;
		_z = z;
	}

	Float2 Float3::XY() const noexcept
	{
		return Float2(_x, _y);
	}
}
