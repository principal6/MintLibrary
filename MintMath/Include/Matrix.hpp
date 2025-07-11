#pragma once


#include <MintMath/Include/Matrix.h>

#include <MintMath/Include/VectorR.hpp>
#include <MintMath/Include/Quaternion.h>

#include <initializer_list>


namespace mint
{
	namespace Math
	{
		template<int32 M, int32 N, typename T>
		bool Equals(const T(&lhs)[M][N], const T(&rhs)[M][N], const T epsilon) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				const bool areRowsEqual = Math::Equals(lhs[row], rhs[row], epsilon);
				if (areRowsEqual == false)
				{
					return false;
				}
			}
			return true;
		}

		template<int32 M, int32 N, typename T>
		void CopyMat(const T(&src)[M][N], T(&dest)[M][N]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					dest[row][col] = src[row][col];
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetAddMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					lhs[row][col] += rhs[row][col];
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetSubMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					lhs[row][col] -= rhs[row][col];
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetMulMat(T(&mat)[M][N], const T scalar) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					mat[row][col] *= scalar;
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetDivMat(T(&mat)[M][N], const T scalar) noexcept
		{
			MINT_ASSERT(scalar != 0.0, "0 으로 나누려 합니다!");
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					mat[row][col] /= scalar;
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetNeg(T(&mat)[M][N]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					mat[row][col] = -mat[row][col];
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetRow(const T(&src)[N], T(&dest)[M][N], const int32 destRow) noexcept
		{
			for (int32 col = 0; col < N; ++col)
			{
				dest[destRow][col] = src[col];
			}
		}

		template<int32 M, int32 N, typename T>
		void GetRow(const T(&mat)[M][N], const int32 row, T(&out)[N]) noexcept
		{
			for (int32 col = 0; col < N; ++col)
			{
				out[col] = mat[row][col];
			}
		}

		template<int32 M, int32 N, typename T>
		void SetCol(const T(&src)[M], T(&dest)[M][N], const int32 destCol) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				dest[row][destCol] = src[row];
			}
		}

		template<int32 M, int32 N, typename T>
		void GetCol(const T(&mat)[M][N], const int32 col, T(&out)[M]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				out[row] = mat[row][col];
			}
		}

		template<int32 M, int32 N, int32 P, typename T>
		void Mul(const T(&lhs)[M][N], const T(&rhs)[N][P], T(&out)[M][P]) noexcept
		{
			T lhsRowVec[N];
			T rhsColVec[N];
			T rhsCopy[N][P];
			CopyMat(rhs, rhsCopy);
			for (int32 row = 0; row < M; ++row)
			{
				Math::GetRow(lhs, row, lhsRowVec);

				for (int32 col = 0; col < P; ++col)
				{
					Math::GetCol(rhsCopy, col, rhsColVec);

					out[row][col] = Math::Dot(lhsRowVec, rhsColVec);
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void Mul(const T(&mat)[M][N], const T(&vec)[N], T(&outVec)[M]) noexcept
		{
			T vecCopy[N];
			CopyVec(vec, vecCopy);

			for (int32 row = 0; row < M; ++row)
			{
				outVec[row] = Math::Dot(mat[row], vecCopy);
			}
		}

		template<int32 M, int32 N, typename T>
		void Mul(const T(&vec)[M], const T(&mat)[M][N], T(&outVec)[N]) noexcept
		{
			T vecCopy[M];
			CopyVec(vec, vecCopy);

			T matCol[M];
			for (int32 col = 0; col < N; ++col)
			{
				Math::GetCol(mat, col, matCol);

				outVec[col] = Math::Dot(vecCopy, matCol);
			}
		}

		template<int32 N, typename T>
		void SetIdentity(T(&inOut)[N][N]) noexcept
		{
			for (int32 row = 0; row < N; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					inOut[row][col] = static_cast<T>((row == col) ? 1 : 0);
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void SetZeroMat(T(&inOut)[M][N]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					inOut[row][col] = static_cast<T>(0);
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void Transpose(const T(&in)[M][N], T(&out)[N][M]) noexcept
		{
			for (int32 row = 0; row < M; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					out[col][row] = in[row][col];
				}
			}
		}

		template<int32 M, int32 N, typename T>
		void Minor(const T(&in)[M][N], const int32 row, const int32 col, T(&out)[M - 1][N - 1]) noexcept
		{
			int32 destRow = 0;
			for (int32 srcRow = 0; srcRow < M; ++srcRow)
			{
				if (row != srcRow)
				{
					int32 destCol = 0;
					for (int32 srcCol = 0; srcCol < M; ++srcCol)
					{
						if (col != srcCol)
						{
							out[destRow][destCol] = in[srcRow][srcCol];

							++destCol;
						}
					}

					++destRow;
				}
			}
		}

		template<int32 N, typename T>
		T Trace(const T(&mat)[N][N]) noexcept
		{
			T result{};
			for (int32 row = 0; row < N; ++row)
			{
				for (int32 col = 0; col < N; ++col)
				{
					result += mat[row][col];
				}
			}
			return result;
		}

		template<typename T>
		MINT_INLINE T Determinant(const T(&mat)[2][2]) noexcept
		{
			return mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
		}

		template<typename T>
		MINT_INLINE T Determinant(T _11, T _12, T _21, T _22) noexcept
		{
			return _11 * _22 - _12 * _21;
		}

		template<typename T>
		MINT_INLINE T Determinant(const T(&mat)[3][3]) noexcept
		{
			return mat[0][0] * (mat[1][1] * mat[2][2] - mat[1][2] * mat[2][1])
				- mat[0][1] * (mat[1][0] * mat[2][2] - mat[1][2] * mat[2][0])
				+ mat[0][2] * (mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0]);
		}

		template<typename T>
		MINT_INLINE T Determinant(T _11, T _12, T _13, T _21, T _22, T _23, T _31, T _32, T _33) noexcept
		{
			return _11 * (_22 * _33 - _23 * _32)
				- _12 * (_21 * _33 - _23 * _31)
				+ _13 * (_21 * _32 - _22 * _31);
		}

		template<typename T>
		MINT_INLINE T DeterminantOfMinor(const T(&mat)[3][3], const int32 row, const int32 col) noexcept
		{
			// 0 => 1 2
			// 1 => 0 2
			// 2 => 0 1
			const int32 r0 = (row ^ 1) & 1;
			const int32 r1 = (r0 ^ 1) & 1;
			const int32 c0 = (col ^ 1) & 1;
			const int32 c1 = (c0 ^ 1) & 1;
			return Determinant(mat[r0][c0], mat[r0][c1], mat[r1][c0], mat[r1][c1]);
		}

		template<typename T>
		MINT_INLINE T DeterminantOfMinor(const T(&mat)[4][4], const int32 row, const int32 col) noexcept
		{
			// 0 => 1 2 3
			// 1 => 0 2 3
			// 2 => 0 1 3
			// 3 => 0 1 2
			const int32 r0 = (row == 0) ? 1 : 0;
			const int32 r1 = ((row & 2) == 0) ? 2 : 1;
			const int32 r2 = (row == 3) ? 2 : 3;
			const int32 c0 = (col == 0) ? 1 : 0;
			const int32 c1 = ((col & 2) == 0) ? 2 : 1;
			const int32 c2 = (col == 3) ? 2 : 3;
			return Determinant(mat[r0][c0], mat[r0][c1], mat[r0][c2], mat[r1][c0], mat[r1][c1], mat[r1][c2], mat[r2][c0], mat[r2][c1], mat[r2][c2]);
		}

		template<typename T>
		MINT_INLINE T Determinant(const T(&mat)[4][4]) noexcept
		{
			return mat[0][0] * DeterminantOfMinor(mat, 0, 0) - mat[0][1] * DeterminantOfMinor(mat, 0, 1)
				+ mat[0][2] * DeterminantOfMinor(mat, 0, 2) - mat[0][3] * DeterminantOfMinor(mat, 0, 3);
		}

		// transpose of cofactor
		template<typename T>
		MINT_INLINE void Adjugate(const T(&in)[3][3], T(&out)[3][3]) noexcept
		{
			out[0][0] = +DeterminantOfMinor(in, 0, 0); out[1][0] = -DeterminantOfMinor(in, 0, 1); out[2][0] = +DeterminantOfMinor(in, 0, 2);
			out[0][1] = -DeterminantOfMinor(in, 1, 0); out[1][1] = +DeterminantOfMinor(in, 1, 1); out[2][1] = -DeterminantOfMinor(in, 1, 2);
			out[0][2] = +DeterminantOfMinor(in, 2, 0); out[1][2] = -DeterminantOfMinor(in, 2, 1); out[2][2] = +DeterminantOfMinor(in, 2, 2);
		}

		// transpose of cofactor
		template<typename T>
		MINT_INLINE void Adjugate(const T(&in)[4][4], T(&out)[4][4]) noexcept
		{
			out[0][0] = +DeterminantOfMinor(in, 0, 0); out[1][0] = -DeterminantOfMinor(in, 0, 1); out[2][0] = +DeterminantOfMinor(in, 0, 2); out[3][0] = -DeterminantOfMinor(in, 0, 3);
			out[0][1] = -DeterminantOfMinor(in, 1, 0); out[1][1] = +DeterminantOfMinor(in, 1, 1); out[2][1] = -DeterminantOfMinor(in, 1, 2); out[3][1] = +DeterminantOfMinor(in, 1, 3);
			out[0][2] = +DeterminantOfMinor(in, 2, 0); out[1][2] = -DeterminantOfMinor(in, 2, 1); out[2][2] = +DeterminantOfMinor(in, 2, 2); out[3][2] = -DeterminantOfMinor(in, 2, 3);
			out[0][3] = -DeterminantOfMinor(in, 3, 0); out[1][3] = +DeterminantOfMinor(in, 3, 1); out[2][3] = -DeterminantOfMinor(in, 3, 2); out[3][3] = +DeterminantOfMinor(in, 3, 3);
		}
	}


	template<typename T>
	constexpr T GetScalarZero() noexcept
	{
		MINT_NEVER;
		return T();
	}

	template<>
	constexpr float GetScalarZero() noexcept
	{
		return 0.0f;
	}

	template<>
	constexpr double GetScalarZero() noexcept
	{
		return 0.0;
	}

	template<typename T>
	constexpr T GetScalarOne() noexcept
	{
		MINT_NEVER;
		return T();
	}

	template<>
	constexpr float GetScalarOne() noexcept
	{
		return 1.0f;
	}

	template<>
	constexpr double GetScalarOne() noexcept
	{
		return 1.0;
	}


	template<int32 M, int32 N, typename T>
	inline Matrix<M, N, T>::Matrix()
		: _m{}
	{
		__noop;
	}

	template<int32 M, int32 N, typename T>
	inline Matrix<M, N, T>::Matrix(const std::initializer_list<T>& initializerList)
	{
		const int32 count = min(static_cast<int32>(initializerList.size()), M * N);
		const T* const first = initializerList.begin();
		for (int32 index = 0; index < count; ++index)
		{
			const int32 col = index % M;
			const int32 row = index / M;
			_m[row][col] = *(first + index);
		}
	}

	template<int32 M, int32 N, typename T>
	inline Matrix<M, N, T>::~Matrix()
	{
		__noop;
	}

	template<int32 M, int32 N, typename T>
	Matrix<M, N, T>::operator float() const requires (M == 1 && N == 1)
	{
		return _m[0][0];
	}

	template<int32 M, int32 N, typename T>
	inline bool Matrix<M, N, T>::operator==(const Matrix& rhs) const noexcept
	{
		return Math::Equals(_m, rhs._m);
	}

	template<int32 M, int32 N, typename T>
	inline bool Matrix<M, N, T>::operator!=(const Matrix& rhs) const noexcept
	{
		return !(*this == rhs);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const T scalar) noexcept
	{
		Math::SetMulMat(_m, scalar);
		return *this;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator/=(const T scalar) noexcept
	{
		Math::SetDivMat(_m, scalar);
		return *this;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator+=(const Matrix& rhs) noexcept
	{
		Math::SetAddMat(_m, rhs._m);
		return *this;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T>& Matrix<M, N, T>::operator-=(const Matrix& rhs) noexcept
	{
		Math::SetSubMat(_m, rhs._m);
		return *this;
	}

	template<int32 M, int32 N, typename T>
	inline Matrix<M, N, T>& Matrix<M, N, T>::operator*=(const Matrix<N, N, T>& rhs) noexcept
	{
		static_assert(M == N, "Power of non-square matrix!!!");
		Math::Mul(_m, rhs._m, _m);
		return *this;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator*(const T scalar) const noexcept
	{
		Matrix result = *this;
		result *= scalar;
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator/(const T scalar) const noexcept
	{
		Matrix result = *this;
		result /= scalar;
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator+(const Matrix& rhs) const noexcept
	{
		Matrix result = *this;
		result += rhs;
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator-(const Matrix& rhs) const noexcept
	{
		Matrix result = *this;
		result -= rhs;
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> Matrix<M, N, T>::operator-() const noexcept
	{
		Matrix result = *this;
		Math::SetNeg(result._m);
		return result;
	}

	template<int32 M, int32 N, typename T>
	template<int32 P>
	MINT_INLINE Matrix<M, P, T> Matrix<M, N, T>::operator*(const Matrix<N, P, T>& rhs) const noexcept
	{
		Matrix<M, P, T> result;
		Math::Mul(_m, rhs._m, result._m);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::SetElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
	{
		MINT_ASSERT((rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
		_m[rowIndex][columnIndex] = value;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::AddElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
	{
		MINT_ASSERT((rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
		_m[rowIndex][columnIndex] += value;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::MulElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept
	{
		MINT_ASSERT((rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
		_m[rowIndex][columnIndex] *= value;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE T Matrix<M, N, T>::GetElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept
	{
		MINT_ASSERT((rowIndex < static_cast<uint32>(M) && columnIndex < static_cast<uint32>(N)), "범위를 벗어난 접근입니다!");
		return _m[rowIndex][columnIndex];
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::SetRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept
	{
		if (rowIndex < static_cast<uint32>(M))
		{
			Math::SetRow(row._c, _m, rowIndex);
		}
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<N, T> Matrix<M, N, T>::GetRow(const uint32 rowIndex) const noexcept
	{
		MINT_ASSERT(rowIndex < static_cast<uint32>(M), "범위를 벗어난 접근입니다!");

		VectorR<N, T> result;
		Math::GetRow(_m, rowIndex, result._c);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::SetColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept
	{
		if (columnIndex < static_cast<uint32>(N))
		{
			Math::SetCol(column._c, _m, static_cast<int32>(columnIndex));
		}
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<M, T> Matrix<M, N, T>::GetColumn(const uint32 columnIndex) const noexcept
	{
		MINT_ASSERT(columnIndex < static_cast<uint32>(N), "범위를 벗어난 접근입니다!");

		VectorR<M, T> result;
		Math::GetCol(_m, columnIndex, result._c);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<N, T> Matrix<M, N, T>::MultiplyRowVector(const VectorR<M, T>& rowVector) const noexcept
	{
		VectorR<N, T> result;
		Math::Mul(rowVector._c, _m, result._c);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<M, T> Matrix<M, N, T>::MultiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept
	{
		VectorR<M, T> result;
		Math::Mul(_m, columnVector._c, result._c);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<N, M, T> Matrix<M, N, T>::Transpose() const noexcept
	{
		Matrix<N, M, T> result;
		Math::Transpose(_m, result._m);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE T Matrix<M, N, T>::Trace() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			MINT_LOG_ERROR("Tried to compute trace from a non-square matrix!");
		}

		return Math::Trace(_m);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M - 1, N - 1, T> Matrix<M, N, T>::Minor(const uint32 row, const uint32 col) const noexcept
	{
		Matrix<M - 1, N - 1, T> result;
		Math::Minor(_m, row, col, result._m);
		return result;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE constexpr bool Matrix<M, N, T>::IsSquareMatrix() const noexcept
	{
		return (M == N);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsDiagonalMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (columnIndex != rowIndex)
				{
					if (_m[rowIndex][columnIndex] != 0)
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsScalarMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		const T scale = _m[0][0];
		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? scale : 0))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsIdentityMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (_m[rowIndex][columnIndex] != ((columnIndex == rowIndex) ? 1 : 0))
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsZeroMatrix() const noexcept
	{
		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (_m[rowIndex][columnIndex] != 0)
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsSymmetricMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (rowIndex == columnIndex)
				{
					continue;
				}

				if (_m[rowIndex][columnIndex] != _m[columnIndex][rowIndex])
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsSkewSymmetricMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		for (int32 rowIndex = 0; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < N; ++columnIndex)
			{
				if (rowIndex == columnIndex)
				{
					continue;
				}

				if (_m[rowIndex][columnIndex] != -_m[columnIndex][rowIndex])
				{
					return false;
				}
			}
		}
		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsUpperTriangularMatrix() const noexcept
	{
		if (IsSquareMatrix() == false)
		{
			return false;
		}

		for (int32 rowIndex = 1; rowIndex < M; ++rowIndex)
		{
			for (int32 columnIndex = 0; columnIndex < rowIndex; ++columnIndex)
			{
				if (_m[rowIndex][columnIndex] != 0)
				{
					return false;
				}
			}
		}

		return true;
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE bool Matrix<M, N, T>::IsIdempotentMatrix() const noexcept
	{
		Matrix<M, N, T> squared = *this;
		squared *= *this;
		return (squared == *this);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::SetIdentity() noexcept
	{
		if (IsSquareMatrix() == false)
		{
			MINT_LOG_ERROR("Non-square matrix is set to identity!");
		}

		Math::SetIdentity(_m);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE void Matrix<M, N, T>::SetZero() noexcept
	{
		Math::SetZeroMat(_m);
	}


	template<int32 M, int32 N, typename T>
	MINT_INLINE Matrix<M, N, T> operator*(const T scalar, const Matrix<M, N, T>& matrix) noexcept
	{
		return (matrix * scalar);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<N, T> operator*(const VectorR<M, T>& rowVector, const Matrix<M, N, T>& matrix) noexcept
	{
		return matrix.MultiplyRowVector(rowVector);
	}

	template<int32 M, int32 N, typename T>
	MINT_INLINE VectorR<M, T> operator*(const Matrix<M, N, T>& matrix, const VectorR<N, T>& columnVector) noexcept
	{
		return matrix.MultiplyColumnVector(columnVector);
	}


	namespace MatrixUtils
	{
		template<int32 N, typename T>
		const Matrix<N, N, T>& Identity() noexcept
		{
			static bool isFirstTime = true;
			static Matrix<N, N, T> identityMatrix;
			if (isFirstTime)
			{
				identityMatrix.SetIdentity();
				isFirstTime = false;
			}
			return identityMatrix;
		}
	}
}
