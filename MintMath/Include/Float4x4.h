#pragma once


#ifndef _MINT_MATH_FLOAT4X4_H_
#define _MINT_MATH_FLOAT4X4_H_


#include <MintMath/Include/Float4.h>


namespace mint
{
#pragma region Forward declaration
	template<typename T>
	class Quaternion;
	using QuaternionF = Quaternion<float>;
#pragma endregion


	// Right-handed system
	// Mostly for column vector
	class Float4x4 final
	{
	public:
		static const Float4x4 kIdentity;

#pragma region Static Functions
	public:
		static Float4 Mul(const Float4x4& m, const Float4& v) noexcept;
		static Float3 Mul(const Float4x4& m, const Float3& v) noexcept;
		static Float4x4 Mul(const Float4x4& l, const Float4x4& r) noexcept;

		static Float4x4 TranslationMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4 TranslationMatrix(const Float3& translation) noexcept;
		static Float4x4 ScalingMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4 ScalingMatrix(const Float3& scale) noexcept;
		static Float4x4 RotationMatrixX(const float angle) noexcept;
		static Float4x4 RotationMatrixY(const float angle) noexcept;
		static Float4x4 RotationMatrixZ(const float angle) noexcept;
		static Float4x4 RotationMatrixRollPitchYaw(const float pitch, const float yaw, const float roll) noexcept;
		// Rodrigues' rotation formula
		static Float4x4 RotationMatrixAxisAngle(const Float3& axis, const float angle) noexcept;
		static Float4x4 RotationMatrixFromAxes(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		static Float4x4 RotationMatrix(const QuaternionF& rotation) noexcept;
		static Float4x4 AxesToColumns(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		// Interprets vector from new basis.
		static Float4x4 AxesToRows(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		static Float4x4 SRTMatrix(const Float3& scale, const QuaternionF& rotation, const Float3& translation) noexcept;

		// +X is the right direction for world.
		static Float4x4 ProjectionMatrixPerspectiveYUP(const bool isRightHanded, const float FOVAngle, const float nearDepth, const float farDepth, const float screenWidthOverHeight) noexcept;
		static Float4x4 ProjectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept;
		static Float4x4 ProjectionMatrix2DNormal(const float pixelWidth, const float pixelHeight) noexcept;
#pragma endregion

	public:
		Float4x4();
		explicit Float4x4(const float scalar);
		explicit Float4x4(
			const float _11, const float _12, const float _13, const float _14,
			const float _21, const float _22, const float _23, const float _24,
			const float _31, const float _32, const float _33, const float _34,
			const float _41, const float _42, const float _43, const float _44);
		Float4x4(const Float4x4& rhs) = default;
		Float4x4(Float4x4&& rhs) noexcept = default;
		~Float4x4() = default;

	public:
		Float4x4& operator=(const Float4x4& rhs) = default;
		Float4x4& operator=(Float4x4&& rhs) noexcept = default;

	public:
		Float4x4& operator*=(const Float4x4& rhs) noexcept;

	public:
		Float4x4 operator+(const Float4x4& rhs) const noexcept;
		Float4x4 operator-(const Float4x4& rhs) const noexcept;

		Float4x4 operator*(const float scalar) const noexcept;
		Float4x4 operator/(const float scalar) const noexcept;
		// matrix(lhs) * matrix(rhs)
		Float4x4 operator*(const Float4x4& rhs) const noexcept;
		// matrix * (column) vector
		Float4 operator*(const Float4& vec) const noexcept;
		// matrix * (column) vector
		Float3 operator*(const Float3& vec) const noexcept;

	public:
		void Set(
			const float _11, const float _12, const float _13, const float _14,
			const float _21, const float _22, const float _23, const float _24,
			const float _31, const float _32, const float _33, const float _34,
			const float _41, const float _42, const float _43, const float _44) noexcept;
		void SetZero() noexcept;
		void SetIdentity() noexcept;

	public:
		void PreScale(const float x, const float y, const float z) noexcept;
		void PostScale(const float x, const float y, const float z) noexcept;
		void PreScale(const Float3& scale) noexcept;
		void PostScale(const Float3& scale) noexcept;

	public:
		void SetTranslation(const float x, const float y, const float z) noexcept;
		void SetTranslation(const Float3& translation) noexcept;
		void PreTranslate(const float x, const float y, const float z) noexcept;
		void PostTranslate(const float x, const float y, const float z) noexcept;
		void PreTranslate(const Float3& translation) noexcept;
		void PostTranslate(const Float3& translation) noexcept;

	public:
		Float3 GetTranslation() const noexcept;
		void DecomposeSRT(Float3& outScale, Float4x4& outRotationMatrix, Float3& outTranslation) const noexcept;

	public:
		float Determinant() const noexcept;
		Float4x4 Transpose() const noexcept;
		Float4x4 Inverse() const noexcept;

	public:
		Float4x4 Mul(const Float4x4& rhs) const noexcept;
		void MulAssignReverse(const Float4x4& lhs) noexcept;

		// matrix * (column) vector
		Float4 Mul(const Float4& vec) const noexcept;
		// matrix * (column) vector
		Float3 Mul(const Float3& vec) const noexcept;

	public:
		union
		{
			float _m[4][4];
			Float4 _row[4];
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
		};
	};
}


#endif // !_MINT_MATH_FLOAT4X4_H_
