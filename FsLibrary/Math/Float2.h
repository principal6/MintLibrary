#pragma once


#ifndef FS_FLOAT2_H
#define FS_FLOAT2_H


#include <CommonDefinitions.h>


namespace fs
{
	class Float2 final
	{
	public:
								Float2();
		explicit				Float2(const float s);
		explicit				Float2(const float x, const float y);
								Float2(const Float2& rhs)				= default;
								Float2(Float2&& rhs)					= default;
								~Float2()								= default;

	public:
		Float2&					operator=(const Float2& rhs)			= default;
		Float2&					operator=(Float2&& rhs)					= default;

	public:
		Float2&					operator+=(const Float2& rhs);		
		Float2&					operator-=(const Float2& rhs);
		Float2&					operator*=(const float s);
		Float2&					operator/=(const float s);

	public:
		const Float2			operator+(const Float2& rhs) const;
		const Float2			operator-(const Float2& rhs) const;
		const Float2			operator*(const float s) const;
		const Float2			operator/(const float s) const;

	public:
		float&					operator[](const uint32 index) noexcept;
		const float&			operator[](const uint32 index) const noexcept;

	public:
		const bool				operator==(const Float2& rhs) const noexcept;
		const bool				operator!=(const Float2& rhs) const noexcept;

	public:
		static const float		dot(const Float2& lhs, const Float2& rhs) noexcept;
		static const Float2		normalize(const Float2& float2) noexcept;

	public:
		const float				lengthSqaure() const noexcept;
		const float				length() const noexcept;
		
	public:
		void					set(const float x, const float y);
		void					x(const float newX) noexcept;
		const float				x() const noexcept;
		void					y(const float newY) noexcept;
		const float				y() const noexcept;

	private:
		float					_f[2];
	};
}


#endif // !FS_FLOAT2_H
