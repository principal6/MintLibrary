#pragma once


#ifndef _MINT_MATH_FLOAT4_H_
#define _MINT_MATH_FLOAT4_H_


#include <MintMath/Include/Float3.h>


namespace mint
{
	class Float4x4;


	class alignas(16) Float4 final
	{
	public:
		static const Float4 kZero;

	public:
		constexpr Float4();
		constexpr explicit Float4(const float s);
		constexpr explicit Float4(const float x, const float y, const float z, const float w);
		constexpr explicit Float4(const Float2& rhs);
		constexpr explicit Float4(const Float3& rhs);
		constexpr explicit Float4(const Float3& rhs, const float w);
		constexpr Float4(const Float4& rhs) = default;
		constexpr Float4(Float4&& rhs) noexcept = default;
		~Float4() = default;

	public:
		Float4& operator=(const Float4& rhs) = default;
		Float4& operator=(Float4&& rhs) noexcept = default;

	public:
		Float4& operator+=(const Float4& rhs);
		Float4& operator-=(const Float4& rhs);
		Float4& operator*=(const float scalar);
		Float4& operator/=(const float scalar);

	public:
		const Float4& operator+() const noexcept;
		Float4 operator-() const noexcept;

	public:
		Float4 operator+(const Float4& rhs) const noexcept;
		Float4 operator-(const Float4& rhs) const noexcept;
		Float4 operator*(const float scalar) const noexcept;
		Float4 operator/(const float scalar) const noexcept;

	public:
		float& operator[](const uint32 index) noexcept;
		const float& operator[](const uint32 index) const noexcept;

	public:
		constexpr bool operator==(const Float4& rhs) const noexcept;
		constexpr bool operator!=(const Float4& rhs) const noexcept;

	public:
		static float Dot(const Float4& lhs, const Float4& rhs) noexcept;

		// in 3D affine space
		static Float4 Cross(const Float4& lhs, const Float4& rhs) noexcept;
		static Float4 CrossAndNormalize(const Float4& lhs, const Float4& rhs) noexcept;
		static Float4 Normalize(const Float4& in) noexcept;

	public:
		Float4& Transform(const Float4x4& matrix) noexcept;

	public:
		void Normalize() noexcept;

	public:
		void SetXYZ(const float x, const float y, const float z) noexcept;
		void SetXYZ(const Float3& rhs) noexcept;
		void SetXYZ(const Float4& rhs) noexcept;
		Float4 GetXYZ0() const noexcept;
		Float4 GetXYZ1() const noexcept;
		Float3 GetXYZ() const noexcept;
		Float2 GetXY() const noexcept;
		float GetMaxComponent() const noexcept;
		float GetMinComponent() const noexcept;

	public:
		float LengthSqaure() const noexcept;
		float Length() const noexcept;

	public:
		void Set(const float x, const float y, const float z, const float w) noexcept;
		void SetZero() noexcept;
		void SetNAN() noexcept;

	public:
		bool IsNAN() const noexcept;

	public:
		union
		{
			struct
			{
				float _x;
				float _y;
				float _z;
				float _w;
			};
			float _c[4];
		};

	};
}


#include <MintMath/Include/Float4.inl>


#endif // !_MINT_MATH_FLOAT4_H_
