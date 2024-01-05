#pragma once


#ifndef _MINT_MATH_TRANSFORM_H_
#define _MINT_MATH_TRANSFORM_H_


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
		Transform2D();
		Transform2D(const Float2& translation);
		Transform2D(float rotation, const Float2& translation);

		Transform2D operator*(const Transform2D& rhs) const;
		Transform2D& operator*=(const Transform2D& rhs);

		float _rotation;
		Float2 _translation;
	};
#pragma endregion
}


#endif // !_MINT_MATH_TRANSFORM_H_
