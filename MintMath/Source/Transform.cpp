#include <MintMath/Include/Transform.h>
#include <MintMath/Include/Float2x2.h>


namespace mint
{
#pragma region Transform
	Transform::Transform()
		: Transform(Float3::kZero)
	{
		__noop;
	}

	Transform::Transform(const Float3& translation)
		: _scale{ Float3::kUnitScale }
		, _translation{ translation }
	{
		__noop;
	}

	Float4x4 Transform::ToMatrix() const noexcept
	{
		return Float4x4::SRTMatrix(_scale, _rotation, _translation);
	}
#pragma endregion

#pragma region Transform2D
	Transform2D::Transform2D()
		: Transform2D(0.0f, Float2::kZero)
	{
		__noop;
	}

	Transform2D::Transform2D(const Float2& translation)
		: Transform2D(0.0f, translation)
	{
		__noop;
	}

	Transform2D::Transform2D(float rotation, const Float2& translation)
		: _rotation{ rotation }
		, _translation{ translation }
	{
		__noop;
	}

	Transform2D Transform2D::operator*(const Transform2D& rhs) const
	{
		// *this == parent, rhs == child
		Transform2D result;
		result._translation = _translation;
		result._translation += Float2x2::RotationMatrix(_rotation).Mul(rhs._translation);
		result._rotation = _rotation + rhs._rotation;
		return result;
	}

	Transform2D& Transform2D::operator*=(const Transform2D& rhs)
	{
		// *this == parent, rhs == child
		_translation += Float2x2::RotationMatrix(_rotation).Mul(rhs._translation);
		_rotation += rhs._rotation;
		return *this;
	}
#pragma endregion
}
