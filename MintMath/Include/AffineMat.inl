#pragma once


namespace mint
{
#pragma region Free functions
	template<typename T>
	MINT_INLINE AffineMat<T> operator*(const T scalar, const AffineMat<T>& mat) noexcept
	{
		return mat * scalar;
	}

	template<typename T>
	MINT_INLINE AffineVec<T> operator*(const AffineVec<T>& rowVec, const AffineMat<T>& mat) noexcept
	{
		return AffineVec<T>(mat.GetCol(0).Dot(rowVec), mat.GetCol(1).Dot(rowVec), mat.GetCol(2).Dot(rowVec), mat.GetCol(3).Dot(rowVec));
	}

	template<typename T>
	MINT_INLINE AffineVec<T> operator*(const AffineMat<T>& mat, const AffineVec<T>& colVec) noexcept
	{
		return AffineVec<T>(mat.GetRow(0).Dot(colVec), mat.GetRow(1).Dot(colVec), mat.GetRow(2).Dot(colVec), mat.GetRow(3).Dot(colVec));
	}

	template<typename T>
	MINT_INLINE AffineMat<T> TranslationMatrix(const AffineVec<T>& translation) noexcept
	{
		T t[4];
		translation.Get(t);
		return AffineMat<T>
			(
				1, 0, 0, t[0],
				0, 1, 0, t[1],
				0, 0, 1, t[2],
				0, 0, 0, 1
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> ScalarMatrix(const T scalar) noexcept
	{
		return AffineMat<T>
			(
				scalar, 0, 0, 0,
				0, scalar, 0, 0,
				0, 0, scalar, 0,
				0, 0, 0, 1
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> ScalarMatrix(const AffineVec<T>& scalar) noexcept
	{
		T s[4];
		scalar.Get(s);
		return AffineMat<T>
			(
				s[0], 0, 0, 0,
				0, s[1], 0, 0,
				0, 0, s[2], 0,
				0, 0, 0, 1
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixX(const T angle) noexcept
	{
		return AffineMat<T>
			(
				1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, +::cos(angle), -::sin(angle), 0.0f,
				0.0f, +::sin(angle), +::cos(angle), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixY(const T angle) noexcept
	{
		return AffineMat<T>
			(
				+::cos(angle), 0.0f, +::sin(angle), 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				-::sin(angle), 0.0f, +::cos(angle), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixZ(const T angle) noexcept
	{
		return AffineMat<T>
			(
				+::cos(angle), -::sin(angle), 0.0f, 0.0f,
				+::sin(angle), +::cos(angle), 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixRollPitchYaw(const T pitch, const T yaw, const T roll) noexcept
	{
		return RotationMatrixY(yaw) * RotationMatrixX(pitch) * RotationMatrixZ(roll);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixAxisAngle(const AffineVec<T>& axis, const T angle) noexcept
	{
		// (v * r)r(1 - cosθ) + vcosθ + (r X v)sinθ

		T r[4];
		Normalize(axis).Get(r);
		const T c = cosf(angle);
		const T s = sinf(angle);

		const T rx = r[0];
		const T ry = r[1];
		const T rz = r[2];
		AffineMat<T> result
		(
			(1 - c) * rx * rx + c, (1 - c) * ry * rx - (rz * s), (1 - c) * rz * rx + (ry * s), 0,
			(1 - c) * rx * ry + (rz * s), (1 - c) * ry * ry + c, (1 - c) * rz * ry - (rx * s), 0,
			(1 - c) * rx * rz - (ry * s), (1 - c) * ry * rz + (rx * s), (1 - c) * rz * rz + c, 0,
			0, 0, 0, 1
		);
		return result;
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrixFromAxes(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
	{
		return AxesToColumns(axisX, axisY, axisZ);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> RotationMatrix(const Quaternion<T>& rotation) noexcept
	{
		T axis[3];
		T angle;
		rotation.GetAxisAngle(axis, angle);
		return RotationMatrixAxisAngle(AffineVec<T>(axis[0], axis[1], axis[2], 0), angle);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> AxesToColumns(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
	{
		T aX[4];
		T aY[4];
		T aZ[4];
		axisX.get(aX);
		axisY.get(aY);
		axisZ.get(aZ);
		return AffineMat<T>
			(
				aX[0], aY[0], aZ[0], 0,
				aX[1], aY[1], aZ[1], 0,
				aX[2], aY[2], aZ[2], 0,
				0, 0, 0, 1
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> AxesToRows(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept
	{
		T aX[4];
		T aY[4];
		T aZ[4];
		axisX.get(aX);
		axisY.get(aY);
		axisZ.get(aZ);
		return AffineMat<T>
			(
				aX[0], aX[1], aX[2], 0,
				aY[0], aY[1], aY[2], 0,
				aZ[0], aZ[1], aZ[2], 0,
				0, 0, 0, 1
				);
	}

	template<typename T>
	MINT_INLINE AffineMat<T> SRTMatrix(const AffineVec<T>& scale, const Quaternion<T>& rotation, const AffineVec<T>& translation) noexcept
	{
		// SRT matrix for column vector is like below:
		// SRT = T * R * S
		// which is the same as below..
		AffineMat<T> matrix = RotationMatrix(rotation);
		matrix.PreTranslate(translation._x, translation._y, translation._z);
		matrix.PostScale(scale._x, scale._y, scale._z);
		return matrix;
	}
#pragma endregion


	inline AffineMat<float>::AffineMat()
		: _rows{ AffineVecF(1, 0, 0, 0), AffineVecF(0, 1, 0, 0), AffineVecF(0, 0, 1, 0),AffineVecF(0, 0, 0, 1) }
	{
		__noop;
	}

	inline AffineMat<float>::AffineMat(const AffineVecF& row0, const AffineVecF& row1, const AffineVecF& row2, const AffineVecF& row3)
		: _rows{ row0, row1, row2, row3 }
	{
		__noop;
	}

	inline AffineMat<float>::AffineMat(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44)
		: _rows{ { _11, _12, _13, _14 }, { _21, _22, _23, _24 }, { _31, _32, _33, _34 }, { _41, _42, _43, _44 } }
	{
		__noop;
	}

	inline AffineMat<float>::AffineMat(const float(&mat)[4][4])
		: AffineMat(mat[0][0], mat[0][1], mat[0][2], mat[0][3], mat[1][0], mat[1][1], mat[1][2], mat[1][3], mat[2][0], mat[2][1], mat[2][2], mat[2][3], mat[3][0], mat[3][1], mat[3][2], mat[3][3])
	{
		__noop;
	}

	MINT_INLINE bool AffineMat<float>::operator==(const AffineMat& rhs) const noexcept
	{
		return (_rows[0] == rhs._rows[0]) && (_rows[1] == rhs._rows[1]) && (_rows[2] == rhs._rows[2]) && (_rows[3] == rhs._rows[3]);
	}

	MINT_INLINE bool AffineMat<float>::operator!=(const AffineMat& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator+() noexcept
	{
		return *this;
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator-() const noexcept
	{
		return AffineMat(-_rows[0], -_rows[1], -_rows[2], -_rows[3]);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator+(const AffineMat& rhs) const noexcept
	{
		return AffineMat(_rows[0] + rhs._rows[0], _rows[1] + rhs._rows[1], _rows[2] + rhs._rows[2], _rows[3] + rhs._rows[3]);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator-(const AffineMat& rhs) const noexcept
	{
		return AffineMat(_rows[0] - rhs._rows[0], _rows[1] - rhs._rows[1], _rows[2] - rhs._rows[2], _rows[3] - rhs._rows[3]);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator*(const AffineMat& rhs) const noexcept
	{
		return AffineMat
		(
			_rows[0].Dot(rhs.GetCol(0)), _rows[0].Dot(rhs.GetCol(1)), _rows[0].Dot(rhs.GetCol(2)), _rows[0].Dot(rhs.GetCol(3)),
			_rows[1].Dot(rhs.GetCol(0)), _rows[1].Dot(rhs.GetCol(1)), _rows[1].Dot(rhs.GetCol(2)), _rows[1].Dot(rhs.GetCol(3)),
			_rows[2].Dot(rhs.GetCol(0)), _rows[2].Dot(rhs.GetCol(1)), _rows[2].Dot(rhs.GetCol(2)), _rows[2].Dot(rhs.GetCol(3)),
			_rows[3].Dot(rhs.GetCol(0)), _rows[3].Dot(rhs.GetCol(1)), _rows[3].Dot(rhs.GetCol(2)), _rows[3].Dot(rhs.GetCol(3))
		);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator*(const float scalar) const noexcept
	{
		return AffineMat(_rows[0] * scalar, _rows[1] * scalar, _rows[2] * scalar, _rows[3] * scalar);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::operator/(const float scalar) const noexcept
	{
		return AffineMat(_rows[0] / scalar, _rows[1] / scalar, _rows[2] / scalar, _rows[3] / scalar);
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator+=(const AffineMat& rhs) noexcept
	{
		_rows[0] += rhs._rows[0];
		_rows[1] += rhs._rows[1];
		_rows[2] += rhs._rows[2];
		_rows[3] += rhs._rows[3];
		return *this;
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator-=(const AffineMat& rhs) noexcept
	{
		_rows[0] -= rhs._rows[0];
		_rows[1] -= rhs._rows[1];
		_rows[2] -= rhs._rows[2];
		_rows[3] -= rhs._rows[3];
		return *this;
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const AffineMat& rhs) noexcept
	{
		*this = (*this * rhs);
		return *this;
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator*=(const float scalar) noexcept
	{
		_rows[0] *= scalar;
		_rows[1] *= scalar;
		_rows[2] *= scalar;
		_rows[3] *= scalar;
		return *this;
	}

	MINT_INLINE AffineMat<float>& AffineMat<float>::operator/=(const float scalar) noexcept
	{
		_rows[0] /= scalar;
		_rows[1] /= scalar;
		_rows[2] /= scalar;
		_rows[3] /= scalar;
		return *this;
	}

	MINT_INLINE void AffineMat<float>::Set(float _11, float _12, float _13, float _14, float _21, float _22, float _23, float _24, float _31, float _32, float _33, float _34, float _41, float _42, float _43, float _44) noexcept
	{
		_rows[0].Set(_11, _12, _13, _14);
		_rows[1].Set(_21, _22, _23, _24);
		_rows[2].Set(_31, _32, _33, _34);
		_rows[3].Set(_41, _42, _43, _44);
	}

	MINT_INLINE void AffineMat<float>::SetZero() noexcept
	{
		_rows[0].Set(0, 0, 0, 0);
		_rows[1].Set(0, 0, 0, 0);
		_rows[2].Set(0, 0, 0, 0);
		_rows[3].Set(0, 0, 0, 0);
	}

	MINT_INLINE void AffineMat<float>::SetIdentity() noexcept
	{
		_rows[0].Set(1, 0, 0, 0);
		_rows[1].Set(0, 1, 0, 0);
		_rows[2].Set(0, 0, 1, 0);
		_rows[3].Set(0, 0, 0, 1);
	}

	MINT_INLINE void AffineMat<float>::SetRow(const int32 row, const AffineVecF& vec) noexcept
	{
		_rows[row] = vec;
	}

	MINT_INLINE void AffineMat<float>::SetCol(const int32 col, const AffineVecF& vec) noexcept
	{
		float v[4];
		vec.Get(v);
		_rows[0].SetComponent(col, v[0]);
		_rows[1].SetComponent(col, v[1]);
		_rows[2].SetComponent(col, v[2]);
		_rows[3].SetComponent(col, v[3]);
	}

	MINT_INLINE void AffineMat<float>::SetElement(const int32 row, const int32 col, const float scalar) noexcept
	{
		_rows[row].SetComponent(col, scalar);
	}

	MINT_INLINE void AffineMat<float>::Get(float(&mat)[4][4]) const noexcept
	{
		_rows[0].Get(mat[0]);
		_rows[1].Get(mat[1]);
		_rows[2].Get(mat[2]);
		_rows[3].Get(mat[3]);
	}

	MINT_INLINE const AffineVecF& AffineMat<float>::GetRow(const int32 row) const noexcept
	{
		return _rows[row];
	}

	MINT_INLINE AffineVecF AffineMat<float>::GetCol(const int32 col) const noexcept
	{
		return AffineVecF(_rows[0].GetComponent(col), _rows[1].GetComponent(col), _rows[2].GetComponent(col), _rows[3].GetComponent(col));
	}

	MINT_INLINE void AffineMat<float>::PreScale(const AffineVecF& scale) noexcept
	{
		float s[4];
		scale.Get(s);
		_rows[0] *= s[0];
		_rows[1] *= s[1];
		_rows[2] *= s[2];
	}

	MINT_INLINE void AffineMat<float>::PostScale(const AffineVecF& scale) noexcept
	{
		AffineVecF scaleW0 = scale;
		scaleW0.SetComponent(3, 0.0f);
		_rows[0] *= scaleW0;
		_rows[1] *= scaleW0;
		_rows[2] *= scaleW0;
		_rows[3] *= scaleW0;
	}

	MINT_INLINE void AffineMat<float>::PreTranslate(const AffineVecF& translation) noexcept
	{
		float t[4];
		translation.Get(t);
		_rows[0].AddComponent(3, t[0]);
		_rows[1].AddComponent(3, t[1]);
		_rows[2].AddComponent(3, t[2]);
	}

	MINT_INLINE void AffineMat<float>::PostTranslate(const AffineVecF& translation) noexcept
	{
		AffineVecF rowCopy = _rows[0];
		rowCopy.SetComponent(3, 0.0f);
		_rows[0].AddComponent(3, rowCopy.Dot(rowCopy));

		rowCopy = _rows[1];
		rowCopy.SetComponent(3, 0.0f);
		_rows[1].AddComponent(3, rowCopy.Dot(rowCopy));

		rowCopy = _rows[2];
		rowCopy.SetComponent(3, 0.0f);
		_rows[2].AddComponent(3, rowCopy.Dot(rowCopy));

		rowCopy = _rows[3];
		rowCopy.SetComponent(3, 0.0f);
		_rows[3].AddComponent(3, rowCopy.Dot(rowCopy));
	}

	MINT_INLINE void AffineMat<float>::DecomposeSRT(AffineVecF& outScale, AffineMat& outRotationMatrix, AffineVecF& outTranslation) const noexcept
	{
		// TODO: avoid nan in outRotationMatrix

		float m[4][4];
		Get(m);

		// SRT Matrix
		// 
		// | s_x * r_11  s_y * r_12  s_z * r_13  t_x |
		// | s_x * r_21  s_y * r_22  s_z * r_23  t_y |
		// | s_x * r_31  s_y * r_32  s_z * r_33  t_z |
		// | 0           0           0           1   |

		// s
		const float sx = ::sqrt((m[0][0] * m[0][0]) + (m[1][0] * m[1][0]) + (m[2][0] * m[2][0]));
		const float sy = ::sqrt((m[0][1] * m[0][1]) + (m[1][1] * m[1][1]) + (m[2][1] * m[2][1]));
		const float sz = ::sqrt((m[0][2] * m[0][2]) + (m[1][2] * m[1][2]) + (m[2][2] * m[2][2]));
		outScale.Set(sx, sy, sz, 1.0f);

		// r
		outRotationMatrix.SetIdentity();
		outRotationMatrix.SetRow(0, _rows[0] / outScale);
		outRotationMatrix.SetRow(1, _rows[1] / outScale);
		outRotationMatrix.SetRow(2, _rows[2] / outScale);
		outRotationMatrix.SetElement(0, 3, 0.0f);
		outRotationMatrix.SetElement(1, 3, 0.0f);
		outRotationMatrix.SetElement(2, 3, 0.0f);

		// t
		outTranslation.Set(m[0][3], m[1][3], m[2][3], 1.0f);

		// s !!
		outScale.SetComponent(3, 0.0f);
	}

	MINT_INLINE AffineMat<float> AffineMat<float>::Inverse() const noexcept
	{
		float m[4][4];
		_rows[0].Get(m[0]);
		_rows[1].Get(m[1]);
		_rows[2].Get(m[2]);
		_rows[3].Get(m[3]);

		float adj[4][4];
		Math::Adjugate(m, adj);
		const float det = Math::Determinant(m);
		AffineMat inv = AffineMat(adj);
		inv /= det;
		return inv;
	}
}
