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
		: Transform2D(Float2::kZero)
	{
		__noop;
	}

	Transform2D::Transform2D(const Float2& translation)
		: Transform2D(0.0f, translation)
	{
		__noop;
	}

	Transform2D::Transform2D(float rotation, const Float2& translation)
		: Transform2D(Float2::kOne, rotation, translation)
	{
		__noop;
	}

	Transform2D::Transform2D(const Float2& scale, float rotation, const Float2& translation)
		: _scale{ scale }
		, _rotation{ rotation }
		, _translation{ translation }
	{
		__noop;
	}
	
	Transform2D::Transform2D(const Float3x3& matrix)
	{
		Float3x3 rotationMatrix;
		matrix.DecomposeSRT(_scale, rotationMatrix, _translation);
		
		// RotationMatrix
		// | +cos  -sin  0 |
		// | +sin  +cos  0 |
		// | 0     0     1 |
		_rotation = ::acos(rotationMatrix._11);
	}

	Transform2D Transform2D::operator*(const Transform2D& rhs) const
	{
		// *this == parent, rhs == child
		// If _scale is not uniform, the result is different from matrix multiplication!!!
		Transform2D result;
		result._scale = _scale * rhs._scale;
		result._translation = _translation;
		result._translation += Float2x2::RotationMatrix(_rotation).Mul(_scale * rhs._translation);
		result._rotation = _rotation + rhs._rotation;
		return result;
	}

	Transform2D& Transform2D::operator*=(const Transform2D& rhs)
	{
		// *this == parent, rhs == child
		// If _scale is not uniform, the result is different from matrix multiplication!!!
		_scale *= rhs._scale;
		_translation += Float2x2::RotationMatrix(_rotation).Mul(_scale * rhs._translation);
		_rotation += rhs._rotation;
		return *this;
	}

	Float2 Transform2D::operator*(const Float2& v) const
	{
		return Float2x2::RotationMatrix(_rotation).Mul(_scale * v) + _translation;
	}

	Float3 Transform2D::operator*(const Float3& v) const
	{
		return Float3(Float2x2::RotationMatrix(_rotation).Mul(_scale * v.XY()), v._z) + Float3(_translation);
	}

	bool Transform2D::IsIdentity() const
	{
		return _scale == Float2::kOne && _rotation == 0.0f && _translation == Float2::kZero;
	}

	Float3x3 Transform2D::ToMatrix() const
	{
		//Float3x3 matrix = Float3x3::TranslationMatrix(_translation);
		//matrix *= Float3x3::RotationMatrixZ(_rotation);
		//matrix *= Float3x3::ScalingMatrix(_scale);
		Float3x3 matrix = Float3x3::RotationMatrixZ(_rotation);
		matrix._13 = _translation._x;
		matrix._23 = _translation._y;
		matrix._11 *= _scale._x;
		matrix._21 *= _scale._x;
		matrix._12 *= _scale._y;
		matrix._22 *= _scale._y;
		return matrix;
	}

	Transform2D Transform2D::GetInverted() const
	{
		return Transform2D(Float2::kOne / _scale, -_rotation, -_translation);
	}
#pragma endregion

	namespace Math
	{
		bool Equals(const Transform2D& lhs, const Transform2D& rhs, float epsilon) noexcept
		{
			if (Math::Equals(lhs._scale._c, rhs._scale._c, epsilon) == false)
				return false;
			if (Math::Equals(lhs._rotation, rhs._rotation, epsilon) == false)
				return false;
			if (Math::Equals(lhs._translation._c, rhs._translation._c, epsilon) == false)
				return false;

			return true;
		}
	}
}
