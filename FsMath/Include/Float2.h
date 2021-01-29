#pragma once


#ifndef FS_FLOAT2_H
#define FS_FLOAT2_H


#include <FsMath/Include/MathCommon.h>


namespace fs
{
#pragma region Forward declaration
	class Int2;
#pragma endregion


	// No SIMD
	class alignas(8) Float2 final
	{
	public:
		static const Float2		kZero;
		static const Float2		kOne;

	public:
		constexpr				Float2();
		constexpr explicit		Float2(const float s);
		constexpr explicit		Float2(const float x, const float y);
		explicit				Float2(const Int2& rhs);
		constexpr				Float2(const Float2& rhs)				= default;
		constexpr				Float2(Float2&& rhs) noexcept			= default;
								~Float2()								= default;

	public:
		Float2&					operator=(const Float2& rhs)			= default;
		Float2&					operator=(Float2&& rhs) noexcept		= default;

	public:
		Float2&					operator+=(const Float2& rhs);		
		Float2&					operator-=(const Float2& rhs);
		Float2&					operator*=(const float s);
		Float2&					operator/=(const float s);

	public:
		const Float2&			operator+() const noexcept;
		Float2					operator-() const noexcept;

	public:
		Float2					operator+(const Float2& rhs) const noexcept;
		Float2					operator-(const Float2& rhs) const noexcept;
		Float2					operator*(const float s) const noexcept;
		Float2					operator/(const float s) const noexcept;

	public:
		float&					operator[](const uint32 index) noexcept;
		const float&			operator[](const uint32 index) const noexcept;

	public:
		const bool				operator==(const Float2& rhs) const noexcept;
		const bool				operator!=(const Float2& rhs) const noexcept;

	public:
		static const float		dot(const Float2& lhs, const Float2& rhs) noexcept;
		static Float2			normalize(const Float2& float2) noexcept;
		static Float2			abs(const Float2& rhs) noexcept;

	public:
		const float				lengthSqaure() const noexcept;
		const float				length() const noexcept;
		const float				maxElement() const noexcept;
		const float				minElement() const noexcept;
		
	public:
		void					set(const float x, const float y);

	public:
		float					_x;
		float					_y;
	};
}


#include <FsMath/Include/Float2.inl>


#endif // !FS_FLOAT2_H
