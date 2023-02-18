#pragma once


#ifndef _MINT_MATH_AFFINE_MAT_H_
#define _MINT_MATH_AFFINE_MAT_H_


#include <MintMath/Include/AffineVec.h>

#include <MintMath/Include/Matrix.h>
#include <MintMath/Include/Matrix.hpp>


namespace mint
{
	template<typename T>
	class AffineMat
	{
		AffineMat() = default;
	};


#pragma region Free functions
	template<typename T>
	AffineMat<T> operator*(const T scalar, const AffineMat<T>& mat) noexcept;

	// Row vector * Matrix Multiplication
	template<typename T>
	AffineVec<T> operator*(const AffineVec<T>& rowVec, const AffineMat<T>& mat) noexcept;

	// Matrix * Column vector Multiplication
	template <typename T>
	AffineVec<T> operator*(const AffineMat<T>& mat, const AffineVec<T>& colVec) noexcept;

	template<typename T>
	AffineMat<T> TranslationMatrix(const AffineVec<T>& translation) noexcept;

	template<typename T>
	AffineMat<T> ScalarMatrix(const T scalar) noexcept;

	template<typename T>
	AffineMat<T> ScalarMatrix(const AffineVec<T>& scalar) noexcept; // actually diagonal ...

	template<typename T>
	AffineMat<T> RotationMatrixX(const T angle) noexcept;

	template<typename T>
	AffineMat<T> RotationMatrixY(const T angle) noexcept;

	template<typename T>
	AffineMat<T> RotationMatrixZ(const T angle) noexcept;

	template<typename T>
	AffineMat<T> RotationMatrixRollPitchYaw(const T pitch, const T yaw, const T roll) noexcept;

	// Rodrigues' rotation formula
	template<typename T>
	AffineMat<T> RotationMatrixAxisAngle(const AffineVec<T>& axis, const T angle) noexcept;

	template<typename T>
	AffineMat<T> RotationMatrixFromAxes(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

	template<typename T>
	AffineMat<T> RotationMatrix(const Quaternion<T>& rotation) noexcept;

	template<typename T>
	AffineMat<T> AxesToColumns(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

	// Interprets vector from new basis.
	template<typename T>
	AffineMat<T> AxesToRows(const AffineVec<T>& axisX, const AffineVec<T>& axisY, const AffineVec<T>& axisZ) noexcept;

	template<typename T>
	AffineMat<T> SRTMatrix(const AffineVec<T>& scale, const Quaternion<T>& rotation, const AffineVec<T>& translation) noexcept;
#pragma endregion


	template<>
	class AffineMat<float>
	{
	public:
		AffineMat();
		AffineMat(const AffineVecF& row0, const AffineVecF& row1, const AffineVecF& row2, const AffineVecF& row3);
		AffineMat(
			float _11, float _12, float _13, float _14
			, float _21, float _22, float _23, float _24
			, float _31, float _32, float _33, float _34
			, float _41, float _42, float _43, float _44);
		AffineMat(const float(&mat)[4][4]);
		AffineMat(const AffineMat& rhs) = default;
		AffineMat(AffineMat&& rhs) noexcept = default;
		~AffineMat() = default;

	public:
		AffineMat& operator=(const AffineMat& rhs) = default;
		AffineMat& operator=(AffineMat&& rhs) noexcept = default;

	public:
		bool operator==(const AffineMat& rhs) const noexcept;
		bool operator!=(const AffineMat& rhs) const noexcept;

	public:
		AffineMat& operator+() noexcept;
		AffineMat operator-() const noexcept;

		AffineMat operator+(const AffineMat& rhs) const noexcept;
		AffineMat operator-(const AffineMat& rhs) const noexcept;
		AffineMat operator*(const AffineMat& rhs) const noexcept;
		AffineMat operator*(const float scalar) const noexcept;
		AffineMat operator/(const float scalar) const noexcept;

		AffineMat& operator+=(const AffineMat& rhs) noexcept;
		AffineMat& operator-=(const AffineMat& rhs) noexcept;
		AffineMat& operator*=(const AffineMat& rhs) noexcept;
		AffineMat& operator*=(const float scalar) noexcept;
		AffineMat& operator/=(const float scalar) noexcept;

	public:
		void Set(
			float _11, float _12, float _13, float _14
			, float _21, float _22, float _23, float _24
			, float _31, float _32, float _33, float _34
			, float _41, float _42, float _43, float _44) noexcept;
		void SetZero() noexcept;
		void SetIdentity() noexcept;
		void SetRow(const int32 row, const AffineVecF& vec) noexcept;
		void SetCol(const int32 col, const AffineVecF& vec) noexcept;
		void SetElement(const int32 row, const int32 col, const float scalar) noexcept;

	public:
		void Get(float(&mat)[4][4]) const noexcept;
		const AffineVecF& GetRow(const int32 row) const noexcept;
		AffineVecF GetCol(const int32 col) const noexcept;

	public:
		void PreScale(const AffineVecF& scale) noexcept;
		void PostScale(const AffineVecF& scale) noexcept;
		void PreTranslate(const AffineVecF& translation) noexcept;
		void PostTranslate(const AffineVecF& translation) noexcept;

	public:
		void SetTranslation(const AffineVecF& translation) noexcept { SetCol(3, translation); }
		AffineVecF GetTranslation() const noexcept { return GetCol(3); }

	public:
		void DecomposeSRT(AffineVecF& outScale, AffineMat& outRotationMatrix, AffineVecF& outTranslation) const noexcept;

	public:
		AffineMat Inverse() const noexcept;

	private:
		AffineVecF _rows[4];
	};


	// TODO: AffineMat<double>
}


#include <MintMath/Include/AffineMat.inl>


#endif // !_MINT_MATH_AFFINE_MAT_H_
