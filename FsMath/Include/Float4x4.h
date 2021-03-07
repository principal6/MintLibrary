#pragma once


#ifndef FS_FLOAT4X4_H
#define FS_FLOAT4X4_H


#include <FsMath/Include/Float4.h>


namespace fs
{
#pragma region Forward declaration
	class Float3x3;
#pragma endregion


	// Mostly for column vector
	class Float4x4 final
	{
#pragma region Static Functions
	public:
		static Float4			mul(const Float4x4& m, const Float4& v) noexcept;
		static Float4x4			mul(const Float4x4& l, const Float4x4& r) noexcept;
			   
		static Float4x4			translationMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4			scalingMatrix(const float x, const float y, const float z) noexcept;
		static Float4x4			rotationMatrixX(const float angle) noexcept;
		static Float4x4			rotationMatrixY(const float angle) noexcept;
		static Float4x4			rotationMatrixZ(const float angle) noexcept;
		static Float4x4			rotationMatrixRollPitchYaw(const float pitch, const float yaw, const float roll) noexcept;

		// Rodrigues' rotation formula
		static Float4x4			rotationMatrixAxisAngle(const Float4& axis, const float angle) noexcept;
		static Float4x4			projectionMatrixPerspective(const float Fov, const float nearZ, const float farZ, const float ratio) noexcept;
		static Float4x4			projectionMatrix2DFromTopLeft(const float pixelWidth, const float pixelHeight) noexcept;
#pragma endregion

	public:
								Float4x4();
		explicit				Float4x4(const float s);
		explicit				Float4x4(
									const float m00, const float m01, const float m02, const float m03,
									const float m10, const float m11, const float m12, const float m13,
									const float m20, const float m21, const float m22, const float m23,
									const float m30, const float m31, const float m32, const float m33);
								Float4x4(const Float4x4& rhs)																= default;
								Float4x4(Float4x4&& rhs) noexcept															= default;
								~Float4x4()																					= default;

	public:
		Float4x4&				operator=(const Float4x4& rhs)																= default;
		Float4x4&				operator=(Float4x4&& rhs) noexcept															= default;

	public:
		Float4x4				operator+(const Float4x4& rhs) const noexcept;
		Float4x4				operator-(const Float4x4& rhs) const noexcept;

		Float4x4				operator*(const float s) const noexcept;
		Float4x4				operator/(const float s) const noexcept;
		// matrix(lhs) * matrix(rhs)
		Float4x4				operator*(const Float4x4& rhs) const noexcept;
		// matrix * (column) vector
		Float4					operator*(const Float4& v) const noexcept;

	public:
		void					set(
									float m00, float m01, float m02, float m03,
									float m10, float m11, float m12, float m13,
									float m20, float m21, float m22, float m23,
									float m30, float m31, float m32, float m33) noexcept;
		void					setZero() noexcept;
		void					setIdentity() noexcept;

	public:
		Float3x3				minor(const uint32 row, const uint32 col) const noexcept;
		const float				determinant() const noexcept;
		Float4x4				transpose() const noexcept;
		Float4x4				cofactor() const noexcept;
		Float4x4				adjugate() const noexcept;
		Float4x4				inverse() const noexcept;

	public:
		Float4x4				mul(const Float4x4& rhs) const noexcept;
		// matrix * (column) vector
		Float4					mul(const Float4& v) const noexcept;

	public:
		union
		{
			float					_m[4][4];
			fs::Float4				_row[4];
			struct
			{
				float				_11, _12, _13, _14;
				float				_21, _22, _23, _24;
				float				_31, _32, _33, _34;
				float				_41, _42, _43, _44;
			};
		};
	};
}


#endif // !FS_FLOAT4X4_H
