#pragma once


#include <FsMath/Shared/stdafx.h>
#include <FsMath/Include/Float4x4.h>

#include <FsMath/Include/Float3x3.h>
#include <FsMath/Include/Float4.h>


namespace fs
{
	Float4 Float4x4::mul(const Float4x4& m, const Float4& v) noexcept
	{
		return m.mul(v);
	}

	Float4x4 Float4x4::mul(const Float4x4& l, const Float4x4& r) noexcept
	{
		return l.mul(r);
	}

	Float4x4 Float4x4::translationMatrix(const float x, const float y, const float z) noexcept
	{
		return Float4x4
		(
			1, 0, 0, x,
			0, 1, 0, y,
			0, 0, 1, z,
			0, 0, 0, 1
		);
	}

	Float4x4 Float4x4::scalingMatrix(const float x, const float y, const float z) noexcept
	{
		return Float4x4
		(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1
		);
	}

	Float4x4 Float4x4::rotationMatrixX(const float angle) noexcept
	{
		return Float4x4
		(
			1.0f			, 0.0f			, 0.0f			, 0.0f,
			0.0f			, +cosf(angle)	, -sinf(angle)	, 0.0f,
			0.0f			, +sinf(angle)	, +cosf(angle)	, 0.0f,
			0.0f			, 0.0f			, 0.0f			, 1.0f
		);
	}

	Float4x4 Float4x4::rotationMatrixY(const float angle) noexcept
	{
		return Float4x4
		(
			+cosf(angle)	, 0.0f			, +sinf(angle)	, 0.0f,
			0.0f			, 1.0f			, 0.0f			, 0.0f,
			-sinf(angle)	, 0.0f			, +cosf(angle)	, 0.0f,
			0.0f			, 0.0f			, 0.0f			, 1.0f
		);
	}

	Float4x4 Float4x4::rotationMatrixZ(const float angle) noexcept
	{
		return Float4x4
		(
			+cosf(angle)	, -sinf(angle)	, 0.0f			, 0.0f,
			+sinf(angle)	, +cosf(angle)	, 0.0f			, 0.0f,
			0.0f			, 0.0f			, 1.0f			, 0.0f,
			0.0f			, 0.0f			, 0.0f			, 1.0f
		);
	}

	Float4x4 Float4x4::rotationMatrixAxisAngle(const Float4& axis, const float angle) noexcept
	{
		// (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

		const Float4 r = Float4::normalize(Float4(axis._x, axis._y, axis._z, 0.0f));
		const float c = cosf(angle);
		const float s = sinf(angle);

		const float rx = r._x;
		const float ry = r._y;
		const float rz = r._z;
		Float4x4 result
		(
			(1 - c) * rx * rx  + c            , (1 - c) * ry * rx       - (rz * s), (1 - c) * rz * rx       + (ry * s), 0,
			(1 - c) * rx * ry       + (rz * s), (1 - c) * ry * ry  + c            , (1 - c) * rz * ry       - (rx * s), 0,
			(1 - c) * rx * rz       - (ry * s), (1 - c) * ry * rz       + (rx * s), (1 - c) * rz * rz  + c            , 0,
			0                                 , 0                                 , 0                                 , 1
		);
		return result;
	}

	Float4x4 Float4x4::projectionMatrixPerspective(const float Fov, const float nearZ, const float farZ, const float ratio) noexcept
	{
		const float a = 1.0f / (tanf(Fov) * ratio);
		const float b = 1.0f / (tanf(Fov));
		const float c = (-farZ) / (farZ - nearZ);
		const float d = (nearZ * farZ) / (farZ - nearZ);
		const float e = -1.0f;
		return Float4x4
		(
			a, 0, 0, 0,
			0, b, 0, 0,
			0, 0, c, d,
			0, 0, e, 0
		);
	}

	Float4x4 Float4x4::projectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept
	{
		return Float4x4
		(
			+2.0f / pixelWidth,  0.0f              , 0.0f, -1.0f,
			 0.0f             , -2.0f / pixelHeight, 0.0f, +1.0f,
			 0.0f             ,  0.0f              , 1.0f,  0.0f,
			 0.0f             ,  0.0f              , 0.0f,  1.0f
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

	Float4x4::Float4x4(const float s)
		: Float4x4(
			s, s, s, s,
			s, s, s, s,
			s, s, s, s,
			s, s, s, s
		)
	{
		__noop;
	}

	Float4x4::Float4x4(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33)
		: _m{
			m00, m01, m02, m03,
			m10, m11, m12, m13,
			m20, m21, m22, m23,
			m30, m31, m32, m33
		}
	{
		__noop;
	}

	Float4x4 Float4x4::operator+(const Float4x4& rhs) const noexcept
	{
		return Float4x4(
			_m[0][0] + rhs._m[0][0], _m[0][1] + rhs._m[0][1], _m[0][2] + rhs._m[0][2], _m[0][3] + rhs._m[0][3],
			_m[1][0] + rhs._m[1][0], _m[1][1] + rhs._m[1][1], _m[1][2] + rhs._m[1][2], _m[1][3] + rhs._m[1][3],
			_m[2][0] + rhs._m[2][0], _m[2][1] + rhs._m[2][1], _m[2][2] + rhs._m[2][2], _m[2][3] + rhs._m[2][3],
			_m[3][0] + rhs._m[3][0], _m[3][1] + rhs._m[3][1], _m[3][2] + rhs._m[3][2], _m[3][3] + rhs._m[3][3]
		);
	}

	Float4x4 Float4x4::operator-(const Float4x4& rhs) const noexcept
	{
		return Float4x4(
			_m[0][0] - rhs._m[0][0], _m[0][1] - rhs._m[0][1], _m[0][2] - rhs._m[0][2], _m[0][3] - rhs._m[0][3],
			_m[1][0] - rhs._m[1][0], _m[1][1] - rhs._m[1][1], _m[1][2] - rhs._m[1][2], _m[1][3] - rhs._m[1][3],
			_m[2][0] - rhs._m[2][0], _m[2][1] - rhs._m[2][1], _m[2][2] - rhs._m[2][2], _m[2][3] - rhs._m[2][3],
			_m[3][0] - rhs._m[3][0], _m[3][1] - rhs._m[3][1], _m[3][2] - rhs._m[3][2], _m[3][3] - rhs._m[3][3]
		);
	}

	Float4x4 Float4x4::operator*(const float s) const noexcept
	{
		return Float4x4(
			_m[0][0] * s, _m[0][1] * s, _m[0][2] * s, _m[0][3] * s,
			_m[1][0] * s, _m[1][1] * s, _m[1][2] * s, _m[1][3] * s,
			_m[2][0] * s, _m[2][1] * s, _m[2][2] * s, _m[2][3] * s,
			_m[3][0] * s, _m[3][1] * s, _m[3][2] * s, _m[3][3] * s
		);
	}

	Float4x4 Float4x4::operator/(const float s) const noexcept
	{
		return Float4x4(
			_m[0][0] / s, _m[0][1] / s, _m[0][2] / s, _m[0][3] / s,
			_m[1][0] / s, _m[1][1] / s, _m[1][2] / s, _m[1][3] / s,
			_m[2][0] / s, _m[2][1] / s, _m[2][2] / s, _m[2][3] / s,
			_m[3][0] / s, _m[3][1] / s, _m[3][2] / s, _m[3][3] / s
		);
	}

	Float4x4 Float4x4::operator*(const Float4x4& rhs) const noexcept
	{
		return mul(*this, rhs);
	}

	Float4 Float4x4::operator*(const Float4& v) const noexcept
	{
		return mul(*this, v);
	}

	void Float4x4::set(
		float m00, float m01, float m02, float m03, 
		float m10, float m11, float m12, float m13, 
		float m20, float m21, float m22, float m23, 
		float m30, float m31, float m32, float m33) noexcept
	{
		// row 0
		_m[0][0] = m00;
		_m[0][1] = m01;
		_m[0][2] = m02;
		_m[0][3] = m03;

		// row 1
		_m[1][0] = m10;
		_m[1][1] = m11;
		_m[1][2] = m12;
		_m[1][3] = m13;

		// row 2
		_m[2][0] = m20;
		_m[2][1] = m21;
		_m[2][2] = m22;
		_m[2][3] = m23;

		// row 3
		_m[3][0] = m30;
		_m[3][1] = m31;
		_m[3][2] = m32;
		_m[3][3] = m33;
	}

	void Float4x4::setZero() noexcept
	{
		memset(_m, 0, sizeof(float) * 16);
	}

	void Float4x4::setIdentity() noexcept
	{
		_m[0][0] = _m[1][1] = _m[2][2] = _m[3][3] = 1.0f;

		_m[0][1] = _m[0][2] = _m[0][3] = _m[1][0] = _m[1][2] = _m[1][3]
			= _m[2][0] = _m[2][1] = _m[2][3] = _m[3][0] = _m[3][1] = _m[3][2] = 0.0f;
	}

	Float3x3 Float4x4::minor(const uint32 row, const uint32 col) const noexcept
	{
		Float3x3 result;

		uint32 smallRow{};
		for (uint32 myRow = 0; myRow < 4; ++myRow)
		{
			if (myRow == row) continue;

			uint32 smallCol{};
			for (uint32 myCol = 0; myCol < 4; ++myCol)
			{
				if (myCol == col) continue;

				result._m[smallRow][smallCol] = _m[myRow][myCol];

				++smallCol;
			}

			++smallRow;
		}

		return result;
	}

	const float Float4x4::determinant() const noexcept
	{
		const float a = _m[0][0];
		const float b = _m[0][1];
		const float c = _m[0][2];
		const float d = _m[0][3];
		return a * minor(0, 0).determinant() - b * minor(0, 1).determinant() + c * minor(0, 2).determinant() - d * minor(0, 3).determinant();
	}

	Float4x4 Float4x4::transpose() const noexcept
	{
		return Float4x4
		(
			_m[0][0], _m[1][0], _m[2][0], _m[3][0],
			_m[0][1], _m[1][1], _m[2][1], _m[3][1],
			_m[0][2], _m[1][2], _m[2][2], _m[3][2],
			_m[0][3], _m[1][3], _m[2][3], _m[3][3]
		);
	}

	Float4x4 Float4x4::cofactor() const noexcept
	{
		return Float4x4
		(
			+minor(0, 0).determinant(), -minor(0, 1).determinant(), +minor(0, 2).determinant(), -minor(0, 3).determinant(),
			-minor(1, 0).determinant(), +minor(1, 1).determinant(), -minor(1, 2).determinant(), +minor(1, 3).determinant(),
			+minor(2, 0).determinant(), -minor(2, 1).determinant(), +minor(2, 2).determinant(), -minor(2, 3).determinant(),
			-minor(3, 0).determinant(), +minor(3, 1).determinant(), -minor(3, 2).determinant(), +minor(3, 3).determinant()
		);
	}

	Float4x4 Float4x4::adjugate() const noexcept
	{
		return cofactor().transpose();
	}

	Float4x4 Float4x4::inverse() const noexcept
	{
		return adjugate() / determinant();
	}

	Float4x4 Float4x4::mul(const Float4x4& rhs) const noexcept
	{
		return Float4x4
		(
			// row 0
			Float4::dotProductRaw(_m[0], rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
			Float4::dotProductRaw(_m[0], rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
			Float4::dotProductRaw(_m[0], rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
			Float4::dotProductRaw(_m[0], rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

			// row 1
			Float4::dotProductRaw(_m[1], rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
			Float4::dotProductRaw(_m[1], rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
			Float4::dotProductRaw(_m[1], rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
			Float4::dotProductRaw(_m[1], rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

			// row 2
			Float4::dotProductRaw(_m[2], rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
			Float4::dotProductRaw(_m[2], rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
			Float4::dotProductRaw(_m[2], rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
			Float4::dotProductRaw(_m[2], rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3]),

			// row 3
			Float4::dotProductRaw(_m[3], rhs._m[0][0], rhs._m[1][0], rhs._m[2][0], rhs._m[3][0]),
			Float4::dotProductRaw(_m[3], rhs._m[0][1], rhs._m[1][1], rhs._m[2][1], rhs._m[3][1]),
			Float4::dotProductRaw(_m[3], rhs._m[0][2], rhs._m[1][2], rhs._m[2][2], rhs._m[3][2]),
			Float4::dotProductRaw(_m[3], rhs._m[0][3], rhs._m[1][3], rhs._m[2][3], rhs._m[3][3])
		);
	}

	Float4 Float4x4::mul(const Float4& v) const noexcept
	{
		return Float4
		(
			// x'
			Float4::dotProductRaw(_m[0], v[0], v[1], v[2], v[3]),

			// y'
			Float4::dotProductRaw(_m[1], v[0], v[1], v[2], v[3]),

			// z'
			Float4::dotProductRaw(_m[2], v[0], v[1], v[2], v[3]),

			// w'
			Float4::dotProductRaw(_m[3], v[0], v[1], v[2], v[3])
		);
	}

}
