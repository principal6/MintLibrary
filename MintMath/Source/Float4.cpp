#include <MintMath/Include/Float4.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Float4x4.h>


namespace mint
{
	const Float4 Float4::kZero = Float4(0.0f);

	float& Float4::operator[](const uint32 index) noexcept
	{
		MINT_ASSERT(index < 4, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	const float& Float4::operator[](const uint32 index) const noexcept
	{
		MINT_ASSERT(index < 4, "범위를 벗어난 접근입니다.");
		return _c[index];
	}

	float Float4::Dot(const Float4& lhs, const Float4& rhs) noexcept
	{
		return Math::Dot(lhs._c, rhs._c);
	}

	Float4 Float4::Cross(const Float4& lhs, const Float4& rhs) noexcept
	{
		Float4 result;
		Math::Cross(lhs._c, rhs._c, result._c);
		return result;
	}

	Float4 Float4::CrossAndNormalize(const Float4& lhs, const Float4& rhs) noexcept
	{
		return Normalize(Cross(lhs, rhs));
	}

	Float4 Float4::Normalize(const Float4& in) noexcept
	{
		Float4 result = in;
		Math::Normalize(result._c);
		return result;
	}

	Float4& Float4::Transform(const Float4x4& matrix) noexcept
	{
		*this = matrix * (*this);
		return *this;
	}

	void Float4::Normalize() noexcept
	{
		Math::Normalize(_c);
	}

	void Float4::SetXYZ(const float x, const float y, const float z) noexcept
	{
		_x = x;
		_y = y;
		_z = z;
	}

	void Float4::SetXYZ(const Float3& rhs) noexcept
	{
		_x = rhs._x;
		_y = rhs._y;
		_z = rhs._z;
	}

	void Float4::SetXYZ(const Float4& rhs) noexcept
	{
		_x = rhs._x;
		_y = rhs._y;
		_z = rhs._z;
	}

	Float4 Float4::GetXYZ0() const noexcept
	{
		return Float4(_x, _y, _z, 0.0f);
	}

	Float4 Float4::GetXYZ1() const noexcept
	{
		return Float4(_x, _y, _z, 1.0f);
	}

	Float3 Float4::GetXYZ() const noexcept
	{
		return Float3(_x, _y, _z);
	}
	
	Float2 Float4::GetXY() const noexcept
	{
		return Float2(_x, _y);
	}

	float Float4::LengthSqaure() const noexcept
	{
		return Math::NormSq(_c);
	}

	float Float4::Length() const noexcept
	{
		return Math::Norm(_c);
	}

	void Float4::Set(const float x, const float y, const float z, const float w) noexcept
	{
		_x = x;
		_y = y;
		_z = z;
		_w = w;
	}

	void Float4::SetZero() noexcept
	{
		Math::SetZeroVec(_c);
	}

	bool Float4::IsNAN() const noexcept
	{
		return Math::IsNAN(_c);
	}

	void Float4::SetNAN() noexcept
	{
		Math::SetNAN(_c);
	}
}
