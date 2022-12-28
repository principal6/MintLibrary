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
		static Float4 mul(const Float4x4& m, const Float4& v) noexcept;
		static Float3 mul(const Float4x4& m, const Float3& v) noexcept;
		static Float4x4 mul(const Float4x4& l, const Float4x4& r) noexcept;

		static Float4x4 translationMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4 translationMatrix(const Float3& translation) noexcept;
		static Float4x4 scalingMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4 scalingMatrix(const Float3& scale) noexcept;
		static Float4x4 rotationMatrixX(const float angle) noexcept;
		static Float4x4 rotationMatrixY(const float angle) noexcept;
		static Float4x4 rotationMatrixZ(const float angle) noexcept;
		static Float4x4 rotationMatrixRollPitchYaw(const float pitch, const float yaw, const float roll) noexcept;
		// Rodrigues' rotation formula
		static Float4x4 rotationMatrixAxisAngle(const Float3& axis, const float angle) noexcept;
		static Float4x4 rotationMatrixFromAxes(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		static Float4x4 rotationMatrix(const QuaternionF& rotation) noexcept;
		static Float4x4 axesToColumns(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		// Interprets vector from new basis.
		static Float4x4 axesToRows(const Float3& axisX, const Float3& axisY, const Float3& axisZ) noexcept;
		static Float4x4 srtMatrix(const Float3& scale, const QuaternionF& rotation, const Float3& translation) noexcept;

		// +X is the right direction for world.
		static Float4x4 projectionMatrixPerspectiveYUP(const bool isRightHanded, const float FOVAngle, const float nearDepth, const float farDepth, const float screenWidthOverHeight) noexcept;
		static Float4x4 projectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept;
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
		void set(
			const float _11, const float _12, const float _13, const float _14,
			const float _21, const float _22, const float _23, const float _24,
			const float _31, const float _32, const float _33, const float _34,
			const float _41, const float _42, const float _43, const float _44) noexcept;
		void setZero() noexcept;
		void setIdentity() noexcept;

	public:
		void preScale(const float x, const float y, const float z) noexcept;
		void postScale(const float x, const float y, const float z) noexcept;
		void preScale(const Float3& scale) noexcept;
		void postScale(const Float3& scale) noexcept;

	public:
		void setTranslation(const float x, const float y, const float z) noexcept;
		void setTranslation(const Float3& translation) noexcept;
		void preTranslate(const float x, const float y, const float z) noexcept;
		void postTranslate(const float x, const float y, const float z) noexcept;
		void preTranslate(const Float3& translation) noexcept;
		void postTranslate(const Float3& translation) noexcept;

	public:
		Float3 getTranslation() const noexcept;
		void decomposeSRT(Float3& outScale, Float4x4& outRotationMatrix, Float3& outTranslation) const noexcept;

	public:
		float determinant() const noexcept;
		Float4x4 transpose() const noexcept;
		Float4x4 inverse() const noexcept;

	public:
		Float4x4 mul(const Float4x4& rhs) const noexcept;
		void mulAssignReverse(const Float4x4& lhs) noexcept;

		// matrix * (column) vector
		Float4 mul(const Float4& vec) const noexcept;
		// matrix * (column) vector
		Float3 mul(const Float3& vec) const noexcept;

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
