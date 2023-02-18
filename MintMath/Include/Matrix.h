#pragma once


#ifndef _MINT_MATH_MATRIX_H_
#define _MINT_MATH_MATRIX_H_


#include <MintCommon/Include/CommonDefinitions.h>

#include <MintMath/Include/VectorR.h>


namespace std
{
	template <class _Elem>
	class initializer_list;
}


namespace mint
{
	namespace Math
	{
		template <int32 M, int32 N, typename T>
		bool Equals(const T(&lhs)[M][N], const T(&rhs)[M][N], const T epsilon = 0) noexcept;

		template <int32 M, int32 N, typename T>
		void CopyMat(const T(&src)[M][N], T(&dest)[M][N]) noexcept;

		template <int32 M, int32 N, typename T>
		void SetAddMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept;

		template <int32 M, int32 N, typename T>
		void SetSubMat(T(&lhs)[M][N], const T(&rhs)[M][N]) noexcept;

		template <int32 M, int32 N, typename T>
		void SetMulMat(T(&mat)[M][N], const T scalar) noexcept;

		template <int32 M, int32 N, typename T>
		void SetDivMat(T(&mat)[M][N], const T scalar) noexcept;

		template <int32 M, int32 N, typename T>
		void SetRow(const T(&src)[N], T(&dest)[M][N], const int32 destRow) noexcept;

		template <int32 M, int32 N, typename T>
		void GetRow(const T(&mat)[M][N], const int32 row, T(&out)[N]) noexcept;

		template <int32 M, int32 N, typename T>
		void SetCol(const T(&src)[M], T(&dest)[M][N], const int32 destCol) noexcept;

		template <int32 M, int32 N, typename T>
		void GetCol(const T(&mat)[M][N], const int32 col, T(&out)[M]) noexcept;

		template <int32 M, int32 N, int32 P, typename T>
		void Mul(const T(&lhs)[M][N], const T(&rhs)[N][P], T(&out)[M][P]) noexcept;

		// mat * vec
		template <int32 M, int32 N, typename T>
		void Mul(const T(&lhsMat)[M][N], const T(&rhsVec)[N], T(&outVec)[M]) noexcept;

		// vec * mat
		template <int32 M, int32 N, typename T>
		void Mul(const T(&lhsVec)[M], const T(&rhsMat)[M][N], T(&outVec)[N]) noexcept;

		template <int32 N, typename T>
		void SetIdentity(T(&inOut)[N][N]) noexcept;

		template <int32 M, int32 N, typename T>
		void SetZeroMat(T(&inOut)[M][N]) noexcept;

		template <int32 M, int32 N, typename T>
		void Transpose(const T(&in)[M][N], T(&out)[N][M]) noexcept;

		template <int32 M, int32 N, typename T>
		void Minor(const T(&in)[M][N], const int32 row, const int32 col, T(&out)[M - 1][N - 1]) noexcept;

		template <int32 N, typename T>
		T Trace(const T(&mat)[N][N]) noexcept;

		template <typename T>
		T Determinant(const T(&mat)[2][2]) noexcept;

		template <typename T>
		T Determinant(const T(&mat)[3][3]) noexcept;

		template <typename T>
		T Determinant(T _11, T _12, T _13, T _21, T _22, T _23, T _31, T _32, T _33) noexcept;

		template <typename T>
		T Determinant(const T(&mat)[4][4]) noexcept;

		template<typename T>
		void Adjugate(const T(&in)[3][3], T(&out)[3][3]) noexcept;

		template<typename T>
		void Adjugate(const T(&in)[4][4], T(&out)[4][4]) noexcept;
	}


	template<typename T>
	class Quaternion;


	template<typename T>
	constexpr T GetScalarZero() noexcept;

	template<>
	constexpr float GetScalarZero() noexcept;

	template<>
	constexpr double GetScalarZero() noexcept;

	template<typename T>
	constexpr T GetScalarOne() noexcept;

	template<>
	constexpr float GetScalarOne() noexcept;

	template<>
	constexpr double GetScalarOne() noexcept;


	template <int32 M, int32 N, typename T>
	class Matrix
	{
		static_assert(M > 0, "M must be greater than 0!");
		static_assert(N > 0, "N must be greater than 0!");

	public:
		Matrix();
		Matrix(const std::initializer_list<T>& initializerList);
		Matrix(const Matrix& rhs) = default;
		Matrix(Matrix&& rhs) noexcept = default;
		~Matrix();

	public:
		Matrix& operator=(const Matrix& rhs) = default;
		Matrix& operator=(Matrix&& rhs) noexcept = default;

	public:
		bool operator==(const Matrix& rhs) const noexcept;
		bool operator!=(const Matrix& rhs) const noexcept;

	public:
		Matrix& operator*=(const T scalar) noexcept;
		Matrix& operator/=(const T scalar) noexcept;
		Matrix& operator+=(const Matrix& rhs) noexcept;
		Matrix& operator-=(const Matrix& rhs) noexcept;
		Matrix& operator*=(const Matrix<N, N, T>& rhs) noexcept;

	public:
		Matrix operator*(const T scalar) noexcept;
		Matrix operator/(const T scalar) noexcept;
		Matrix operator+(const Matrix& rhs) noexcept;
		Matrix operator-(const Matrix& rhs) noexcept;

	public:
		template <int32 P>
		Matrix<M, P, T> operator*(const Matrix<N, P, T>& rhs) const noexcept;

	public:
		void SetElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept;
		void AddElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept;
		void MulElement(const uint32 rowIndex, const uint32 columnIndex, const T value) noexcept;
		T GetElement(const uint32 rowIndex, const uint32 columnIndex) const noexcept;

	public:
		void SetRow(const uint32 rowIndex, const VectorR<N, T>& row) noexcept;
		VectorR<N, T> GetRow(const uint32 rowIndex) const noexcept;
		void SetColumn(const uint32 columnIndex, const VectorR<M, T>& column) noexcept;
		VectorR<M, T> GetColumn(const uint32 columnIndex) const noexcept;

	public:
		VectorR<N, T> MultiplyRowVector(const VectorR<M, T>& rowVector) const noexcept;
		VectorR<M, T> MultiplyColumnVector(const VectorR<N, T>& columnVector) const noexcept;

	public:
		Matrix<N, M, T> Transpose() const noexcept;
		T Trace() const noexcept;
		Matrix<M - 1, N - 1, T> Minor(const uint32 row, const uint32 col) const noexcept;

	public:
		constexpr bool IsSquareMatrix() const noexcept;
		bool IsDiagonalMatrix() const noexcept;
		bool IsScalarMatrix() const noexcept;
		bool IsIdentityMatrix() const noexcept;
		bool IsZeroMatrix() const noexcept;
		bool IsSymmetricMatrix() const noexcept;
		bool IsSkewSymmetricMatrix() const noexcept;
		bool IsUpperTriangularMatrix() const noexcept;
		bool IsIdempotentMatrix() const noexcept;

		void SetIdentity() noexcept;
		void SetZero() noexcept;

	public:
		T _m[M][N];
	};


	template <int32 M, int32 N, typename T>
	Matrix<M, N, T> operator*(const T scalar, const Matrix<M, N, T>& matrix) noexcept;

	// Row vector multiplication
	template <int32 M, int32 N, typename T>
	VectorR<N, T> operator*(const VectorR<M, T>& rowVector, const Matrix<M, N, T>& matrix) noexcept;

	// Column vector multiplication
	template <int32 M, int32 N, typename T>
	VectorR<M, T> operator*(const Matrix<M, N, T>& matrix, const VectorR<N, T>& columnVector) noexcept;


	template <int32 M, int32 N>
	using MatrixF = Matrix<M, N, float>;

	template <int32 M, int32 N>
	using MatrixD = Matrix<M, N, double>;


	namespace MatrixUtils
	{
		template<int32 N, typename T>
		const Matrix<N, N, T>& Identity() noexcept;
	}
}


#endif // !_MINT_MATH_MATRIX_H_
