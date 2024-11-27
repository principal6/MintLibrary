#pragma once


#ifndef _MINT_MATH_TRANSFORM_H_
#define _MINT_MATH_TRANSFORM_H_


#include <MintMath/Include/Float3x3.h>
#include <MintMath/Include/Float4x4.h>
#include <MintMath/Include/Quaternion.h>


namespace mint
{
#pragma region Transform
	struct Transform
	{
	public:
		Transform();
		Transform(const Float3& translation);

	public:
		Float4x4 ToMatrix() const noexcept;

	public:
		Float3 _scale;
		QuaternionF _rotation;
		Float3 _translation;
	};
#pragma endregion

#pragma region Transform2D
	struct Transform2D
	{
	public:
		Transform2D();
		Transform2D(const Float2& translation);
		Transform2D(float rotation, const Float2& translation);
		Transform2D(const Float2& scale, float rotation, const Float2& translation);
		explicit Transform2D(const Float3x3& matrix);

	public:
		Transform2D operator*(const Transform2D& rhs) const;
		Transform2D& operator*=(const Transform2D& rhs);
		Float2 operator*(const Float2& v) const;
		Float3 operator*(const Float3& v) const;
	
	public:
		bool IsIdentity() const;
		Float3x3 ToMatrix() const;
		Transform2D GetInverted() const;

	public:
		Float2 _scale;
		float _rotation;
		Float2 _translation;
	};
#pragma endregion

	namespace Math
	{
		bool Equals(const Transform2D& lhs, const Transform2D& rhs, float epsilon = 0) noexcept;
	}
}


#endif // !_MINT_MATH_TRANSFORM_H_
