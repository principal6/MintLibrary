#pragma once


#ifndef _MINT_MATH_FLOAT3X3_H_
#define _MINT_MATH_FLOAT3X3_H_


#include <MintMath/Include/Float3.h>


namespace mint
{
	// 3x3 floating point matrix
	class Float3x3 final
	{
#pragma region Static Functions
	public:
		static Float3x3 TranslationMatrix(const Float2& translation) noexcept;
		static Float3x3 ScalingMatrix(const Float2& scale) noexcept;
		static Float3x3 RotationMatrixX(const float angle) noexcept;
		static Float3x3 RotationMatrixY(const float angle) noexcept;
		static Float3x3 RotationMatrixZ(const float angle) noexcept;
#pragma endregion

	public:
		Float3x3();
		explicit Float3x3(const float scalar);
		explicit Float3x3(
			const float _11, const float _12, const float _13,
			const float _21, const float _22, const float _23,
			const float _31, const float _32, const float _33);
		Float3x3(const Float3x3& rhs) = default;
		Float3x3(Float3x3&& rhs) noexcept = default;
		~Float3x3() = default;

	public:
		Float3x3& operator=(const Float3x3& rhs) = default;
		Float3x3& operator=(Float3x3&& rhs) noexcept = default;
	
	public:
		Float3x3& operator*=(const Float3x3& rhs) noexcept;

	public:
		bool operator==(const Float3x3& rhs) const;
		Float3x3 operator*(const Float3x3& rhs) const noexcept;
		Float3 operator*(const Float3& vec) const noexcept;
		Float3x3 operator*(const float scalar) const noexcept;
		Float3x3 operator/(const float scalar) const noexcept;

	public:
		void SetZero() noexcept;
		void SetIdentity() noexcept;

	public:
		float Determinant() const noexcept;
		Float3x3 Transpose() const noexcept;
		Float3x3 Inverse() const noexcept;

	public:
		Float3 Mul(const Float3& vec) const noexcept;

	public:
		union
		{
			float _m[3][3];
			Float3 _row[3];
			struct
			{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
			};
		};
	};
}


#endif // !_MINT_MATH_FLOAT3X3_H_
