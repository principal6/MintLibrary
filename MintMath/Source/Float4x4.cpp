#include <MintMath/Include/Float4x4.h>

#include <MintMath/Include/Quaternion.h>
#include <MintMath/Include/VectorR.hpp>


namespace mint
{
	const Float4x4 Float4x4::kIdentity = Float4x4();

	Float4 Float4x4::Mul(const Float4x4& m, const Float4& v) noexcept
	{
		return m.Mul(v);
	}

	Float3 Float4x4::Mul(const Float4x4& m, const Float3& v) noexcept
	{
		return m.Mul(v);
	}

	Float4x4 Float4x4::Mul(const Float4x4& l, const Float4x4& r) noexcept
	{
		return l.Mul(r);
	}

	Float4x4 Float4x4::TranslationMatrix(const float x, const float y, const float z) noexcept
	{
		return Float4x4
		(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1
		);
	}

	Float4x4 Float4x4::TranslationMatrix(const Float3& translation) noexcept
	{
		return TranslationMatrix(translation._x, translation._y, translation._z);
	}

	Float4x4 Float4x4::ScalingMatrix(const float x, const float y, const float z) noexcept
	{
		return Float4x4
		(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		);
	}

	Float4x4 Float4x4::ScalingMatrix(const Float3& scale) noexcept
	{
		return ScalingMatrix(scale._x, scale._y, scale._z);
	}

	Float4x4 Float4x4::RotationMatrixX(const float angle) noexcept
	{
		return Float4x4
		(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, +cosf(angle), -sinf(angle), 0.0f,
			0.0f, +sinf(angle), +cosf(angle), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Float4x4 Float4x4::RotationMatrixY(const float angle) noexcept
	{
		return Float4x4
		(
			+cosf(angle), 0.0f, +sinf(angle), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			-sinf(angle), 0.0f, +cosf(angle), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Float4x4 Float4x4::RotationMatrixZ(const float angle) noexcept
	{
		return Float4x4
		(
			+cosf(angle), -sinf(angle), 0.0f, 0.0f,
			+sinf(angle), +cosf(angle), 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Float4x4 Float4x4::RotationMatrixRollPitchYaw(const float pitch, const float yaw, const float roll) noexcept
	{
		return RotationMatrixY(yaw) * RotationMatrixX(pitch) * RotationMatrixZ(roll);
	}

	Float4x4 Float4x4::RotationMatrixAxisAngle(const Float3& axis, const float angle) noexcept
	{
		// (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

		const Float3 r = Float3::Normalize(axis);
		const float c = cosf(angle);
		const float s = sinf(angle);

		const float rx = r._x;
		const float ry = r._y;
		const float rz = r._z;
		Float4x4 result
		(
			(1 - c) * rx * rx + c, (1 - c) * ry * rx - (rz * s), (1 - c) * rz * rx + (ry * s), 0,
			(1 - c) * rx * ry + (rz * s), (1 - c) * ry * ry + c, (1 - c) * rz * ry - (rx * s), 0,
			(1 - c) * rx * rz - (ry * s), (1 - c) * ry * rz + (rx * s), (1 - c) * rz * rz + c, 0,
			0, 0, 0, 1
		);
		return result;
	}

	Float4x4 Float4x4::RotationMatrixFromAxes(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
	{
		return AxesToColumns(axisX, axisY, axisZ);
	}

	Float4x4 Float4x4::RotationMatrix(const QuaternionF& rotation) noexcept
	{
		Float3 axis;
		float angle;
		rotation.GetAxisAngle(axis, angle);
		return RotationMatrixAxisAngle(axis, angle);
	}

	Float4x4 Float4x4::AxesToColumns(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
	{
		return Float4x4
		(
			axisX._x, axisY._x, axisZ._x, 0.0f,
			axisX._y, axisY._y, axisZ._y, 0.0f,
			axisX._z, axisY._z, axisZ._z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Float4x4 Float4x4::AxesToRows(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept
	{
		return Float4x4
		(
			axisX._x, axisX._y, axisX._z, 0.0f,
			axisY._x, axisY._y, axisY._z, 0.0f,
			axisZ._x, axisZ._y, axisZ._z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Float4x4 Float4x4::SRTMatrix(const Float3& scale, const QuaternionF& rotation, const Float3& translation) noexcept
	{
		// SRT matrix for column vector is like below:
		// SRT = T * R * S
		// which is the same as below..
		Float4x4 matrix = RotationMatrix(rotation);
		matrix.PreTranslate(translation._x, translation._y, translation._z);
		matrix.PostScale(scale._x, scale._y, scale._z);
		return matrix;
	}

	Float4x4 Float4x4::ProjectionMatrixPerspectiveYUP(const bool isRightHanded, const float FOVAngle, const float nearDepth, const float farDepth, const float screenWidthOverHeight) noexcept
	{
		const float halfFOVAngle = FOVAngle * 0.5f;
		const float b = -1.0f / ::tanf(halfFOVAngle);
		const float a = b / screenWidthOverHeight;
		const float c = (farDepth / (nearDepth - farDepth)) * (isRightHanded ? -1.0f : +1.0f);
		const float d = (isRightHanded ? +1.0f : -1.0f);
		const float e = (farDepth * nearDepth) / (nearDepth - farDepth);
		return Float4x4
		(
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, e,
			0, 0, d, 0
		);
	}

	Float4x4 Float4x4::ProjectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept
	{
		return Float4x4
		(
			+2.0f / pixelWidth, 0.0f, 0.0f, -1.0f,
			0.0f, -2.0f / pixelHeight, 0.0f, +1.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}
	
	Float4x4 Float4x4::ProjectionMatrix2DCentered(const float pixelWidth, const float pixelHeight) noexcept
	{
		return Float4x4
		(
			2.0f / pixelWidth, 0.0f              , 0.0f, 0.0f,
			0.0f             , 2.0f / pixelHeight, 0.0f, 0.0f,
			0.0f             , 0.0f              , 1.0f, 0.0f,
			0.0f             , 0.0f              , 0.0f, 1.0f
		);
	}

	Float4x4::Float4x4()
		: Float4x4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		)
	{
		__noop;
	}

	Float4x4::Float4x4(const float scalar)
		: Float4x4(
			scalar, scalar, scalar, scalar,
			scalar, scalar, scalar, scalar,
			scalar, scalar, scalar, scalar,
			scalar, scalar, scalar, scalar
		)
	{
		__noop;
	}

	Float4x4::Float4x4(
		const float _11, const float _12, const float _13, const float _14,
		const float _21, const float _22, const float _23, const float _24,
		const float _31, const float _32, const float _33, const float _34,
		const float _41, const float _42, const float _43, const float _44)
		: _m{
			_11, _12, _13, _14,
			_21, _22, _23, _24,
			_31, _32, _33, _34,
			_41, _42, _43, _44
	}
	{
		__noop;
	}

	Float4x4& Float4x4::operator*=(const Float4x4& rhs) noexcept
	{
		Math::Mul(_m, rhs._m, _m);
		return *this;
	}

	Float4x4 Float4x4::operator+(const Float4x4& rhs) const noexcept
	{
		Float4x4 result = *this;
		Math::SetAddMat(result._m, rhs._m);
		return result;
	}

	Float4x4 Float4x4::operator-(const Float4x4& rhs) const noexcept
	{
		Float4x4 result = *this;
		Math::SetSubMat(result._m, rhs._m);
		return result;
	}

	Float4x4 Float4x4::operator*(const float scalar) const noexcept
	{
		Float4x4 result = *this;
		Math::SetMulMat(result._m, scalar);
		return result;
	}

	Float4x4 Float4x4::operator/(const float scalar) const noexcept
	{
		Float4x4 result = *this;
		Math::SetDivMat(result._m, scalar);
		return result;
	}

	Float4x4 Float4x4::operator*(const Float4x4& rhs) const noexcept
	{
		return Mul(*this, rhs);
	}

	Float4 Float4x4::operator*(const Float4& vec) const noexcept
	{
		return Mul(*this, vec);
	}

	Float3 Float4x4::operator*(const Float3& vec) const noexcept
	{
		return Mul(*this, vec);
	}

	void Float4x4::Set(
		const float _11, const float _12, const float _13, const float _14,
		const float _21, const float _22, const float _23, const float _24,
		const float _31, const float _32, const float _33, const float _34,
		const float _41, const float _42, const float _43, const float _44) noexcept
	{
		// row 0
		_m[0][0] = _11;
		_m[0][1] = _12;
		_m[0][2] = _13;
		_m[0][3] = _14;

		// row 1
		_m[1][0] = _21;
		_m[1][1] = _22;
		_m[1][2] = _23;
		_m[1][3] = _24;

		// row 2
		_m[2][0] = _31;
		_m[2][1] = _32;
		_m[2][2] = _33;
		_m[2][3] = _34;

		// row 3
		_m[3][0] = _41;
		_m[3][1] = _42;
		_m[3][2] = _43;
		_m[3][3] = _44;
	}

	void Float4x4::SetZero() noexcept
	{
		Math::SetZeroMat(_m);
	}

	void Float4x4::SetIdentity() noexcept
	{
		Math::SetIdentity(_m);
	}

	void Float4x4::PreScale(const float x, const float y, const float z) noexcept
	{
		_row[0] *= x;
		_row[1] *= y;
		_row[2] *= z;
	}

	void Float4x4::PostScale(const float x, const float y, const float z) noexcept
	{
		_11 *= x;
		_21 *= x;
		_31 *= x;
		_41 *= x;

		_12 *= y;
		_22 *= y;
		_32 *= y;
		_42 *= y;

		_13 *= z;
		_23 *= z;
		_33 *= z;
		_43 *= z;
	}

	void Float4x4::PreScale(const Float3& scale) noexcept
	{
		PreScale(scale._x, scale._y, scale._z);
	}

	void Float4x4::PostScale(const Float3& scale) noexcept
	{
		PostScale(scale._x, scale._y, scale._z);
	}

	void Float4x4::SetTranslation(const float x, const float y, const float z) noexcept
	{
		_14 = x;
		_24 = y;
		_34 = z;
	}

	void Float4x4::SetTranslation(const Float3& translation) noexcept
	{
		_14 = translation._x;
		_24 = translation._y;
		_34 = translation._z;
	}

	void Float4x4::PreTranslate(const float x, const float y, const float z) noexcept
	{
		_14 += x;
		_24 += y;
		_34 += z;
	}

	void Float4x4::PostTranslate(const float x, const float y, const float z) noexcept
	{
		Float4 row = _row[0];
		_row[0]._w += row._x * x + row._y * y + row._z * z;
		row = _row[1];
		_row[1]._w += row._x * x + row._y * y + row._z * z;
		row = _row[2];
		_row[2]._w += row._x * x + row._y * y + row._z * z;
		row = _row[3];
		_row[3]._w += row._x * x + row._y * y + row._z * z;
	}

	void Float4x4::PreTranslate(const Float3& translation) noexcept
	{
		PreTranslate(translation._x, translation._y, translation._z);
	}

	void Float4x4::PostTranslate(const Float3& translation) noexcept
	{
		PostTranslate(translation._x, translation._y, translation._z);
	}

	Float3 Float4x4::GetTranslation() const noexcept
	{
		return Float3(_14, _24, _34);
	}

	void Float4x4::DecomposeSRT(Float3& outScale, Float4x4& outRotationMatrix, Float3& outTranslation) const noexcept
	{
		// TODO: avoid nan in outRotationMatrix


		// SRT Matrix
		// 
		// | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
		// | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
		// | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
		// | 0           0           0           1   |

		// s
		outScale._x = ::sqrtf((_11 * _11) + (_21 * _21) + (_31 * _31));
		outScale._y = ::sqrtf((_12 * _12) + (_22 * _22) + (_32 * _32));
		outScale._z = ::sqrtf((_13 * _13) + (_23 * _23) + (_33 * _33));

		// r
		outRotationMatrix._11 = _11 / outScale._x;
		outRotationMatrix._21 = _21 / outScale._x;
		outRotationMatrix._31 = _31 / outScale._x;

		outRotationMatrix._12 = _12 / outScale._y;
		outRotationMatrix._22 = _22 / outScale._y;
		outRotationMatrix._32 = _32 / outScale._y;

		outRotationMatrix._13 = _13 / outScale._z;
		outRotationMatrix._23 = _23 / outScale._z;
		outRotationMatrix._33 = _33 / outScale._z;

		outRotationMatrix._14 = outRotationMatrix._24 = outRotationMatrix._34 = outRotationMatrix._41 = outRotationMatrix._42 = outRotationMatrix._43 = 0.0f;
		outRotationMatrix._44 = 1.0f;

		// t
		outTranslation._x = _14;
		outTranslation._y = _24;
		outTranslation._z = _34;
	}

	float Float4x4::Determinant() const noexcept
	{
		return Math::Determinant(_m);
	}

	Float4x4 Float4x4::Transpose() const noexcept
	{
		Float4x4 result;
		Math::Transpose(_m, result._m);
		return result;
	}

	Float4x4 Float4x4::Inverse() const noexcept
	{
		Float4x4 adj;
		Math::Adjugate(_m, adj._m);
		return adj / Math::Determinant(_m);
	}

	Float4x4 Float4x4::Mul(const Float4x4& rhs) const noexcept
	{
		Float4x4 result;
		Math::Mul(_m, rhs._m, result._m);
		return result;
	}

	void Float4x4::MulAssignReverse(const Float4x4& lhs) noexcept
	{
		Float4x4 rhsCopy = *this;
		Math::Mul(lhs._m, rhsCopy._m, _m);
	}

	Float4 Float4x4::Mul(const Float4& vec) const noexcept
	{
		Float4 result;
		Math::Mul(_m, vec._c, result._c);
		return result;
	}

	Float3 Float4x4::Mul(const Float3& vec) const noexcept
	{
		Float4 v4 = Float4(vec._x, vec._y, vec._z, 0.0f);
		Float4 r4;
		Math::Mul(_m, v4._c, r4._c);
		return r4.GetXYZ();
	}
}
