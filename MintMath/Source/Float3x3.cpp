#include <MintMath/Include/Float3x3.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Matrix.hpp>


namespace mint
{
	Float3x3 Float3x3::TranslationMatrix(const Float2& translation) noexcept
	{
		return Float3x3
		(
			1.0f, 0.0f, translation._x,
			0.0f, 1.0f, translation._y,
			0.0f, 0.0f, 1.0f
		);
	}

	Float3x3 Float3x3::ScalingMatrix(const Float2& scale) noexcept
	{
		return Float3x3
		(
			scale._x, 0.0f    , 0.0f,
			0.0f    , scale._y, 0.0f,
			0.0f    , 0.0f    , 1.0f
		);
	}

	Float3x3 Float3x3::RotationMatrixX(const float angle) noexcept
	{
		const float cosAngle = ::cos(angle);
		const float sinAngle = ::sin(angle);
		return Float3x3
		(
			+1.0f, 0.0f     , 0.0f     ,
			 0.0f, +cosAngle, -sinAngle,
			 0.0f, +sinAngle, +cosAngle
		);
	}

	Float3x3 Float3x3::RotationMatrixY(const float angle) noexcept
	{
		const float cosAngle = ::cos(angle);
		const float sinAngle = ::sin(angle);
		return Float3x3
		(
			+cosAngle,  0.0f, +sinAngle,
			0.0f     , +1.0f, 0.0f     ,
			-sinAngle,  0.0f, +cosAngle
		);
	}

	Float3x3 Float3x3::RotationMatrixZ(const float angle) noexcept
	{
		const float cosAngle = ::cos(angle);
		const float sinAngle = ::sin(angle);
		return Float3x3
		(
			+cosAngle, -sinAngle,  0.0f,
			+sinAngle, +cosAngle,  0.0f,
			0.0f     , 0.0f     , +1.0f
		);
	}

	Float3x3::Float3x3()
		: Float3x3(
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f)
	{
		__noop;
	}

	Float3x3::Float3x3(const float scalar)
		: Float3x3(
			scalar, scalar, scalar,
			scalar, scalar, scalar,
			scalar, scalar, scalar)
	{
		__noop;
	}

	Float3x3::Float3x3(
		const float _11, const float _12, const float _13,
		const float _21, const float _22, const float _23,
		const float _31, const float _32, const float _33)
		: _m{ _11, _12, _13, _21, _22, _23, _31, _32, _33 }
	{
		__noop;
	}

	Float3x3& Float3x3::operator*=(const Float3x3& rhs) noexcept
	{
		Math::Mul(_m, rhs._m, _m);
		return *this;
	}

	bool Float3x3::operator==(const Float3x3& rhs) const
	{
		return _row[0] == rhs._row[0] && _row[1] == rhs._row[1] && _row[2] == rhs._row[2];
	}

	Float3x3 Float3x3::operator*(const Float3x3& rhs) const noexcept
	{
		Float3x3 result;
		Math::Mul(_m, rhs._m, result._m);
		return result;
	}

	Float3 Float3x3::operator*(const Float3& vec) const noexcept
	{
		Float3 result;
		Math::Mul(_m, vec._c, result._c);
		return result;
	}

	Float3x3 Float3x3::operator*(const float scalar) const noexcept
	{
		Float3x3 result = *this;
		Math::SetMulMat(result._m, scalar);
		return result;
	}

	Float3x3 Float3x3::operator/(const float scalar) const noexcept
	{
		Float3x3 result = *this;
		Math::SetDivMat(result._m, scalar);
		return result;
	}

	void Float3x3::SetZero() noexcept
	{
		Math::SetZeroMat(_m);
	}

	void Float3x3::SetIdentity() noexcept
	{
		Math::SetIdentity(_m);
	}

	float Float3x3::Determinant() const noexcept
	{
		return Math::Determinant(_m);
	}

	Float3x3 Float3x3::Transpose() const noexcept
	{
		Float3x3 result;
		Math::Transpose(_m, result._m);
		return result;
	}

	Float3x3 Float3x3::Inverse() const noexcept
	{
		Float3x3 adj;
		Math::Adjugate(_m, adj._m);
		return adj / Math::Determinant(_m);
	}

	Float3 Float3x3::Mul(const Float3& vec) const noexcept
	{
		Float3 result;
		Math::Mul(_m, vec._c, result._c);
		return result;
	}

	void Float3x3::DecomposeSRT(Float2& outScale, Float3x3& outRotationMatrix, Float2& outTranslation) const
	{
		// SRT Matrix
		// 
		// | s_x * r_11  s_y * r_12  t_x |
		// | s_x * r_21  s_y * r_22  t_y |
		// | 0           0           1   |

		// s
		outScale._x = ::sqrtf((_11 * _11) + (_21 * _21));
		outScale._y = ::sqrtf((_12 * _12) + (_22 * _22));

		// r
		outRotationMatrix._11 = _11 / outScale._x;
		outRotationMatrix._21 = _21 / outScale._x;

		outRotationMatrix._12 = _12 / outScale._y;
		outRotationMatrix._22 = _22 / outScale._y;

		outRotationMatrix._13 = outRotationMatrix._23 = outRotationMatrix._31 = outRotationMatrix._32 = 0.0f;
		outRotationMatrix._33 = 1.0f;

		// t
		outTranslation._x = _13;
		outTranslation._y = _23;
	}
}
