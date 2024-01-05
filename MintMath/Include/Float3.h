#pragma once


#ifndef _MINT_MATH_FLOAT3_H_
#define _MINT_MATH_FLOAT3_H_


#include <MintMath/Include/Float2.h>


namespace mint
{
	// No SIMD
	class Float3 final
	{
	public:
		static const Float3 kZero;
		static const Float3 kUnitScale;
		static const Float3 kAxisX;
		static const Float3 kAxisY;
		static const Float3 kAxisZ;

	public:
		constexpr Float3();
		constexpr explicit Float3(const float s);
		constexpr explicit Float3(const float x, const float y, const float z);
		constexpr explicit Float3(const Float2& xy);
		constexpr explicit Float3(const Float2& xy, const float z);
		constexpr Float3(const Float3& rhs) = default;
		constexpr Float3(Float3&& rhs) noexcept = default;
		~Float3() = default;

	public:
		Float3& operator=(const Float3& rhs) = default;
		Float3& operator=(Float3&& rhs) noexcept = default;

	public:
		Float3& operator+=(const Float3& rhs);
		Float3& operator-=(const Float3& rhs);
		Float3& operator*=(const float scalar);
		Float3& operator/=(const float scalar);

	public:
		const Float3& operator+() const noexcept;
		Float3 operator-() const noexcept;

	public:
		Float3 operator+(const Float3& rhs) const noexcept;
		Float3 operator-(const Float3& rhs) const noexcept;
		Float3 operator*(const float scalar) const noexcept;
		Float3 operator/(const float scalar) const noexcept;

	public:
		float& operator[](const uint32 index) noexcept;
		const float& operator[](const uint32 index) const noexcept;

	public:
		bool operator==(const Float3& rhs) const noexcept;
		bool operator!=(const Float3& rhs) const noexcept;

	public:
		static float Dot(const Float3& lhs, const Float3& rhs) noexcept;
		static Float3 Cross(const Float3& lhs, const Float3& rhs) noexcept;
		static Float3 CrossAndNormalize(const Float3& lhs, const Float3& rhs) noexcept;
		static Float3 Normalize(const Float3& in) noexcept;

	public:
		float Dot(const Float3& rhs) const noexcept;
		Float3 Cross(const Float3& rhs) const noexcept;
		float LengthSqaure() const noexcept;
		float Length() const noexcept;
		void Normalize() noexcept;

	public:
		void Set(const float x, const float y, const float z) noexcept;
		Float2 XY() const noexcept;

	public:
		union
		{
			struct
			{
				float _x;
				float _y;
				float _z;
			};
			float _c[3];
		};
	};
}


#include <MintMath/Include/Float3.inl>


#endif // !_MINT_MATH_FLOAT3_H_
